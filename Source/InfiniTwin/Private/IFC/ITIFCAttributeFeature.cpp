// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFCAttributeFeature.h"
#include "IFC/ITIFCHierarchyFeature.h"
#include "IFC/ITIFC.h"
#include "WidgetFeature.h"
#include "TypographyFeature.h"

namespace IFC {
	using namespace ECS;

	void AddItem(flecs::world& world, const FString& collectionPath, const FString& ifcObjectId, const flecs::entity item, bool isNestedValue = false) {
		FString id = IdString(item.id()).Replace(TEXT("#"), TEXT("ID"));
		FString value = item.has<Value>() ? ECS::CleanCode(item.try_get<Value>()->Value) : "";

		RunScript(world, "UI/IFC", "ItemAttribute", Tokens({
			TOKEN(TOKEN_NESTED_VALUE, isNestedValue ? TEXT("true") : TEXT("false")),
			TOKEN(TOKEN_PATH, NormalizedPath(collectionPath)),
			TOKEN(TOKEN_ID, id),
			TOKEN(TOKEN_TARGET, ifcObjectId),
			TOKEN(TOKEN_NAME, item.try_get<Name>()->Value),
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
			int32_t index = 0;
			while (flecs::entity attributes = ifcObject.target(world.try_get<AttributeRelationship>()->Value, index++)) {
				attributes.children([&](flecs::entity attribute) {
					if (attribute.has<Attribute>())
						world.try_get<QueryAttributeCollections>()->Value.each([&](flecs::entity collection) {
						AddItem(world, UTF8_TO_TCHAR(collection.path().c_str()), IdString(ifcObject.id()), attribute);
							});
					});
			}
				});

		world.observer<>("AddAttributeUIItemsOnCollectionCreate")
			.with<Attribute>()
			.with<Collection>()
			.event(flecs::OnAdd)
			.each([&](flecs::entity collection) {
			world.try_get<QuerySelectedIfcObjects>()->Value.each([&](flecs::entity ifcObject) {
				int32_t index = 0;
				while (flecs::entity attributes = ifcObject.target(world.try_get<AttributeRelationship>()->Value, index++)) {
					attributes.children([&](flecs::entity attribute) {
						if (attribute.has<Attribute>())
							AddItem(world, UTF8_TO_TCHAR(collection.path().c_str()), IdString(ifcObject.id()), attribute);
						});
				}
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
