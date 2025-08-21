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
		world.try_get_mut<TimerLoadIFCData>()->Value.start(); // Enable LoadIFCData system

		world.try_get<QueryIFCData>()->Value
			.each([](flecs::entity entity, IFCData) {
			entity.destruct();
		});
	}

	FString ExtractSlug(const FString& id) {
		FRegexPattern pattern(TEXT("/([^/]+)\\.ifcx$"));
		FRegexMatcher matcher(pattern, id);
		if (matcher.FindNext())
			return matcher.GetCaptureGroup(1);
		return FString();
	}

	void AddLayerUIItem(flecs::world& world, flecs::entity collection, flecs::entity layer) {
		auto path = FString(collection.path()) + TEXT(".") + FString(layer.name());
		RunScript(world, "UI/IFC", "ItemLayer", Tokens({
			TOKEN(TOKEN_PATH, NormalizedPath(path)),
			TOKEN(TOKEN_TARGET, IdString(layer.id())),
			TOKEN(TOKEN_TEXT, ExtractSlug(layer.try_get<Id>()->Value)) }));
	}

	void ITIFCLayerFeature::RegisterComponents(flecs::world& world) {
		world.component<Enabled>();

		world.component<TimerLoadIFCData>().member<float>(VALUE);
		world.set<TimerLoadIFCData>({ world.timer(COMPONENT(TimerLoadIFCData)).interval(0.01) });
		world.try_get_mut<TimerLoadIFCData>()->Value.stop();
	}

	void ITIFCLayerFeature::CreateQueries(flecs::world& world) {
		world.component<QueryLayers>();
		world.set(QueryLayers{
			world.query_builder<Layer, Id>(COMPONENT(QueryLayers))
			.cached().build() });

		world.component<QueryEnabledLayers>();
		world.set(QueryEnabledLayers{
			world.query_builder<Enabled, Layer>(COMPONENT(QueryEnabledLayers))
			.cached().build() });

		world.component<QueryLayerCollections>();
		world.set(QueryLayerCollections{
			world.query_builder<Layer, Collection>(COMPONENT(QueryLayerCollections))
			.cached().build() });
	};

	void ITIFCLayerFeature::CreateObservers(flecs::world& world) {
		world.observer<>("AddLayerUIOnItemCreate")
			.with<Layer>()
			.with<Id>()
			.event(flecs::OnAdd)
			.each([&world](flecs::entity layer) {
			world.try_get<QueryLayerCollections>()->Value.each([&world, &layer](flecs::entity collection, Layer, Collection) {
				AddLayerUIItem(world, collection, layer); });
		});

		world.observer<>("AddLayerUIOnCollectionCreate")
			.with<Layer>()
			.with<Collection>()
			.event(flecs::OnAdd)
			.each([&world](flecs::entity collection) {
			world.try_get<QueryLayers>()->Value.each([&world, &collection](flecs::entity layer, Layer, Id) {
				AddLayerUIItem(world, collection, layer); });
		});

		world.observer<>("SetLayerStateFromUI")
			.with<Enabled>().filter()
			.with<CheckBoxState>().second(flecs::Wildcard)
			.event(flecs::OnSet)
			.each([](flecs::entity checkBox) {
			checkBox.parent().each<UIOf>([&checkBox](flecs::entity layer) {
				if (checkBox.has(Checked)) layer.remove<Enabled>();
				else layer.add<Enabled>();
			});
		});

		world.observer<>("SetLayerUICheckBoxState")
			.with<Enabled>().filter()
			.with<CheckBox>()
			.event(flecs::OnAdd)
			.each([](flecs::entity checkBox) {
			auto parent = checkBox.parent();
			if (!parent.has<UIOf>(flecs::Wildcard)) return;
			auto target = parent.target<UIOf>();
			if (target.has<Layer>())
				checkBox.add(target.has<Enabled>() ? Unchecked : Checked);
		});

		world.observer<>("DestroyIFCDataOnLayerSwitch")
			.with<Layer>()
			.with<Enabled>()
			.event(flecs::OnAdd)
			.event(flecs::OnRemove)
			.run([&world](flecs::iter& it) {
			DestroyIFCData(world);
		});

		world.system<>("LoadIFCData")
			.tick_source(world.try_get<TimerLoadIFCData>()->Value)
			.each([&world]() {
			if (world.try_get<QueryIFCData>()->Value.count() > 0) return;

			world.try_get_mut<TimerLoadIFCData>()->Value.stop();

			TArray<flecs::entity> layers;
			world.try_get<QueryEnabledLayers>()->Value.each([&layers](flecs::entity layer, Enabled, Layer) {
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
					UTF8_TO_TCHAR(COMPONENT(Enabled))
				};
				AddLayers(world, paths, components);
			}
		});
	}
}
