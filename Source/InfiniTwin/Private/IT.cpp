// Fill out your copyright notice in the Description page of Project Settings.

#include "IT.h"
#include "ECSSubsystem.h"
#include "ECSCore.h"
#include "flecs.h"
#include "UI/ITUI.h"
#include "IFC/ITIFC.h"
#include "Tickable.h"

void UIT::Initialize(FSubsystemCollectionBase& Collection) {
	WaitForGameViewport([this] {
		UWorld* uWorld = GetWorld();
		UECSSubsystem* ecs = uWorld->GetGameInstance()->GetSubsystem<UECSSubsystem>();

		flecs::world& world = *ecs->World;
		world.set_ctx(uWorld);

		ecs->World->import<ECS::Core>();
		ecs->World->import<UI::ITUI>();
		ecs->World->import<IFC::ITIFC>();
		});

	Super::Initialize(Collection);
}

void UIT::Deinitialize() {
	Super::Deinitialize();
}

void UIT::WaitForGameViewport(TFunction<void()> Callback)
{
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Callback](float) -> bool {
		if (GEngine && GEngine->GameViewport) {
			Callback();
			return false;
		}
		return true;
		}));
}
