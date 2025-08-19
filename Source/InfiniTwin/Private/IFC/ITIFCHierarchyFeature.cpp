// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFCHierarchyFeature.h"
#include "IFC.h"
#include "ActionFeature.h"
#include "WidgetFeature.h"
#include "TypographyFeature.h"
#include "ButtonFeature.h"

namespace IFC {
	using namespace ECS;

	void AddItem(flecs::world& world, const FString& path, const flecs::entity item) {
		RunScript(world, "UI/IFC", "ItemHierarchy", Tokens({
			TOKEN(TOKEN_PATH, NormalizedPath(path)),
			TOKEN(TOKEN_TARGET, IdString(item.id())),
			TOKEN(TOKEN_TEXT,  item.try_get<Name>()->Value) }));
	}

	void AddHierarchy(flecs::world& world, const FString& parentPath, flecs::entity item, const FString& container = ITEM_CONTAINER) {
		auto path = parentPath + container + TEXT("::") + FString(item.name());
		AddItem(world, path, item);
		item.children([&](flecs::entity child) {
			AddHierarchy(world, path, child);
			});
	}

	void ITIFCHierarchyFeature::RegisterComponents(flecs::world& world) {
		world.component<Selected>();
	}

	void ITIFCHierarchyFeature::CreateQueries(flecs::world& world) {
		world.component<QueryHierarchies>();
		world.set(QueryHierarchies{
			world.query_builder<Hierarchy>(COMPONENT(QueryHierarchies))
			.without<Collection>()
			.cached().build() });

		world.component<QueryHierarchyCollections>();
		world.set(QueryHierarchyCollections{
			world.query_builder<Collection, Hierarchy>(COMPONENT(QueryHierarchyCollections))
			.cached().build() });

		world.component<QuerySelectedIFCData>();
		world.set(QuerySelectedIFCData{
			world.query_builder<Selected, IFCData>(COMPONENT(QuerySelectedIFCData))
			.cached().build() });
	};

	void ITIFCHierarchyFeature::CreateObservers(flecs::world& world) {
		world.observer<const Name>("AddHierarchyUIItemOnCreate")
			.with<IFCData>()
			.event(flecs::OnSet)
			.each([&](flecs::entity item, const Name& name) {
			if (name.Value.IsEmpty()) return;

			world.try_get<QueryHierarchies>()->Value.each([&](flecs::entity root, Hierarchy) {
				if (!IsDescendant(item, root)) return;

				world.try_get<QueryHierarchyCollections>()->Value.each([&](flecs::entity collection, Collection, Hierarchy) {
					const FString rootPath = UTF8_TO_TCHAR(root.path().c_str());
					const FString collectionPath = UTF8_TO_TCHAR(collection.path().c_str());

					FString itemPath = UTF8_TO_TCHAR(item.path().c_str());
					itemPath.RemoveFromStart(rootPath);
					itemPath.RemoveFromStart(TEXT("::"));
					if (!item.parent().has<Hierarchy>())
						itemPath.ReplaceInline(TEXT("::"), *(UTF8_TO_TCHAR(ITEM_CONTAINER) + FString(TEXT("::"))));
					const FString path = NormalizedPath(collectionPath + TEXT(".") + itemPath);

					AddItem(world, path, item);
					});
				});
				});

		world.observer<>("AddHierarchyUIItemOnCollectionCreate")
			.with<Hierarchy>()
			.with<Collection>()
			.event(flecs::OnAdd)
			.each([&](flecs::entity collection) {
			world.try_get<QueryHierarchies>()->Value.each([&](flecs::entity root, Hierarchy) {
				root.children([&](flecs::entity child) {
					AddHierarchy(world, FString(collection.path()), child, "");
					}); });
				});

		world.observer<>("DestroyItemUIs")
			.with<UIOf>(flecs::Wildcard)
			.event(flecs::OnRemove)
			.each([](flecs::entity itemUI) {
			itemUI.destruct();
				});

		world.observer<>("SwitchItemFromUI")
			.with<CheckBoxState>(flecs::Wildcard)
			.event(flecs::OnSet)
			.each([](flecs::entity checkBox) {
			for (flecs::entity itemUI = checkBox; itemUI.is_valid(); itemUI = itemUI.parent())
				if (itemUI.has<UIOf>(flecs::Wildcard)) {
					if (checkBox.has(Checked)) itemUI.target<UIOf>().add<Selected>();
					else itemUI.target<UIOf>().remove<Selected>();
					return;
				}
				});

		world.observer<>("DeselectOtherItems")
			.with<IFCData>()
			.with<Selected>()
			.event(flecs::OnSet)
			.each([&](flecs::entity item) {
			world.try_get<QuerySelectedIFCData>()->Value.each([&item](flecs::entity otherItem, Selected, IFCData) {
				if (item.id() != otherItem.id())
					otherItem.remove<Selected>();
				});
				});

		world.observer<>("UncheckCheckBoxFromItem")
			.with<Selected>()
			.event(flecs::OnRemove).event(flecs::OnRemove)
			.each([&](flecs::entity item) {
			world.try_get<QueryUIOf>()->Value.iter().set_var("source", item).each([](flecs::entity itemUI) {
				for (flecs::entity checkbox : FindDescendants<Selected, CheckBox>(itemUI, 3))
					checkbox.add(Unchecked);
				});
				});
	}
}
