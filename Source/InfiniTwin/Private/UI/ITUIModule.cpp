// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ITUIModule.h"
#include "UIElements.h"
#include "UI/ITUIFeature.h"
#include "UI/ITUIWidgetFeature.h"

namespace UIElements {
	ITUIModule::ITUIModule(flecs::world& world) {
		world.module<ITUIModule>();

		UIElements::Register(world);

		ITUIWidgetFeature::RegisterComponents(world);

		ITUIWidgetFeature::CreateObservers(world);

		ITUIFeature::Initialize(world);
	}
}