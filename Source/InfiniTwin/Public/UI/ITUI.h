// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>

namespace UI {
	struct ITUI {
		ITUI(flecs::world& world);

		static void Initialize(flecs::world& world);
	};

	struct UIOf{};
}