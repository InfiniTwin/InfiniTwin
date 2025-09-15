// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <flecs.h>
#include "IFC.h"
#include "AttributeFeature.h"

namespace IFC {
	struct ITIFCObjectFeature {
		static void CreateObservers(flecs::world& world);
	};
}