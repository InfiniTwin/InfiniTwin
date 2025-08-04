// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/IFCLayerFeature.h"
#include "IFC.h"
#include "ActionFeature.h"
#include "WidgetFeature.h"
#include "TypographyFeature.h"
#include "EFDCore.h"

namespace IFC {
	using namespace ECS;

	void RefreshIFCData(flecs::world& world) {
		// DO>>> Clear IFC data
		TArray<flecs::entity> layers;
		world.try_get<QueryLayerEnabled>()->Value.each([&layers](flecs::entity layer, Layer, Id) {
			layers.Add(layer);
			});
		LoadIFCData(world, layers);
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

	void IFCLayerFeature::RegisterComponents(flecs::world& world) {
		world.component<LayerState>().add(flecs::Exclusive);
	}

	void IFCLayerFeature::CreateQueries(flecs::world& world) {
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

	void IFCLayerFeature::CreateObservers(flecs::world& world) {
		world.observer<>("AddCollectionLayerUIElement")
			.with<Layer>()
			.with<Collection>()
			.event(flecs::OnAdd)
			.yield_existing()
			.each([&world](flecs::entity collection) {
			world.try_get<QueryLayer>()->Value.each([&world, &collection](flecs::entity layer, Layer, Id) {
				AddLayerUI(world, collection, layer); });
				});

		world.observer<>("RefreshIFCDataOnLayerStateChange")
			.with<Layer>()
			.with<Id>()
			.with<LayerState>()
			.event(flecs::OnSet)
			.yield_existing()
			.run([&world](flecs::iter& it) {
			RefreshIFCData(world);
				});

		world.observer<>("RefreshIFCDataOnLayerCreation")
			.with<Layer>()
			.with<Id>()
			.event(flecs::OnAdd)
			.yield_existing()
			.run([&world](flecs::iter& it) {
			RefreshIFCData(world);
				});

		world.observer<>("AddLayerUIElement")
			.with<Layer>()
			.with<Id>()
			.event(flecs::OnAdd)
			.yield_existing()
			.each([&world](flecs::entity layer) {
			world.try_get<QueryCollectionLayer>()->Value.each([&world, &layer](flecs::entity collection, Collection, Layer) {
				AddLayerUI(world, collection, layer); });
				});
	}

	void IFCLayerFeature::CreateSystems(flecs::world& world) {
		world.system<>("TriggerActionAddLayers")
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
