// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>

inline constexpr TCHAR ITUIConfig[] = TEXT("ITUIConfig");

namespace UIElements {
	struct ITUIFeature {
		static void RegisterComponents(flecs::world& world);
		static void RegisterSystems(flecs::world& world);
		static void Initialize(flecs::world& world);

	private:
		static void SetupViewport();
	};

	struct Viewport {};
	struct Toolbar {};
}