/* 
 *  	Created on: 16.5.2012
 *      Author: Antti Kamppi
 * 		filename: componenteditordeffilebuildersitem.cpp
 *		Project: Kactus 2
 */

#include "componenteditordeffilebuildersitem.h"

#include <QFont>
#include <QApplication>

ComponentEditorDefFileBuildersItem::ComponentEditorDefFileBuildersItem(
	QList<QSharedPointer<FileBuilder> >& fileBuilders,
	ComponentEditorTreeModel* model,
	LibraryInterface* libHandler,
	QSharedPointer<Component> component,
	ComponentEditorItem* parent):
ComponentEditorItem(model, libHandler, component, parent),
fileBuilders_(fileBuilders),
editor_(component, &fileBuilders) {
	editor_.hide();
	connect(&editor_, SIGNAL(contentChanged()),
		this, SLOT(onEditorChanged()), Qt::UniqueConnection);
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

QFont ComponentEditorDefFileBuildersItem::getFont() const {
	return QApplication::font();
}

QString ComponentEditorDefFileBuildersItem::getTooltip() const {
	return tr("Specifies the build commands for the files within file set");
}
