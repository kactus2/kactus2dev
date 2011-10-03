/* 
 *
 *  Created on: 10.5.2011
 *      Author: Antti Kamppi
 * 		filename: filebuildereditor.cpp
 */

#include "filebuildereditor.h"

#include <QHBoxLayout>

FileBuilderEditor::FileBuilderEditor(QSharedPointer<Component> component,
									 void* dataPointer, 
									 QWidget *parent): 
ItemEditor(component, parent), 
editor_(dataPointer, this) {

	connect(&editor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(noticeMessage(const QString&)),
		this, SIGNAL(noticeMessage(const QString&)), Qt::UniqueConnection);
	connect(&editor_, SIGNAL(errorMessage(const QString&)),
		this, SIGNAL(errorMessage(const QString&)), Qt::UniqueConnection);

	QHBoxLayout* layout = new QHBoxLayout(this);
	layout->addWidget(&editor_);

	editor_.restoreChanges();
}

FileBuilderEditor::~FileBuilderEditor() {
}

bool FileBuilderEditor::isValid() const {
	return editor_.isValid();
}

void FileBuilderEditor::makeChanges() {
	editor_.applyChanges();
}
