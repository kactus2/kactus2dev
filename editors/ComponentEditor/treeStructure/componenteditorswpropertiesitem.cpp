/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorswpropertiesitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorswpropertiesitem.h"
#include <editors/ComponentEditor/software/SWPropertiesEditor.h>

ComponentEditorSWPropertiesItem::ComponentEditorSWPropertiesItem(
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
swProperties_(component->getSWProperties()) {

}

ComponentEditorSWPropertiesItem::~ComponentEditorSWPropertiesItem() {
}

QFont ComponentEditorSWPropertiesItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    font.setBold(!swProperties_.isEmpty());
    return font;
}

QString ComponentEditorSWPropertiesItem::getTooltip() const {
	return tr("Contains the software properties of the component");
}

QString ComponentEditorSWPropertiesItem::text() const {
	return tr("Software properties");
}

bool ComponentEditorSWPropertiesItem::isValid() const {
	foreach (QSharedPointer<ComProperty> property, swProperties_) {
		if (!property->isValid()) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorSWPropertiesItem::editor() {
	if (!editor_) {
		editor_ = new SWPropertiesEditor(component_, libHandler_);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}
