/* 
 *
 *  Created on: 15.4.2011
 *      Author: Antti Kamppi
 * 		filename: flatviewgeneraltab.cpp
 */

#include "flatviewgeneraltab.h"

#include <IPXACTmodels/view.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: FlatViewGeneralTab::FlatViewGeneralTab()
//-----------------------------------------------------------------------------
FlatViewGeneralTab::FlatViewGeneralTab(QSharedPointer<Component> component, 
									   QSharedPointer<View> view, 
									   QWidget *parent): 
QWidget(parent),
component_(component),
view_(view), 
language_(this), 
languageStrict_(tr("Strict"), this),
modelName_(this),
fileSetRefs_(component, tr("File set references"), this)
{
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
	topLayout->setContentsMargins(0, 0, 0, 0);

	connect(&language_, SIGNAL(textEdited(const QString&)),	this, SLOT(onLanguageChange()), Qt::UniqueConnection);
	connect(&languageStrict_, SIGNAL(toggled(bool)), this, SLOT(onLanguageChange()), Qt::UniqueConnection);
	connect(&modelName_, SIGNAL(textEdited(const QString&)), 
        this, SLOT(onModelNameChange(const QString&)), Qt::UniqueConnection);
	connect(&fileSetRefs_, SIGNAL(contentChanged()), this, SLOT(onFileSetRefChange()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FlatViewGeneralTab::~FlatViewGeneralTab()
//-----------------------------------------------------------------------------
FlatViewGeneralTab::~FlatViewGeneralTab()
{

}

//-----------------------------------------------------------------------------
// Function: FlatViewGeneralTab::isValid()
//-----------------------------------------------------------------------------
bool FlatViewGeneralTab::isValid() const
{
	// check the file set references that they are to valid file sets.
	QStringList fileSetRefs = fileSetRefs_.items();
	foreach (QString filesetName, fileSetRefs)
    {
		// if the component does not contain the referenced file set.
		if (!component_->hasFileSet(filesetName))
        {
			return false;
		}
	}

	if (language_.text().isEmpty() && languageStrict_.isChecked())
    {
		return false;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: FlatViewGeneralTab::refresh()
//-----------------------------------------------------------------------------
void FlatViewGeneralTab::refresh()
{
	language_.setText(view_->getLanguage());
	languageStrict_.setChecked(view_->getLanguageStrict());

	modelName_.setText(view_->getModelName());
	fileSetRefs_.setItems(view_->getFileSetRefs());
}

//-----------------------------------------------------------------------------
// Function: FlatViewGeneralTab::onLanguageChange()
//-----------------------------------------------------------------------------
void FlatViewGeneralTab::onLanguageChange()
{
	view_->setLanguage(language_.text());
	view_->setLanguageStrict(languageStrict_.isChecked());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FlatViewGeneralTab::onModelNameChange()
//-----------------------------------------------------------------------------
void FlatViewGeneralTab::onModelNameChange(QString const& newName)
{
	view_->setModelName(newName);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FlatViewGeneralTab::onFileSetRefChange()
//-----------------------------------------------------------------------------
void FlatViewGeneralTab::onFileSetRefChange()
{
	view_->setFileSetRefs(fileSetRefs_.items());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FlatViewGeneralTab::showEvent()
//-----------------------------------------------------------------------------
void FlatViewGeneralTab::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/flatview.html");
}
