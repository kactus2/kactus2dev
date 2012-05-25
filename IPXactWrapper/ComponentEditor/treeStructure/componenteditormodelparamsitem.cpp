/* 
 *  	Created on: 9.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditormodelparamsitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditormodelparamsitem.h"

ComponentEditorModelParamsItem::ComponentEditorModelParamsItem( ComponentEditorTreeModel* model,
															   LibraryInterface* libHandler,
															   QSharedPointer<Component> component,
															   ComponentEditorItem* parent ):
ComponentEditorItem(model, libHandler, component, parent),
modelParams_(component->getModelParameters()), 
editor_(component, &modelParams_) {
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
}

ComponentEditorModelParamsItem::~ComponentEditorModelParamsItem() {
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

	// if model parameters were valid then the editor decides the validity
	return editor_.isValid();
}

ItemEditor* ComponentEditorModelParamsItem::editor() {
	return &editor_;
}

QString ComponentEditorModelParamsItem::getTooltip() const {
	return tr("Contains the implementation parameters");
}
