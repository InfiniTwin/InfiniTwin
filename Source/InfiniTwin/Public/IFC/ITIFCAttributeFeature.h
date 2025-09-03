// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>
#include "IFC.h"
#include "AttributeFeature.h"

namespace IFC {
	struct ITIFCAttributeFeature {
		static void CreateQueries(flecs::world& world);
		static void CreateObservers(flecs::world& world);
	};

	inline constexpr TCHAR TOKEN_NESTED_VALUE[] = TEXT("[NESTED_VALUE]");

	using namespace UI;

	struct QueryAttributes { flecs::query<> Value; };
	struct QueryAttributeCollections { flecs::query<> Value; };
}