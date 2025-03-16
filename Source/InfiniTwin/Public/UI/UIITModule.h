// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "flecs.h"

namespace UI {
	struct UIITModule {
	public:
		UIITModule(flecs::world& world);

	private:
		static void Initialize(flecs::world& world);
	};
}