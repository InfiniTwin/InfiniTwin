// Fill out your copyright notice in the Description page of Project Settings.


#include "IFC/ITIFCHierarchyFeature.h"
#include "IFC/ITIFC.h"
#include "WidgetFeature.h"
#include "ButtonFeature.h"

namespace IFC {
	using namespace ECS;

	void AddItem(flecs::world& world, const FString& path, const flecs::entity item) {
		RunScript(world, "UI/IFC", "ItemHierarchy", Tokens({
			TOKEN(TOKEN_CAN_TOGGLE_CHILDREN, item.has<Branch>() ? TEXT("true") : TEXT("false")),
			TOKEN(TOKEN_PATH, NormalizedPath(path)),
			TOKEN(TOKEN_TARGET, IdString(item.id())),
			TOKEN(TOKEN_NAME, item.try_get<Name>()->Value) }));
	}

	void AddHierarchy(flecs::world& world, const FString& parentPath, flecs::entity item, const FString& container = "") {
		FString path = parentPath + container + TEXT("::") + FString(item.name());
		AddItem(world, path, item);
		item.children([&](flecs::entity child) {
			if (child.has<IfcObject>())
				AddHierarchy(world, path, child, ITEM_CONTAINER);
		});
	}

	void ITIFCHierarchyFeature::RegisterComponents(flecs::world& world) {
		world.component<Selected>();
	}

	void ITIFCHierarchyFeature::CreateQueries(flecs::world& world) {
		world.component<QueryRoots>();
		world.set(QueryRoots{
			world.query_builder<>(COMPONENT(QueryRoots))
			.with<Root>()
			.without<Collection>()
			.cached().build() });

		world.component<QueryRootCollections>();
		world.set(QueryRootCollections{
			world.query_builder<>(COMPONENT(QueryRootCollections))
			.with<Root>()
			.with<Collection>()
			.cached().build() });

		world.component<QuerySelectedIfcObjects>();
		world.set(QuerySelectedIfcObjects{
			world.query_builder<>(COMPONENT(QuerySelectedIfcObjects))
			.with<Selected>()
			.with<IfcObject>()
			.cached().build() });
	};

	void ITIFCHierarchyFeature::CreateObservers(flecs::world& world) {
		world.observer<const Name>("AddHierarchyUIItemOnCreate")
			.with<IfcObject>()
			.event(flecs::OnSet)
			.each([&](flecs::entity item, const Name& name) {
			if (name.Value.IsEmpty())
				return;

			world.try_get<QueryRoots>()->Value.each([&](flecs::entity root) {
				if (!IsDescendant(item, root))
					return;

				world.try_get<QueryRootCollections>()->Value.each([&](flecs::entity collection) {
					const FString rootPath = UTF8_TO_TCHAR(root.path().c_str());
					const FString collectionPath = UTF8_TO_TCHAR(collection.path().c_str());

					FString itemPath = UTF8_TO_TCHAR(item.path().c_str());
					itemPath.RemoveFromStart(rootPath);
					itemPath.RemoveFromStart(TEXT("::"));
					if (!item.parent().has<Root>())
						itemPath.ReplaceInline(TEXT("::"), *(UTF8_TO_TCHAR(ITEM_CONTAINER) + FString(TEXT("::"))));
					const FString path = NormalizedPath(collectionPath + TEXT(".") + itemPath);

					AddItem(world, path, item);
				});
			});
		});

		world.observer<>("AddHierarchyUIItemOnCollectionCreate")
			.with<Root>()
			.with<Collection>()
			.event(flecs::OnAdd)
			.each([&](flecs::entity collection) {
			world.try_get<QueryRoots>()->Value.each([&](flecs::entity root) {
				root.children([&](flecs::entity child) {
					AddHierarchy(world, FString(collection.path()), child);
				}); });
		});

		world.observer<>("DestroyItemUIs")
			.with<UIOf>(flecs::Wildcard)
			.event(flecs::OnRemove)
			.each([](flecs::entity itemUI) {
			itemUI.destruct();
		});

		world.observer<>("SwitchItemFromUI")
			.with<Selected>().filter()
			.with<CheckBoxState>(flecs::Wildcard)
			.event(flecs::OnSet)
			.each([](flecs::entity checkBox) {
			for (flecs::entity itemUI : FindAncestors<Border>(checkBox, 3)) {
				if (checkBox.has(Checked)) itemUI.target<UIOf>().add<Selected>();
				else itemUI.target<UIOf>().remove<Selected>();
			}
		});

		world.observer<>("DeselectOtherItems")
			.with<IfcObject>()
			.with<Selected>()
			.event(flecs::OnAdd)
			.each([&](flecs::entity item) {
			world.try_get<QuerySelectedIfcObjects>()->Value.each([&item](flecs::entity otherItem) {
				if (item.id() != otherItem.id())
					otherItem.remove<Selected>();
			});
		});

		world.observer<>("UncheckCheckBoxFromItem")
			.with<IfcObject>().filter()
			.with<Selected>()
			.event(flecs::OnRemove)
			.each([&](flecs::entity item) {
			world.try_get<QueryUIOf>()->Value.iter().set_var("source", item).each([](flecs::entity itemUI) {
				for (flecs::entity checkbox : FindDescendants<Selected, CheckBox>(itemUI, 3))
					checkbox.add(Unchecked);
			});
		});

		world.observer<>("CheckCheckBoxOnItemUICreate")
			.with<Selected>()
			.with<CheckBox>()
			.event(flecs::OnAdd)
			.each([](flecs::entity checkBox) {
			for (flecs::entity itemUI : FindAncestors<Border>(checkBox, 3))
				if (itemUI.has<UIOf>(flecs::Wildcard) && itemUI.target<UIOf>().has<Selected>())
					checkBox.add(Checked);
		});
	}
}
