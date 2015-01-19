//-----------------------------------------------------------------------------
// File: componenteditorrootitem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 9.5.2012
//
// Description:
// The root item in the component editor's navigation tree.
//-----------------------------------------------------------------------------

#include "componenteditorrootitem.h"

#include "componenteditorbusinterfacesitem.h"
#include "componenteditortreemodel.h"

#include <IPXACTmodels/component.h>

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::ComponentEditorRootItem()
//-----------------------------------------------------------------------------
ComponentEditorRootItem::ComponentEditorRootItem(LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorTreeModel* parent):
ComponentEditorItem(libHandler, component, parent)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::~ComponentEditorRootItem()
//-----------------------------------------------------------------------------
ComponentEditorRootItem::~ComponentEditorRootItem()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::text()
//-----------------------------------------------------------------------------
QString ComponentEditorRootItem::text() const
{
	return tr("root");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::isValid()
//-----------------------------------------------------------------------------
bool ComponentEditorRootItem::isValid() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::editor()
//-----------------------------------------------------------------------------
ItemEditor* ComponentEditorRootItem::editor()
{
	return NULL;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::editor()
//-----------------------------------------------------------------------------
const ItemEditor* ComponentEditorRootItem::editor() const
{
	return NULL;
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::getTooltip()
//-----------------------------------------------------------------------------
QString ComponentEditorRootItem::getTooltip() const
{
	return tr("The root item of the navigation tree");
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::addChildItem()
//-----------------------------------------------------------------------------
void ComponentEditorRootItem::addChildItem(QSharedPointer<ComponentEditorItem> childItem)
{
    childItems_.append(childItem);
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::getBusInterfaceItem()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentEditorItem> ComponentEditorRootItem::getBusInterfaceItem(QString const& interfaceName) const
{
	// check all children
	foreach (const QSharedPointer<ComponentEditorItem> item, childItems_)
    {
		const QSharedPointer<ComponentEditorBusInterfacesItem> busIfsItem =
			item.dynamicCast<ComponentEditorBusInterfacesItem>();

		// if the child was for bus interfaces summary item
		if (busIfsItem)
        {
			return busIfsItem->getBusInterfaceItem(interfaceName);
		}
	}

	// if there was no item for bus interfaces summary
	return QSharedPointer<ComponentEditorItem>();
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorRootItem::onInterfaceAdded()
//-----------------------------------------------------------------------------
void ComponentEditorRootItem::onInterfaceAdded()
{
    foreach (const QSharedPointer<ComponentEditorItem> item, childItems_)
    {
        const QSharedPointer<ComponentEditorBusInterfacesItem> busIfsItem =
            item.dynamicCast<ComponentEditorBusInterfacesItem>();

        // if the child was for bus interfaces summary item.
        if (busIfsItem)
        {            
            int childIndex = busIfsItem->children().count();
            model_->addItem(busIfsItem.data(), childIndex);
            busIfsItem->refreshEditor();
            busIfsItem->child(childIndex)->setHighlight(true);
            return;
        }
    }
}
