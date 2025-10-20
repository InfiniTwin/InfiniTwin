// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFCAttributeFeature.h"
#include "IFC/ITIFCHierarchyFeature.h"
#include "IFC.h"
#include "AttributeFeature.h"
#include "WidgetFeature.h"

namespace IFC {
	using namespace ECS;

	void AddItem(flecs::world& world, const FString& collectionPath, const FString& ifcObjectId, const flecs::entity item, bool isNestedValue = false) {
		FString value = item.has<Value>() ? ECS::CleanCode(item.try_get<Value>()->Value) : "";

		RunScript(world, "UI/IFC", "ItemAttribute", Tokens({
			TOKEN(TOKEN_NESTED_VALUE, isNestedValue ? TEXT("true") : TEXT("false")),
			TOKEN(TOKEN_PATH, NormalizedPath(collectionPath)),
			TOKEN(TOKEN_TARGET, ifcObjectId),
			TOKEN(TOKEN_NAME, CleanName(item.try_get<Name>()->Value)),
			TOKEN(TOKEN_VALUE, *value) }));

		item.children([&](flecs::entity value) {
			AddItem(world, collectionPath, ifcObjectId, value, true);
			});
	}

	void ITIFCAttributeFeature::CreateQueries(flecs::world& world) {
		world.component<QueryAttributes>();
		world.set(QueryAttributes{
			world.query_builder<>(COMPONENT(QueryAttributes))
			.with<Attribute>()
			.without<Collection>()
			.cached().build() });

		world.component<QueryAttributeCollections>();
		world.set(QueryAttributeCollections{
			world.query_builder<>(COMPONENT(QueryAttributeCollections))
			.with<Attribute>()
			.with<Collection>()
			.cached().build() });
	};

	void ITIFCAttributeFeature::CreateObservers(flecs::world& world) {
		world.observer<>("AddAttributeUIItemsOnIfcObjectSelected")
			.with<IfcObject>().filter()
			.with<Selected>()
			.event(flecs::OnAdd)
			.each([&](flecs::entity ifcObject) {
			TArray<flecs::entity> attributes = GetAttributes(world, ifcObject);
			world.try_get<QueryAttributeCollections>()->Value.each([&](flecs::entity collection) {
				for (auto& attribute : attributes)
					AddItem(world, UTF8_TO_TCHAR(collection.path().c_str()), IdString(ifcObject.id()), attribute);
				});
				});

		world.observer<>("AddAttributeUIItemsOnCollectionCreate")
			.with<Attribute>()
			.with<Collection>()
			.event(flecs::OnAdd)
			.each([&](flecs::entity collection) {
			world.try_get<QuerySelectedIfcObjects>()->Value.each([&](flecs::entity ifcObject) {
				TArray<flecs::entity> attributes = GetAttributes(world, ifcObject);
				for (auto& attribute : attributes)
					AddItem(world, UTF8_TO_TCHAR(collection.path().c_str()), IdString(ifcObject.id()), attribute);
				});
				});

		world.observer<>("RemoveAttributesOfDeselectedObject")
			.with<IfcObject>().filter()
			.with<Selected>()
			.event(flecs::OnRemove)
			.each([&](flecs::entity ifcObject) {
			world.try_get<QueryUIOf>()->Value.iter().set_var("source", ifcObject).each([](flecs::entity itemUI) {
				if (itemUI.parent().parent().has<Attribute>())
					itemUI.destruct();
				});
				});
	}
}
