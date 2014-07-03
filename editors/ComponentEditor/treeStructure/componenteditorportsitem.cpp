/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorportsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorportsitem.h"
#include <editors/ComponentEditor/ports/portseditor.h>


ComponentEditorPortsItem::ComponentEditorPortsItem(ComponentEditorTreeModel* model,
												   LibraryInterface* libHandler,
												   QSharedPointer<Component> component,
												   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
ports_(component->getPorts()) {

}

ComponentEditorPortsItem::~ComponentEditorPortsItem() {
}

QFont ComponentEditorPortsItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    if ( ports_.empty())
    {
        font.setBold(false);
    }
    return font;
}

QString ComponentEditorPortsItem::text() const {
	return tr("Ports");
}

bool ComponentEditorPortsItem::isValid() const {
	bool hasViews = component_->hasViews();
	foreach (QSharedPointer<Port> port, ports_) {
		if (!port->isValid(hasViews)) {
			return false;
		}
	}
	return true;
}

ItemEditor* ComponentEditorPortsItem::editor() {
	if (!editor_) {
		editor_ = new PortsEditor(component_, libHandler_, false);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)), Qt::UniqueConnection);
        connect(editor_, SIGNAL(createInterface()), 
            this, SIGNAL(createInterface()), Qt::UniqueConnection);
	}
	return editor_;
}

QString ComponentEditorPortsItem::getTooltip() const {
	return tr("Contains the external ports of the component");
}
