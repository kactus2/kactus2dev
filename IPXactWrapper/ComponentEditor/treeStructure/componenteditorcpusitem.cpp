/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorcpusitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorcpusitem.h"
#include <IPXactWrapper/ComponentEditor/cpus/cpuseditor.h>

ComponentEditorCpusItem::ComponentEditorCpusItem(ComponentEditorTreeModel* model, 
												 LibraryInterface* libHandler,
												 QSharedPointer<Component> component,
												 ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
cpus_(component->getCpus()) {
	
}

ComponentEditorCpusItem::~ComponentEditorCpusItem() {
}

QFont ComponentEditorCpusItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    if ( cpus_.isEmpty() )
    {
        font.setBold(false);
    }
    return font;
}

QString ComponentEditorCpusItem::text() const {
	return tr("Cpus");
}

ItemEditor* ComponentEditorCpusItem::editor() {
	if (!editor_) {
		editor_ = new CpusEditor(component_, libHandler_);
		editor_->setDisabled(locked_);
		connect(editor_, SIGNAL(contentChanged()), 
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

QString ComponentEditorCpusItem::getTooltip() const {
	return tr("Contains the programmable cores of the component");
}

bool ComponentEditorCpusItem::isValid() const {
	QStringList addrSpaceNames = component_->getAddressSpaceNames();

	foreach (QSharedPointer<Cpu> cpu, cpus_) {
		if (!cpu->isValid(addrSpaceNames)) {
			return false;
		}
	}

	// all cpus were valid
	return true;
}
