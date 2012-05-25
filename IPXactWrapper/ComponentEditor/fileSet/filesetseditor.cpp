/* 
 *  	Created on: 25.5.2012
 *      Author: Antti Kamppi
 * 		filename: filesetseditor.cpp
 *		Project: Kactus 2
 */

#include "filesetseditor.h"

#include <QHBoxLayout>

FileSetsEditor::FileSetsEditor( QSharedPointer<Component> component):
ItemEditor(component),
view_(this),
model_(component, this) {

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&view_);

	view_.setModel(&model_);

	connect(&model_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

FileSetsEditor::~FileSetsEditor() {

}

bool FileSetsEditor::isValid() const {
	return false;
}

void FileSetsEditor::makeChanges() {

}
