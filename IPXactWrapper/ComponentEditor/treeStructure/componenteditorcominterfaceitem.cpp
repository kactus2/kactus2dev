/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcominterfaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcominterfaceitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorComInterfaceItem::ComponentEditorComInterfaceItem(QSharedPointer<ComInterface> comInterface,
																 ComponentEditorTreeModel* model,
																 LibraryInterface* libHandler,
																 QSharedPointer<Component> component,
																 ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
interface_(comInterface),
editor_(libHandler, component, comInterface) {
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorComInterfaceItem::~ComponentEditorComInterfaceItem() {
}

QString ComponentEditorComInterfaceItem::text() const {
	return interface_->getName();
}

bool ComponentEditorComInterfaceItem::isValid() const {
	if (!interface_->isValid()) {
		return false;
	}
	return editor_.isValid();
}

ItemEditor* ComponentEditorComInterfaceItem::editor() {
	return &editor_;
}

QFont ComponentEditorComInterfaceItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorComInterfaceItem::getTooltip() const {
	return tr("Specifies a virtual communication interface for the component");
}
