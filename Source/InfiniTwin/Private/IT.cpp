// Fill out your copyright notice in the Description page of Project Settings.


#include "IT.h"
#include "flecs.h"
#include "ECS.h"
#include "UI.h"

void UIT::Initialize(FSubsystemCollectionBase& Collection)
{
	UECS* ECS = GetWorld()->GetGameInstance()->GetSubsystem<UECS>();

	ECS->World->import<UI::Module>();

	Super::Initialize(Collection);
}

void UIT::Deinitialize()
{
	Super::Deinitialize();
}