// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ITUIFeature.h"
#include "ECS.h"
#include "GameFramework/GameUserSettings.h"
#include "UIElements.h"

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace UIElements {
	void ITUIFeature::Initialize(flecs::world& world) {
		SetupViewport();

		ECS::FromJsonAsset(world, ITUI, UIElements::Scope());
		ECS::FromJsonAsset(world, "UISettings", UIElements::Scope());
	}

	void ITUIFeature::SetupViewport() {
#if !WITH_EDITOR
		// Setup main viewport window
		if (UGameUserSettings* Settings = GEngine->GameUserSettings) {
			// 1) Calculate window size minus title bar
#if PLATFORM_WINDOWS
			const int32 CaptionHeight = ::GetSystemMetrics(SM_CYCAPTION);
			const int32 FrameThickness = ::GetSystemMetrics(SM_CYSIZEFRAME);
			const int32 TitleBarHeight = CaptionHeight + FrameThickness;
#else
			const int32 TitleBarHeight = 50;
#endif

			FIntPoint DesktopRes = Settings->GetDesktopResolution();
			const int32 DesiredWidth = DesktopRes.X;
			const int32 DesiredHeight = DesktopRes.Y - TitleBarHeight;

			Settings->SetFullscreenMode(EWindowMode::Windowed);
			Settings->SetScreenResolution(FIntPoint(DesiredWidth, DesiredHeight));
			Settings->ApplySettings(false);

			// 2) After the viewport is set, move the window via Slate
			TSharedPtr<SWindow> MainWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
			if (MainWindow.IsValid())
				MainWindow->MoveWindowTo(FVector2D(0, TitleBarHeight));
		}
#endif
	}
}
