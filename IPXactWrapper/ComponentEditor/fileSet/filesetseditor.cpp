/* 
 *  	Created on: 25.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetseditor.cpp
 *		Project: Kactus 2
 */

#include "filesetseditor.h"
#include <common/delegates/LineEditDelegate/lineeditdelegate.h>

#include <QHBoxLayout>

FileSetsEditor::FileSetsEditor( QSharedPointer<Component> component):
ItemEditor(component),
view_(this),
model_(component, this),
proxy_(this) {

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&view_);

	proxy_.setSourceModel(&model_);

	view_.setModel(&proxy_);

	view_.setItemDelegate(new LineEditDelegate(this));

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	connect(&view_, SIGNAL(addItem(const QModelIndex&)),
		&model_, SLOT(onAddItem(const QModelIndex&)), Qt::UniqueConnection);
	connect(&view_, SIGNAL(removeItem(const QModelIndex&)),
		&model_, SLOT(onRemoveItem(const QModelIndex&)), Qt::UniqueConnection);
}

FileSetsEditor::~FileSetsEditor() {

}

bool FileSetsEditor::isValid() const {
	return false;
}

void FileSetsEditor::makeChanges() {

}

void FileSetsEditor::refresh() {

}
