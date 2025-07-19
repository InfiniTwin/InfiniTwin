// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/IFCLayerFeature.h"
#include "IFC.h"
#include "ActionFeature.h"
#include "Assets.h"
#include "TypographyFeature.h"

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace IFC {
	void IFCLayerFeature::RegisterComponents(flecs::world& world) {
		using namespace ECS;

		world.component<Layer>().member<FString>(VALUE).add(flecs::OnInstantiate, flecs::Inherit);
	}

	void IFCLayerFeature::CreateSystems(flecs::world& world) {
		world.system<>("TriggerActionAddLayers")
			.with(Operation::Add)
			.with<Layer>()
			.with<Action>().id_flags(flecs::TOGGLE).with<Action>()
			.each([&world](flecs::entity action) {
			action.disable<Action>();

			auto layers = Assets::SelectFiles(
				UI::GetLocalizedText(world, SelectIfcDialogTitle),
				FPaths::ProjectContentDir(),
				SelectIfcDialogFileType);

			LoadIFCFiles(world, layers);
				});
	}
}
