// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>

namespace IFC {
	struct ITIFC {
		ITIFC(flecs::world& world);

		static void Initialize(flecs::world& world);
	};

	constexpr const char* ITEM_CONTAINER = "::Container::Children";
}