// Fill out your copyright notice in the Description page of Project Settings.

#include "IT.h"
#include "ECSSubsystem.h"
#include "flecs.h"
#include "ECS.h"
#include "UI/ITUIModule.h"
#include "UI/ITUIFeature.h"
#include "Tickable.h"

#define NEXTFRAME(block) \
{ \
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([&](float) { block; return false; })); \
}

#define NEXTFRAMES(frames, block) \
{ \
    int32 RemainingFrames = frames; \
    FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([&](float) mutable -> bool { \
        if (--RemainingFrames <= 0) { \
            block; \
            return false; /* Remove ticker */ \
        } \
        return true; /* Keep ticking */ \
    })); \
}

#define DELAY(time, block) \
{ \
    FTimerHandle TimerHandle; \
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()block, time, false); \
}

void UIT::Initialize(FSubsystemCollectionBase& Collection) {
	WaitForGameViewport([this] {
		UECSSubsystem* ecs = GetWorld()->GetGameInstance()->GetSubsystem<UECSSubsystem>();
		ecs->World->import<UIElements::ITUIModule>();

		NEXTFRAMES(100, {
			UECSSubsystem * ecs = GetWorld()->GetGameInstance()->GetSubsystem<UECSSubsystem>();
			ECS::FromJsonAsset(*ecs->World, "UISettings", UIElements::Scope);
			});
		});

	Super::Initialize(Collection);
}

void UIT::Deinitialize() {
	Super::Deinitialize();
}

void UIT::WaitForGameViewport(TFunction<void()> Callback)
{
	FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateLambda([Callback](float) -> bool
		{
			if (GEngine && GEngine->GameViewport)
			{
				Callback();
				return false;
			}
			return true;
		}));
}
