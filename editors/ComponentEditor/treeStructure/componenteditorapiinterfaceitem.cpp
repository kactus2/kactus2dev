/* 
 *  	Created on: 24.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditorapiinterfaceitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditorapiinterfaceitem.h"
#include <editors/ComponentEditor/software/apiInterface/ApiInterfaceEditor.h>

#include <QApplication>

ComponentEditorAPIInterfaceItem::ComponentEditorAPIInterfaceItem(
	QSharedPointer<ApiInterface> APIInterface,
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
apiIf_(APIInterface) {

	Q_ASSERT(apiIf_);
}

ComponentEditorAPIInterfaceItem::~ComponentEditorAPIInterfaceItem() {
}

QString ComponentEditorAPIInterfaceItem::getTooltip() const {
	return tr("Contains the details of an API interface in the component");
}

QString ComponentEditorAPIInterfaceItem::text() const {
	return apiIf_->getName();
}

bool ComponentEditorAPIInterfaceItem::isValid() const {
	// check that the API interface is valid
	if (!apiIf_->isValid()) {
		return false;
	}

	// check that if API definition is specified it must exist
	if (apiIf_->getApiType().isValid()) {
		if (!libHandler_->contains(apiIf_->getApiType())) {
			return false;
		}
	}

	return true;
}

ItemEditor* ComponentEditorAPIInterfaceItem::editor() {
	if (!editor_) {
		editor_ = new ApiInterfaceEditor(libHandler_, component_, apiIf_, NULL);
		editor_->setProtection(locked_);
		connect(editor_, SIGNAL(contentChanged()),
			this, SLOT(onEditorChanged()), Qt::UniqueConnection);
		connect(editor_, SIGNAL(helpUrlRequested(QString const&)),
			this, SIGNAL(helpUrlRequested(QString const&)));
	}
	return editor_;
}

QFont ComponentEditorAPIInterfaceItem::getFont() const {
	return QApplication::font();
}
