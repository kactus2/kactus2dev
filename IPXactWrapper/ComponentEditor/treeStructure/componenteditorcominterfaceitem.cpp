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
	connect(&editor_, SIGNAL(helpUrlRequested(QString const&)),
		this, SIGNAL(helpUrlRequested(QString const&)));
}

ComponentEditorComInterfaceItem::~ComponentEditorComInterfaceItem() {
}

QString ComponentEditorComInterfaceItem::text() const {
	return interface_->getName();
}

bool ComponentEditorComInterfaceItem::isValid() const {
	// check that the COM interface is valid
	if (!interface_->isValid()) {
		return false;
	}

	// check that if there is a COM definition it is found
	else if (interface_->getComType().isValid()) {
		if (!libHandler_->contains(interface_->getComType())) {
			return false;
		}
	}

	return true;
}

ItemEditor* ComponentEditorComInterfaceItem::editor() {
	return &editor_;
}

const ItemEditor* ComponentEditorComInterfaceItem::editor() const {
	return &editor_;
}

QFont ComponentEditorComInterfaceItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorComInterfaceItem::getTooltip() const {
	return tr("Specifies a virtual communication interface for the component");
}

bool ComponentEditorComInterfaceItem::canBeOpened() const {
	if (interface_->getComType().isValid()) {
		return libHandler_->contains(interface_->getComType());
	}
	else {
		return false;
	}
}

void ComponentEditorComInterfaceItem::openItem( bool builtinEditor /*= false*/ ) {
	VLNV comdefVLNV = interface_->getComType();
	emit openComDefinition(comdefVLNV);
}
