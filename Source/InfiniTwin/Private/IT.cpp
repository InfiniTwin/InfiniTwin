// Fill out your copyright notice in the Description page of Project Settings.


#include "IT.h"
#include "flecs.h"
#include "ECS.h"
#include "UI.h"

void UIT::Initialize(FSubsystemCollectionBase& Collection)
{
	flecs::world* world = GetWorld()->GetGameInstance()->GetSubsystem<UECS>()->World();

	world->import<UI::Module>();

	Super::Initialize(Collection);
}

void UIT::Deinitialize()
{
	Super::Deinitialize();
}