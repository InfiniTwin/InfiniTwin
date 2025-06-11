// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ITUI.h"
#include "UIElements.h"
#include "UI/ITUIFeature.h"
#include "UI/ITUIWidgetFeature.h"

namespace UI{
	ITUI::ITUI(flecs::world& world) {
		world.module<ITUI>();

		UI::Register(world);

		ITUIWidgetFeature::RegisterComponents(world);

		//ITUIWidgetFeature::CreateObservers(world);

		ITUIFeature::Initialize(world);
	}
}