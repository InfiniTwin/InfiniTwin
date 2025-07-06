// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFC.h"
#include <IFC.h>
#include "IFC/ITIFCFeature.h"

namespace IFC{
	ITIFC::ITIFC(flecs::world& world) {
		world.module<ITIFC>();

		IFC::Register(world);

		ITIFCFeature::Initialize(world);
	
		LoadIFCFile(world, "A:/InfiniTwinOrg/IFC5-development/examples/Hello Wall/hello-wall.ifcx");
	}
}