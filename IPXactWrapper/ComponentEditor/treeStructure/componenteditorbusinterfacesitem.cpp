/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorbusinterfacesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorbusinterfacesitem.h"
#include "componenteditorbusinterfaceitem.h"
#include <IPXactWrapper/ComponentEditor/busInterfaces/businterfaceseditor.h>

ComponentEditorBusInterfacesItem::ComponentEditorBusInterfacesItem(ComponentEditorTreeModel* model,
																   LibraryInterface* libHandler,
																   QSharedPointer<Component> component,
																   ComponentEditorItem* parent,
                                                                   QWidget* parentWnd):
ComponentEditorItem(model, libHandler, component, parent),
busifs_(component->getBusInterfaces()),
parentWnd_(parentWnd) {

	foreach (QSharedPointer<BusInterface> busif, busifs_) {
		QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(
			new ComponentEditorBusInterfaceItem(busif, model, libHandler, component, this, parentWnd));

		childItems_.append(busifItem);
	}
}

ComponentEditorBusInterfacesItem::~ComponentEditorBusInterfacesItem() {
}

QFont ComponentEditorBusInterfacesItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    if ( !component_->hasInterfaces() )
    {
        font.setBold(false);
    }
    return font;
}

QString ComponentEditorBusInterfacesItem::text() const {
	return tr("Bus interfaces");
}

ItemEditor* ComponentEditorBusInterfacesItem::editor() {
	if (!editor_) {
		editor_ = new BusInterfacesEditor(libHandler_, component_);
		editor_->setDisabled(locked_);
		connect(editor_, SIGNAL(contentChanged()), 
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childAdded(int)),
			this, SLOT(onAddChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(childRemoved(int)),
			this, SLOT(onRemoveChild(int)), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

QString ComponentEditorBusInterfacesItem::getTooltip() const {
	return tr("Contains the bus interfaces specified for a component");
}

void ComponentEditorBusInterfacesItem::createChild( int index ) {
	QSharedPointer<ComponentEditorBusInterfaceItem> busifItem(
		new ComponentEditorBusInterfaceItem(busifs_.at(index),
		model_, libHandler_, component_, this, parentWnd_));
	busifItem->setLocked(locked_);

	childItems_.insert(index, busifItem);
}

QSharedPointer<ComponentEditorItem> ComponentEditorBusInterfacesItem::getBusInterfaceItem( const QString& interfaceName ) const {
	foreach (const QSharedPointer<ComponentEditorItem> child, childItems_) {
		
		// if the bus interface name matches the searched interface name
		if (child->text() == interfaceName) {
			return child;
		}
	}

	// if child was not found
	return QSharedPointer<ComponentEditorItem>();
}
