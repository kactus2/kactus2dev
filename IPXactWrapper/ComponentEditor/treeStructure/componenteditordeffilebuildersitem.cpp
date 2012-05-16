/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditordeffilebuildersitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditordeffilebuildersitem.h"

ComponentEditorDefFileBuildersItem::ComponentEditorDefFileBuildersItem(
	QList<QSharedPointer<FileBuilder> >& fileBuilders,
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	QWidget* widget,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
fileBuilders_(fileBuilders),
editor_(component, &fileBuilders, widget) {

}

ComponentEditorDefFileBuildersItem::~ComponentEditorDefFileBuildersItem() {
}

QString ComponentEditorDefFileBuildersItem::text() const {
	return tr("Default file builders");
}

bool ComponentEditorDefFileBuildersItem::isValid() const {
	return editor_.isValid();
}

ItemEditor* ComponentEditorDefFileBuildersItem::editor() {
	return &editor_;
}
