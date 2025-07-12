// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFC.h"
#include "ECS.h"
#include "IFC.h"
#include "IFC/ITIFCFeature.h"

namespace IFC {
	ITIFC::ITIFC(flecs::world& world) {
		world.module<ITIFC>();

		IFC::Register(world);

		ITIFCFeature::Initialize(world);

		//ECS::RunScript(world, "IFC/HelloWall");

		TArray<FString> ifcFilePaths = {
			TEXT("D:/Projects/Explore/IFC5-development/examples/Hello Wall/hello-wall.ifcx"),
			TEXT("D:/Projects/Explore/IFC5-development/examples/Hello Wall/hello-wall-add-fire-rating-30.ifcx"),
			TEXT("D:/Projects/Explore/IFC5-development/examples/Hello Wall/advanced/3rd-window.ifcx"),
			TEXT("D:/Projects/Explore/IFC5-development/examples/Hello Wall/advanced/add-2nd-storey.ifcx"),
		};
		LoadIFCFiles(world, ifcFilePaths);
	}
}