// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/IFCLayerFeature.h"
#include "IFC.h"
#include "ActionFeature.h"
#include "WidgetFeature.h"
#include "TypographyFeature.h"
#include "EFDCore.h"

namespace IFC {
	using namespace ECS;

	FString ExtractSlug(const FString& Input) {
		FRegexPattern pattern(TEXT("/([^/@\\s]+)@"));
		FRegexMatcher matcher(pattern, Input);
		if (matcher.FindNext())
			return matcher.GetCaptureGroup(1);
		return FString();
	}

	void IFCLayerFeature::RegisterComponents(flecs::world& world) {}

	void IFCLayerFeature::CreateQueries(flecs::world& world) {
		world.component<QueryCollectionLayer>();
		world.set(QueryCollectionLayer{
			world.query_builder<Layer>(COMPONENT(QueryCollectionLayer))
			.with<Collection>()
			.cached().build() });
	};

	void IFCLayerFeature::CreateObservers(flecs::world& world) {
		world.observer<>("SetupLayerUIElement")
			.with<Layer>()
			.with<Id>()
			.event(flecs::OnAdd)
			.yield_existing()
			.each([&world](flecs::entity layer) {
			world.try_get<QueryCollectionLayer>()->Value.each([&world, &layer](flecs::entity collection, Layer) {
				auto path = FString(collection.path()) + TEXT(".") + FString(layer.name());
				RunScript(world, "UI/IFC", "LayerItem", Tokens({
					TOKEN(ECS::PATH, ECS::NormalizedPath(path)),
					TOKEN(ECS::TARGET, ECS::NormalizedPath(layer.path().c_str())),
					TOKEN(TEXT, ExtractSlug(layer.try_get<Id>()->Value)) }));
				});
				});
	}

	void IFCLayerFeature::CreateSystems(flecs::world& world) {
		world.system<>("TriggerActionAddLayers")
			.with(Operation::Add)
			.with<Layer>()
			.with<Action>().id_flags(flecs::TOGGLE).with<Action>()
			.each([&world](flecs::entity action) {
			action.disable<Action>();

			const FString dialogTitle = UI::GetLocalizedText(world, SelectIfcDialogTitle);
			const FString defaultPath = FPaths::ProjectContentDir();
			const FString defaultFile = TEXT("");
			const FString fileTypes = SelectIfcDialogFileType;
			const uint32 flags = EEasyFileDialogFlags::Multiple;

			TArray<FString> layers;
			if (EFDCore::OpenFileDialogCore(dialogTitle, defaultPath, defaultFile, fileTypes, flags, layers))
				LoadIFCFiles(world, layers);
				});
	}
}
