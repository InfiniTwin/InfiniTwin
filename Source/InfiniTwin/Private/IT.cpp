// Fill out your copyright notice in the Description page of Project Settings.


#include "IT.h"
#include "ECS.h"
#include "flecs.h"
#include "UICore/Public/UICoreModule.h"
#include "UI/UIITModule.h"

void UIT::Initialize(FSubsystemCollectionBase& Collection)
{
	UECS* ECS = GetWorld()->GetGameInstance()->GetSubsystem<UECS>();

	ECS->World->import<UI::UICoreModule>();
	ECS->World->import<UI::UIITModule>();

	Super::Initialize(Collection);
}

void UIT::Deinitialize()
{
	Super::Deinitialize();
}