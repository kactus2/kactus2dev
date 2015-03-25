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
    QSharedPointer<View> view, QSharedPointer<ParameterFinder> parameterFinder, 
    QSharedPointer<ExpressionFormatter> expressionFormatter,
									   QWidget *parent): 
QWidget(parent),
component_(component),
view_(view), 
language_(this), 
languageStrict_(tr("Strict"), this),
modelName_(this),
fileSetRefs_(component, tr("File set references"), this),
fileBuilders_(view->getDefaultFileBuilders(), this),
moduleParameters_(view->getModuleParameters(), component, parameterFinder, expressionFormatter, this),
parameters_(view->getParameters(), component, parameterFinder, expressionFormatter, this)
{
	fileSetRefs_.initialize();

    moduleParameters_.setTitle(tr("Module parameters"));

    parameters_.setTitle(tr("View-specific generator parameters"));

	connect(&language_, SIGNAL(textEdited(const QString&)),	this, SLOT(onLanguageChange()), Qt::UniqueConnection);
	connect(&languageStrict_, SIGNAL(toggled(bool)), this, SLOT(onLanguageChange()), Qt::UniqueConnection);
	connect(&modelName_, SIGNAL(textEdited(const QString&)), 
        this, SLOT(onModelNameChange(const QString&)), Qt::UniqueConnection);

	connect(&fileSetRefs_, SIGNAL(contentChanged()), this, SLOT(onFileSetRefChange()), Qt::UniqueConnection);

    connect(&fileBuilders_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&moduleParameters_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&moduleParameters_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&moduleParameters_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&moduleParameters_, SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

    connect(&parameters_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

    setupLayout();
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

    return fileBuilders_.isValid() && moduleParameters_.isValid() && parameters_.isValid();
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

    fileBuilders_.refresh();
    parameters_.refresh();
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

//-----------------------------------------------------------------------------
// Function: FlatViewGeneralTab::setupLayout()
//-----------------------------------------------------------------------------
void FlatViewGeneralTab::setupLayout()
{
    // create the labels for user to identify the editors
    QLabel* languageLabel = new QLabel(tr("Language"), this);
    QLabel* modelLabel = new QLabel(tr("Model name"), this);

    QGridLayout* languageAndModelLayout = new QGridLayout();
    languageAndModelLayout->addWidget(languageLabel, 0, 0, 1, 1);
    languageAndModelLayout->addWidget(&language_, 0, 1, 1, 1);
    languageAndModelLayout->addWidget(&languageStrict_, 0, 2, 1, 1);
    languageAndModelLayout->addWidget(modelLabel, 1, 0, 1, 1);
    languageAndModelLayout->addWidget(&modelName_, 1, 1, 1, 1);

    QGridLayout* halfPageLayout = new QGridLayout();
    halfPageLayout->addLayout(languageAndModelLayout, 0, 0, 1, 1);
    halfPageLayout->setColumnStretch(0, 50);
    halfPageLayout->setColumnStretch(1, 50);

    QHBoxLayout* fileSetAndBuildLayout = new QHBoxLayout();
    fileSetAndBuildLayout->addWidget(&fileSetRefs_);
    fileSetAndBuildLayout->addWidget(&fileBuilders_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(halfPageLayout);
    topLayout->addLayout(fileSetAndBuildLayout);
    topLayout->addWidget(&moduleParameters_, 1);
    topLayout->addWidget(&parameters_, 1);
    topLayout->setContentsMargins(0, 0, 0, 0);
}
