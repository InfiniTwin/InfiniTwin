// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>

inline constexpr TCHAR ITUI[] = TEXT("ITUI");

namespace UIElements {
	//struct Delay {
	//	float RemainingTime;
	//	std::function<void()> Callback;
	//};

	struct ITUIFeature {
		static void Initialize(flecs::world& world);

	private:
		static void SetupViewport();
		//static void AwaitDelay(flecs::entity entity, float seconds, std::function<void()> onCompleted);
	};
}