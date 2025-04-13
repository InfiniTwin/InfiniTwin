// Fill out your copyright notice in the Description page of Project Settings.

#include "IT.h"
#include "ECSSubsystem.h"
#include "flecs.h"
#include "UI/ITUIModule.h"

void UIT::Initialize(FSubsystemCollectionBase& Collection) {
	UECSSubsystem* ECS = GetWorld()->GetGameInstance()->GetSubsystem<UECSSubsystem>();
	ECS->World->import<UIElements::ITUIModule>();

	Super::Initialize(Collection);
}

void UIT::Deinitialize() {
	Super::Deinitialize();
}