/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormodelparamsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormodelparamsitem.h"
#include <IPXactWrapper/ComponentEditor/modelParameters/modelparametereditor.h>

ComponentEditorModelParamsItem::ComponentEditorModelParamsItem( ComponentEditorTreeModel* model,
															   LibraryInterface* libHandler,
															   QSharedPointer<Component> component,
															   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
modelParams_(component->getModelParameters()) {
	
}

ComponentEditorModelParamsItem::~ComponentEditorModelParamsItem() {
}

QFont ComponentEditorModelParamsItem::getFont() const {
    QFont font(ComponentEditorItem::getFont());
    font.setBold(true);
    if ( modelParams_.empty())
    {
        font.setBold(false);
    }
    return font;
}

QString ComponentEditorModelParamsItem::text() const {
	return tr("Model parameters");
}

bool ComponentEditorModelParamsItem::isValid() const {

	// if one model parameter is invalid
	foreach (QSharedPointer<ModelParameter> modelParam, modelParams_) {
		if (!modelParam->isValid()) {
			return false;
		}
	}

	return true;
}

ItemEditor* ComponentEditorModelParamsItem::editor() {
	if (!editor_) {
		editor_ = new ModelParameterEditor(component_, libHandler_);
		editor_->setDisabled(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

QString ComponentEditorModelParamsItem::getTooltip() const {
	return tr("Contains the implementation parameters");
}
