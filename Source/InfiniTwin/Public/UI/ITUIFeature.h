// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>

inline constexpr TCHAR ITUIScope[] = TEXT("UIElements.ITUIModule");

namespace UIElements {
	struct ITUIFeature {
		static void Initialize(flecs::world& world);

	private:
		static void SetupViewport();
	};
}