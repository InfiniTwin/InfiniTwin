// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>
#include "ECS.h"
#include "ECSCore.h"

namespace IFC {
	struct IFCLayerFeature {
		static void RegisterComponents(flecs::world& world);
		static void CreateSystems(flecs::world& world);
	};
}