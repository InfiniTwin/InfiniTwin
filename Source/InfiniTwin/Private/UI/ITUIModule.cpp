// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ITUIModule.h"
#include "UI/ITUIFeature.h"
#include "UIElements.h"

namespace UIElements {
	ITUIModule::ITUIModule(flecs::world& world) {
		world.module<ITUIModule>();

		UIElements::Register(world, "UIElements.ITUIModule");

		ITUIFeature::RegisterComponents(world);

		ITUIFeature::Initialize(world);
	}
}