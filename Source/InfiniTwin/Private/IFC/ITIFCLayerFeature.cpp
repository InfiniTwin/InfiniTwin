// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFCLayerFeature.h"
#include "IFC.h"
#include "ActionFeature.h"
#include "WidgetFeature.h"
#include "TypographyFeature.h"
#include "EFDCore.h"
#include "ButtonFeature.h"

namespace IFC {
	using namespace ECS;

	void DestroyIFCData(flecs::world& world) {
		world.try_get_mut<TimerLoadIFCData>()->Value.start();

		world.try_get<QueryIFCData>()->Value
			.each([](flecs::entity entity, IFCData) {
			entity.destruct();
				});
	}

	FString ExtractSlug(const FString& Input) {
		FRegexPattern pattern(TEXT("/([^/@\\s]+)@"));
		FRegexMatcher matcher(pattern, Input);
		if (matcher.FindNext())
			return matcher.GetCaptureGroup(1);
		return FString();
	}

	void AddLayerUI(flecs::world& world, flecs::entity collection, flecs::entity layer) {
		auto path = FString(collection.path()) + TEXT(".") + FString(layer.name());
		RunScript(world, "UI/IFC", "LayerItem", Tokens({
			TOKEN(ECS::PATH, ECS::NormalizedPath(path)),
			TOKEN(ECS::TARGET, ECS::NormalizedPath(layer.path().c_str())),
			TOKEN(TEXT, ExtractSlug(layer.try_get<Id>()->Value)) }));
	}

	void ITIFCLayerFeature::RegisterComponents(flecs::world& world) {
		using namespace ECS;
		world.component<LayerState>().add(flecs::Exclusive);

		world.component<TimerLoadIFCData>().member<float>(VALUE);
		world.set<TimerLoadIFCData>({ world.timer(COMPONENT(TimerLoadIFCData)).interval(0.01) });
		world.try_get_mut<TimerLoadIFCData>()->Value.stop();
	}

	void ITIFCLayerFeature::CreateQueries(flecs::world& world) {
		world.component<QueryLayer>();
		world.set(QueryLayer{
			world.query_builder<Layer, Id>(COMPONENT(QueryLayer))
			.cached().build() });

		world.component<QueryLayerEnabled>();
		world.set(QueryLayerEnabled{
			world.query_builder<Layer, Id>(COMPONENT(QueryLayerEnabled))
			.with(Enabled)
			.cached().build() });

		world.component<QueryCollectionLayer>();
		world.set(QueryCollectionLayer{
			world.query_builder<Collection, Layer>(COMPONENT(QueryCollectionLayer))
			.cached().build() });
	};

	void ITIFCLayerFeature::CreateObservers(flecs::world& world) {
		world.observer<>("AddLayerUIElement")
			.with<Layer>()
			.with<Id>()
			.event(flecs::OnAdd)
			.each([&world](flecs::entity layer) {
			world.try_get<QueryCollectionLayer>()->Value.each([&world, &layer](flecs::entity collection, Collection, Layer) {
				AddLayerUI(world, collection, layer); });
				});

		world.observer<>("AddCollectionLayerUIElement")
			.with<Layer>()
			.with<Collection>()
			.event(flecs::OnAdd)
			.each([&world](flecs::entity collection) {
			world.try_get<QueryLayer>()->Value.each([&world, &collection](flecs::entity layer, Layer, Id) {
				AddLayerUI(world, collection, layer); });
				});

		world.observer<>("SetLayerState")
			.with<CheckBoxState>().second(flecs::Wildcard)
			.event(flecs::OnSet)
			.each([](flecs::iter& it, size_t i) {
			auto checkBox = it.entity(i);
			checkBox.parent().each<UIOf>([&checkBox](flecs::entity layer) {
				layer.add(checkBox.has(Unchecked) ? Enabled : Disabled);
				});
				});

		world.observer<>("SetLayerUIState")
			.with<CheckBox>()
			.event(flecs::OnAdd)
			.each([](flecs::iter& it, size_t i) {
			auto checkBox = it.entity(i);
			auto parent = checkBox.parent();
			if (!parent.has<UIOf>(flecs::Wildcard)) return;
			auto target = parent.target<UIOf>();
			if (target.has<Layer>())
				checkBox.add(target.has(Enabled) ? Unchecked : Checked);
				});

		world.observer<>("RefreshIFCDataOnLayerStateChange")
			.with<LayerState>().second(flecs::Wildcard)
			.event(flecs::OnSet)
			.run([&world](flecs::iter& it) {
			DestroyIFCData(world);
				});

		world.observer<>("RefreshIFCDataOnLayerCreation")
			.with<Layer>()
			.with<Id>()
			.event(flecs::OnAdd)
			.run([&world](flecs::iter& it) {
			DestroyIFCData(world);
				});

		world.system<>("LoadIFCData")
			.tick_source(world.try_get<TimerLoadIFCData>()->Value)
			.each([&world]() {
			if (world.try_get<QueryIFCData>()->Value.count() > 0) return;

			world.try_get_mut<TimerLoadIFCData>()->Value.stop();

			TArray<flecs::entity> layers;
			world.try_get<QueryLayerEnabled>()->Value.each([&layers](flecs::entity layer, Layer, Id) {
				layers.Add(layer);
				});
			LoadIFCData(world, layers);
				});
	}

	void ITIFCLayerFeature::CreateSystems(flecs::world& world) {
		world.system<>("AddLayers")
			.with(Operation::Add)
			.with<Layer>()
			.with<Action>().id_flags(flecs::TOGGLE).with<Action>()
			.each([&world](flecs::entity action) {
			action.disable<Action>();

			TArray<FString> paths;
			if (EFDCore::OpenFileDialogCore(
				UI::GetLocalizedText(world, SelectIfcDialogTitle),
				FPaths::ProjectContentDir(),
				TEXT(""),
				SelectIfcDialogFileType,
				EEasyFileDialogFlags::Multiple,
				paths)) {
				TArray<FString> components = {
					UTF8_TO_TCHAR(COMPONENT(Layer)),
					FString::Printf(TEXT("(%s, %s)"), UTF8_TO_TCHAR(COMPONENT(LayerState)), UTF8_TO_TCHAR(COMPONENT(Enabled)))
				};
				AddLayers(world, paths, components);
			}
				});
	}
}
