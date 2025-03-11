// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ITHUD.generated.h"

/**
 * 
 */
UCLASS()
class INFINITWIN_API AITHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	virtual void BeginPlay() override;
};
