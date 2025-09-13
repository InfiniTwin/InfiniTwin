// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ITUI.h"
#include "UIElements.h"
#include "ECS.h"
#include "ECSCore.h"
#include "GameFramework/GameUserSettings.h"

#if PLATFORM_WINDOWS
#include <Windows.h>
#endif

namespace UI{
	ITUI::ITUI(flecs::world& world) {
		world.module<ITUI>();

		UI::Register(world);

		Initialize(world);
	}

	void SetupViewport() {
#if !WITH_EDITOR
		if (UGameUserSettings* settings = GEngine->GameUserSettings) {
#if PLATFORM_WINDOWS
			const int32 captionHeight = ::GetSystemMetrics(SM_CYCAPTION);
			const int32 frameThickness = ::GetSystemMetrics(SM_CYSIZEFRAME);
			const int32 titleBarHeight = captionHeight + frameThickness;
#else
			const int32 titleBarHeight = 50;
#endif

			FIntPoint desktopRes = settings->GetDesktopResolution();
			const int32 desiredWidth = desktopRes.X;
			const int32 desiredHeight = desktopRes.Y - titleBarHeight;

			settings->SetFullscreenMode(EWindowMode::Windowed);
			settings->SetScreenResolution(FIntPoint(desiredWidth, desiredHeight));
			settings->ApplySettings(false);

			TSharedPtr<SWindow> mainWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
			if (mainWindow.IsValid())
				mainWindow->MoveWindowTo(FVector2D(0, titleBarHeight));
		}
#endif
	}

	void ITUI::Initialize(flecs::world& world) {
		SetupViewport();

		using namespace ECS;

		Scopes.Add(TEXT("[UI]"), UI::Scope() = "UI.ITUI");

		RunScripts(world, "UI/Core/", { "Colors", "Texts", "Layout", "Widgets", "Styles", "Actions", "Elements", "settings" });

		RunScripts(world, "UI/IT/", { "ViewportMain", "Layout" });
	}
}