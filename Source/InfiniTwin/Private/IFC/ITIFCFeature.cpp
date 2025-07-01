// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFCFeature.h"
#include <ECS.h>
#include <IFC.h>

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace IFC {
	void ITIFCFeature::Initialize(flecs::world& world) {
		ECS::Tokens.Add(TEXT("[IFC]"), IFC::Scope() = "IFC.ITIFC");

		using namespace ECS;
		RunScript(world, "IFC/HelloWall");
	}
}
