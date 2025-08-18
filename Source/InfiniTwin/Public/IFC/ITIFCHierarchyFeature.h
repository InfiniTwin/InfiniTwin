// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>
#include "IFC.h"
#include "WidgetFeature.h"

namespace IFC {
	struct ITIFCHierarchyFeature {
		static void RegisterComponents(flecs::world& world);
		static void CreateQueries(flecs::world& world);
		static void CreateObservers(flecs::world& world);
	};

	constexpr const char* ITEM_CONTAINER = "::Container::Children";

	struct Selected {};

	using namespace UI;
	struct QueryHierarchies { flecs::query<Hierarchy> Value; };
	struct QueryHierarchyCollections { flecs::query<Collection, Hierarchy> Value; };
	struct QuerySelectedIFCData { flecs::query<IFCData, Selected> Value; };
}