// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFCObjectFeature.h"
#include "ECS.h"
#include "IFC/ITIFCHierarchyFeature.h"
#include "ModelFeature.h"
#include "ISMSubsystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "ButtonFeature.h"

namespace IFC {
	constexpr float focusZoom = 0.25f;  // target occupies 25% of screen height
	constexpr float focusPad = 1.05f;  // padding so mesh doesn’t touch screen edge
	constexpr float focusDuration = 0.35f;  // tween duration (seconds)
	constexpr float focusTickRate = 0.01f;  // update interval (seconds)

	void ITIFCObjectFeature::CreateObservers(flecs::world& world) {
		world.observer<const ISM>("FocusOnSelectedObject")
			.with<Selected>()
			.event(flecs::OnAdd)
			.each([&](flecs::entity e, const ISM& ism) {
			UWorld* uworld = static_cast<UWorld*>(world.get_ctx());
			if (!uworld) return;

			UISMSubsystem* ismSubsystem = uworld->GetSubsystem<UISMSubsystem>();
			if (!ismSubsystem) return;

			APlayerController* pc = nullptr;
			for (FConstPlayerControllerIterator it = uworld->GetPlayerControllerIterator(); it; ++it) {
				APlayerController* c = it->Get();
				if (c && c->IsLocalController()) { pc = c; break; }
			}
			if (!pc) return;

			APawn* pawn = pc->GetPawn();
			if (!pawn) return;

			// bounds and framing distance (vertical fit)
			FBoxSphereBounds bounds = ismSubsystem->GetBounds(ism.Value);
			FVector center = bounds.Origin;
			float   halfHeight = bounds.BoxExtent.Z;

			APlayerCameraManager* camMgr = pc->PlayerCameraManager;
			float halfFov = FMath::DegreesToRadians(camMgr->GetFOVAngle() * 0.5f);
			float distance = focusPad * (halfHeight / (focusZoom * FMath::Tan(halfFov)));

			// stable direction from controller yaw only
			FRotator ctrl = pc->GetControlRotation();
			FRotator yawOnly = FRotator(0.f, ctrl.Yaw, 0.f);
			FVector dirXY = FRotationMatrix(yawOnly).GetUnitAxis(EAxis::X).GetSafeNormal();
			if (dirXY.IsNearlyZero()) dirXY = FVector(1, 0, 0);

			// target camera transform (same height as target)
			FVector  targetLocXY = center - dirXY * distance;
			FVector  targetLoc = FVector(targetLocXY.X, targetLocXY.Y, center.Z);
			FRotator targetRot = (center - targetLoc).Rotation();

			// ensure pawn is the view target and follows control rotation
			pc->bAutoManageActiveCameraTarget = false;
			pc->SetViewTarget(pawn);
			pc->ClientSetViewTarget(pawn);

			if (USpringArmComponent* armCfg = pawn->FindComponentByClass<USpringArmComponent>()) {
				armCfg->bUsePawnControlRotation = true;
				armCfg->bInheritPitch = armCfg->bInheritYaw = armCfg->bInheritRoll = true;
			}
			if (UCameraComponent* cam = pawn->FindComponentByClass<UCameraComponent>()) {
				cam->bUsePawnControlRotation = true;
			}

			// one active tween per local PC
			struct FFocusTween { FTimerHandle Handle; bool InProgress = false; };
			static TMap<TWeakObjectPtr<APlayerController>, FFocusTween> sActiveTweens;

			FFocusTween& state = sActiveTweens.FindOrAdd(pc);
			if (state.InProgress) {
				uworld->GetTimerManager().ClearTimer(state.Handle);
				state.InProgress = false;
			}

			// capture start state
			double   startTime = uworld->GetTimeSeconds();
			FVector  startLoc = pawn->GetActorLocation();
			FRotator startRot = pc->GetControlRotation();

			// temporarily disable spring-arm lag and look input
			USpringArmComponent* arm = pawn->FindComponentByClass<USpringArmComponent>();
			bool  hadArm = arm != nullptr;
			bool  oldCamLag = hadArm ? arm->bEnableCameraLag : false;
			bool  oldRotLag = hadArm ? arm->bEnableCameraRotationLag : false;
			float oldCamLagSpeed = hadArm ? arm->CameraLagSpeed : 0.f;
			float oldRotLagSpeed = hadArm ? arm->CameraRotationLagSpeed : 0.f;

			if (hadArm) {
				arm->bEnableCameraLag = false;
				arm->bEnableCameraRotationLag = false;
			}
			pc->SetIgnoreLookInput(true);

			state.InProgress = true;

			// weak refs for safety
			TWeakObjectPtr<APawn>             pawnW = pawn;
			TWeakObjectPtr<APlayerController> pcW = pc;

			FTimerDelegate del;
			del.BindLambda([uworld, &state, pawnW, pcW, startTime,
				startLoc, startRot, targetLoc, targetRot,
				hadArm, arm, oldCamLag, oldRotLag, oldCamLagSpeed, oldRotLagSpeed]() {
					if (!pawnW.IsValid() || !pcW.IsValid()) {
						uworld->GetTimerManager().ClearTimer(state.Handle);
						state.InProgress = false;
						return;
					}

					double tRaw = (uworld->GetTimeSeconds() - startTime) / focusDuration;
					float  t = FMath::Clamp(static_cast<float>(tRaw), 0.f, 1.f);
					float  ease = FMath::InterpEaseInOut(0.f, 1.f, t, 2.f);

					FVector  newLoc = FMath::Lerp(startLoc, targetLoc, ease);
					FRotator newRot = FMath::Lerp(startRot, targetRot, ease).GetNormalized();

					pawnW->SetActorLocation(newLoc, false);
					pcW->SetControlRotation(newRot);

					if (t >= 1.0f) {
						uworld->GetTimerManager().ClearTimer(state.Handle);
						state.InProgress = false;

						if (hadArm && arm) {
							arm->bEnableCameraLag = oldCamLag;
							arm->bEnableCameraRotationLag = oldRotLag;
							arm->CameraLagSpeed = oldCamLagSpeed;
							arm->CameraRotationLagSpeed = oldRotLagSpeed;
						}

						pcW->SetIgnoreLookInput(false);
					}
				});

			uworld->GetTimerManager().SetTimer(state.Handle, del, focusTickRate, true);
				});
	}
}
