/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: flatviewgeneraltab.cpp
 */

#include "flatviewgeneraltab.h"

#include <models/view.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStringList>

FlatViewGeneralTab::FlatViewGeneralTab(QSharedPointer<Component> component, 
									   View* view, 
									   QWidget *parent): 
QWidget(parent),
component_(component),
view_(view), 
language_(this), 
languageStrict_(tr("Strict"), this),
modelName_(this),
fileSetRefs_(component, tr("File set references"), this) {

	fileSetRefs_.initialize();

	// create the labels for user to identify the editors
	QLabel* languageLabel = new QLabel(tr("Language"), this);
	QLabel* modelLabel = new QLabel(tr("Model name"), this);

	QGridLayout* gridLayout = new QGridLayout();
	gridLayout->addWidget(languageLabel, 0, 0, 1, 1);
	gridLayout->addWidget(&language_, 0, 1, 1, 1);
	gridLayout->addWidget(&languageStrict_, 0, 2, 1, 1);
	gridLayout->addWidget(modelLabel, 1, 0, 1, 1);
	gridLayout->addWidget(&modelName_, 1, 1, 1, 1);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addLayout(gridLayout);
	topLayout->addWidget(&fileSetRefs_, 1);

	restoreChanges();

	connect(&language_, SIGNAL(textEdited(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&languageStrict_, SIGNAL(toggled(bool)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&modelName_, SIGNAL(textEdited(const QString&)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&fileSetRefs_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

FlatViewGeneralTab::~FlatViewGeneralTab() {
}

bool FlatViewGeneralTab::isValid() const {

	// check the file set references that they are to valid file sets.
	QStringList fileSetRefs = fileSetRefs_.items();
	foreach (QString ref, fileSetRefs) {
		
		// if the component does not contain the referenced file set.
		if (!component_->hasFileSet(ref)) {
			return false;
		}
	}

	if (language_.text().isEmpty() && languageStrict_.isChecked())
		return false;

	return true;
}

void FlatViewGeneralTab::restoreChanges() {

	language_.setText(view_->getLanguage());
	languageStrict_.setChecked(view_->getLanguageStrict());
	modelName_.setText(view_->getModelName());
	fileSetRefs_.setItems(view_->getFileSetRefs());
}

void FlatViewGeneralTab::applyChanges() {

	view_->setLanguage(language_.text());
	view_->setLanguageStrict(languageStrict_.isChecked());
	view_->setModelName(modelName_.text());
	view_->setFileSetRefs(fileSetRefs_.items());
}
