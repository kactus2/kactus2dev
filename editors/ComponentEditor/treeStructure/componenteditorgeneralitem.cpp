/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorgeneralitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorgeneralitem.h"
#include <editors/ComponentEditor/general/generaleditor.h>

ComponentEditorGeneralItem::ComponentEditorGeneralItem( ComponentEditorTreeModel* model,
													   LibraryInterface* libHandler,
													   QSharedPointer<Component> component,
													   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent) {

}

ComponentEditorGeneralItem::~ComponentEditorGeneralItem() {
}

QString ComponentEditorGeneralItem::text() const {
	return tr("General");
}

bool ComponentEditorGeneralItem::isValid() const {
	// general editor is always valid because it contains no mandatory elements
	return true;
}

ItemEditor* ComponentEditorGeneralItem::editor() {
	if (!editor_) {
		editor_ = new GeneralEditor(libHandler_, component_);
		editor_->setDisabled(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

QString ComponentEditorGeneralItem::getTooltip() const {
	return tr("Contains the general settings of a component");
}
