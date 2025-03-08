// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldSettings/ITHUD.h"
#include "GameFramework/GameUserSettings.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <Windows.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

void AITHUD::BeginPlay() {
	Super::BeginPlay();

	SetupViewport();
}

void AITHUD::SetupViewport() {
	if (UGameUserSettings* Settings = GEngine->GameUserSettings) {
		int32 TitleBarHeight = 100;
#if PLATFORM_WINDOWS
		int32 CaptionHeight = ::GetSystemMetrics(SM_CYCAPTION);
		int32 FrameThickness = ::GetSystemMetrics(SM_CYSIZEFRAME);
		TitleBarHeight = CaptionHeight + FrameThickness;
#endif

		FIntPoint DesktopRes = Settings->GetDesktopResolution();
		int32 DesiredWidth = DesktopRes.X;
		int32 DesiredHeight = DesktopRes.Y - TitleBarHeight;

		Settings->SetFullscreenMode(EWindowMode::Windowed);
		Settings->SetScreenResolution(FIntPoint(DesiredWidth, DesiredHeight));
		Settings->ApplySettings(false);
	}
}