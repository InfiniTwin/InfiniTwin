// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/IFCLayerFeature.h"
#include "IFC.h"
#include "ActionFeature.h"
#include "TypographyFeature.h"
#include "EFDCore.h"

namespace IFC {
	void IFCLayerFeature::RegisterComponents(flecs::world& world) {
		using namespace ECS;
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
