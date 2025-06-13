// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ITUI.h"
#include "UIElements.h"
#include "UI/ITUIFeature.h"

namespace UI{
	ITUI::ITUI(flecs::world& world) {
		world.module<ITUI>();

		UI::Register(world);

		ITUIFeature::Initialize(world);
	}
}