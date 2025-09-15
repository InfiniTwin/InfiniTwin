// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFCObjectFeature.h"
#include "IFC/ITIFCHierarchyFeature.h"
#include "ModelFeature.h"
#include "ISMSubsystem.h"

namespace IFC {
	void ITIFCObjectFeature::CreateObservers(flecs::world& world) {
		world.observer<const ISM>("FocusOnSelectedObject")
			.with<Selected>()
			.event(flecs::OnAdd)
			.each([&](flecs::entity ifcObject, const ISM& ism) {
			FVector center = static_cast<UWorld*>(world.get_ctx())
				->GetSubsystem<UISMSubsystem>()->GetCenter(ism.Value);

			UWorld* uWorld = static_cast<UWorld*>(world.get_ctx());
			APlayerController* pc = uWorld->GetFirstPlayerController();

			// Move camera instantly
			FRotator currentRot = pc->PlayerCameraManager->GetCameraRotation();
			pc->SetViewTarget(pc->GetPawn()); // ensure view target is the pawn
			pc->SetControlRotation(currentRot);
			pc->SetViewTargetWithBlend(pc->GetPawn(), 0.0f);

			// Move pawn/camera location
			APawn* pawn = pc->GetPawn();
			if (pawn) pawn->SetActorLocation(center);
			else pc->SetInitialLocationAndRotation(center, currentRot);
			UE_LOG(LogTemp, Warning, TEXT(">>> Center: X=%f, Y=%f, Z=%f"), center.X, center.Y, center.Z);
		});
	}
}