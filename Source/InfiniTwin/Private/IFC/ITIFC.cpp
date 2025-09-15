// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFC.h"
#include "ECS.h"
#include "IFC.h"
#include "IFC/ITIFCLayerFeature.h"
#include "IFC/ITIFCHierarchyFeature.h"
#include "IFC/ITIFCAttributeFeature.h"
#include "IFC/ITIFCObjectFeature.h"

namespace IFC {
	ITIFC::ITIFC(flecs::world& world) {
		world.module<ITIFC>();

		IFC::Register(world);

		ITIFCLayerFeature::CreateComponents(world);
		ITIFCHierarchyFeature::CreateComponents(world);

		ITIFCLayerFeature::CreateQueries(world);
		ITIFCHierarchyFeature::CreateQueries(world);
		ITIFCAttributeFeature::CreateQueries(world);

		ITIFCLayerFeature::CreateObservers(world);
		ITIFCHierarchyFeature::CreateObservers(world);
		ITIFCAttributeFeature::CreateObservers(world);
		ITIFCObjectFeature::CreateObservers(world);

		ITIFCLayerFeature::CreateSystems(world);

		Initialize(world);
	}

	void ITIFC::Initialize(flecs::world& world) {
		ECS::Scopes.Add(TEXT("[IFC]"), IFC::Scope() = "IFC.ITIFC");
	}
}