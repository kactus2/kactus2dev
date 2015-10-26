//-----------------------------------------------------------------------------
// File: ComponentInstantiationEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 26.10.2015
//
// Description:
// Contains the GUI items to edit the settings of a view component instance.
//-----------------------------------------------------------------------------

#include "ComponentInstantiationEditor.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::ComponentInstantiationEditor()
//-----------------------------------------------------------------------------
ComponentInstantiationEditor::ComponentInstantiationEditor(QSharedPointer<Component> component,
    QSharedPointer<View> view, QSharedPointer<ComponentInstantiation> componentInstantiation,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionFormatter> expressionFormatter,
    QWidget *parent):
QWidget(parent),
component_(component),
view_(view),
componentInstantiation_(componentInstantiation),
language_(this), 
languageStrict_(tr("Strict"), this),
modelName_(this),
fileSetRefs_(component, tr("File set references"), this),
fileBuilders_(componentInstantiation->getDefaultFileBuilders(), this),
moduleParameters_(componentInstantiation->getModuleParameters(), component->getChoices(), parameterFinder,
                  expressionFormatter, this)
{
	fileSetRefs_.initialize();

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

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::~ComponentInstantiationEditor()
//-----------------------------------------------------------------------------
ComponentInstantiationEditor::~ComponentInstantiationEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::isValid()
//-----------------------------------------------------------------------------
bool ComponentInstantiationEditor::isValid() const
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

    return fileBuilders_.isValid() && moduleParameters_.isValid();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::refresh()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::refresh()
{
    language_.setText(componentInstantiation_->getLanguage());
    languageStrict_.setChecked(componentInstantiation_->isLanguageStrict());

    modelName_.setText(componentInstantiation_->getModuleName());
    fileSetRefs_.setItems(*componentInstantiation_->getFileSetReferences().data());

    fileBuilders_.refresh();

    moduleParameters_.refresh();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onLanguageChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onLanguageChange()
{
    componentInstantiation_->setLanguage(language_.text());
    componentInstantiation_->setLanguageStrictness(languageStrict_.isChecked());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onModelNameChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onModelNameChange(QString const& newName)
{
    componentInstantiation_->setModuleName(newName);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onFileSetRefChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onFileSetRefChange()
{
    componentInstantiation_->getFileSetReferences()->clear();
    foreach (QString setReference, fileSetRefs_.items())
    {
        componentInstantiation_->getFileSetReferences()->append(setReference);
    }

	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::showEvent()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/flatview.html");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::setupLayout()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::setupLayout()
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
    topLayout->setContentsMargins(0, 0, 0, 0);
}
