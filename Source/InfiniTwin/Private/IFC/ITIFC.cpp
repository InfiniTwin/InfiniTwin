// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFC.h"
#include "ECS.h"
#include "IFC.h"
#include "IFC/IFCLayerFeature.h"

namespace IFC {
	ITIFC::ITIFC(flecs::world& world) {
		world.module<ITIFC>();

		IFC::Register(world);

		IFCLayerFeature::RegisterComponents(world);

		IFCLayerFeature::CreateQueries(world);

		IFCLayerFeature::CreateObservers(world);
		
		IFCLayerFeature::CreateSystems(world);

		Initialize(world);
	}

	void ITIFC::Initialize(flecs::world& world) {
		ECS::Tokens.Add(TEXT("[IFC]"), IFC::Scope() = "IFC.ITIFC");
	}
}