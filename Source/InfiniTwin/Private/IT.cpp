// Fill out your copyright notice in the Description page of Project Settings.

#include "IT.h"
#include "ECSSubsystem.h"
#include "flecs.h"
#include "UI/ITUI.h"
#include "Tickable.h"

void UIT::Initialize(FSubsystemCollectionBase& Collection) {
	WaitForGameViewport([this] {
		UECSSubsystem* ecs = GetWorld()->GetGameInstance()->GetSubsystem<UECSSubsystem>();
		ecs->World->import<UI::ITUI>();
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
