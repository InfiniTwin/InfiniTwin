// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>
#include "IFC.h"
#include "WidgetFeature.h"

namespace IFC {
	struct ITIFCHierarchyFeature {
		static void CreateQueries(flecs::world& world);
		static void CreateObservers(flecs::world& world);
	};

	constexpr const char* ITEM_CONTAINER = "::Container";

	using namespace UI;
	struct QueryHierarchy { flecs::query<Hierarchy> Value; };
	struct QueryCollectionHierarchy { flecs::query<Collection, Hierarchy> Value; };
}