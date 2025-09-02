// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>
#include "IFC.h"
#include "LayerFeature.h"
#include "WidgetFeature.h"

namespace IFC {
	struct ITIFCLayerFeature {
		static void RegisterComponents(flecs::world& world);
		static void CreateQueries(flecs::world& world);
		static void CreateObservers(flecs::world& world);
		static void CreateSystems(flecs::world& world);
	};

	inline constexpr TCHAR SelectIfcDialogTitle[] = TEXT("IT::LoadIfcDialogTitle");
	inline constexpr TCHAR SelectIfcDialogFileType[] = TEXT("IFC 5 (*.ifcx)|*.ifcx");

	enum Enabled {};

	using namespace UI;
	struct QueryLayers { flecs::query<Layer, Id> Value; };
	struct QueryEnabledLayers { flecs::query<Enabled, Layer> Value; };
	struct QueryLayerCollections { flecs::query<Layer, Collection> Value; };

	struct TimerLoadIfcData { flecs::timer Value; };
}