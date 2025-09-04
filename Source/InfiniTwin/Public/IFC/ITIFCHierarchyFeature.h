// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>
#include "IFC.h"

namespace IFC {
	struct ITIFCHierarchyFeature {
		static void CreateComponents(flecs::world& world);
		static void CreateQueries(flecs::world& world);
		static void CreateObservers(flecs::world& world);
	};

	inline constexpr TCHAR TOKEN_CAN_TOGGLE_CHILDREN[] = TEXT("[CAN_TOGGLE_CHILDREN]");

	struct Selected {};

	using namespace UI;

	struct QueryRoots { flecs::query<> Value; };
	struct QueryRootCollections { flecs::query<> Value; };
	struct QuerySelectedIfcObjects { flecs::query<> Value; };
}