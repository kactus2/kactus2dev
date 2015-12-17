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

#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QStringList>
#include <QVBoxLayout>

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
languageEditor_(this), 
languageStrict_(tr("Strict"), this),
libraryEditor_(this),
packageEditor_(this),
modulelNameEditor_(this),
architectureEditor_(this),
configurationEditor_(this),
fileSetRefs_(component, tr("File set references"), this),
fileBuilders_(componentInstantiation->getDefaultFileBuilders(), this),
moduleParameters_(componentInstantiation->getModuleParameters(), component->getChoices(), parameterFinder,
                  expressionFormatter, this)
{
	fileSetRefs_.initialize();

	connect(&languageEditor_, SIGNAL(textEdited(const QString&)), 
        this, SLOT(onLanguageChange()), Qt::UniqueConnection);
	connect(&languageStrict_, SIGNAL(toggled(bool)),
        this, SLOT(onLanguageChange()), Qt::UniqueConnection);

    connect(&libraryEditor_, SIGNAL(textEdited(const QString&)), 
        this, SLOT(onLibraryChange()), Qt::UniqueConnection);

    connect(&packageEditor_, SIGNAL(textEdited(const QString&)), 
        this, SLOT(onPackageChange()), Qt::UniqueConnection);

	connect(&modulelNameEditor_, SIGNAL(textEdited(const QString&)), 
        this, SLOT(onModelNameChange()), Qt::UniqueConnection);

    connect(&architectureEditor_, SIGNAL(textEdited(const QString&)), 
        this, SLOT(onArchitectureChange()), Qt::UniqueConnection);

    connect(&configurationEditor_, SIGNAL(textEdited(const QString&)), 
        this, SLOT(onConfigurationChange()), Qt::UniqueConnection);

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
// Function: ComponentInstantiationEditor::refresh()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::refresh()
{
    languageEditor_.setText(componentInstantiation_->getLanguage());
    languageStrict_.setChecked(componentInstantiation_->isLanguageStrict());

    libraryEditor_.setText(componentInstantiation_->getLibraryName());

    packageEditor_.setText(componentInstantiation_->getPackageName());

    modulelNameEditor_.setText(componentInstantiation_->getModuleName());

    architectureEditor_.setText(componentInstantiation_->getArchitectureName());

    configurationEditor_.setText(componentInstantiation_->getConfigurationName());

    fileSetRefs_.setItems(*componentInstantiation_->getFileSetReferences().data());

    fileBuilders_.refresh();

    moduleParameters_.refresh();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::getComponentInstance()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> ComponentInstantiationEditor::getComponentInstance() const
{
    return componentInstantiation_;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onLanguageChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onLanguageChange()
{
    componentInstantiation_->setLanguage(languageEditor_.text());
    componentInstantiation_->setLanguageStrictness(languageStrict_.isChecked());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onLibraryChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onLibraryChange()
{
    componentInstantiation_->setLibraryName(libraryEditor_.text());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onPackageChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onPackageChange()
{
    componentInstantiation_->setPackageName(packageEditor_.text());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onModelNameChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onModelNameChange()
{
    componentInstantiation_->setModuleName(modulelNameEditor_.text());
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onArchitectureChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onArchitectureChange()
{
    componentInstantiation_->setArchitectureName(architectureEditor_.text());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onConfigurationChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onConfigurationChange()
{
    componentInstantiation_->setConfigurationName(configurationEditor_.text());
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
    QGroupBox* instanceGroup = new QGroupBox(tr("Implementation details"), this);

    QGridLayout* languageLayout = new QGridLayout();
    languageLayout->addWidget(new QLabel(tr("Language:"), this), 0, 0, 1, 1);
    languageLayout->addWidget(&languageEditor_, 0, 1, 1, 1);
    languageLayout->addWidget(&languageStrict_, 0, 2, 1, 1);

    languageLayout->addWidget(new QLabel(tr("Library:"), this), 1, 0, 1, 1);
    languageLayout->addWidget(&libraryEditor_, 1, 1, 1, 2);
    
    languageLayout->addWidget(new QLabel(tr("Package:"), this), 2, 0, 1, 1);
    languageLayout->addWidget(&packageEditor_, 2, 1, 1, 2);

    QFormLayout* moduleLaout = new QFormLayout();
    moduleLaout->addRow(tr("Module name:"), &modulelNameEditor_);
    moduleLaout->addRow(tr("Architecture:"), &architectureEditor_);
    moduleLaout->addRow(tr("Configuration:"), &configurationEditor_);

    QGridLayout* instanceLayout = new QGridLayout(instanceGroup);
    instanceLayout->addLayout(languageLayout, 0, 0, 1, 1);
    instanceLayout->addLayout(moduleLaout, 0, 1, 1, 1);
    instanceLayout->setColumnStretch(0, 50);
    instanceLayout->setColumnStretch(1, 50);

    QHBoxLayout* fileSetAndBuildLayout = new QHBoxLayout();
    fileSetAndBuildLayout->addWidget(&fileSetRefs_);
    fileSetAndBuildLayout->addWidget(&fileBuilders_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(instanceGroup);
    topLayout->addLayout(fileSetAndBuildLayout);
    topLayout->addWidget(&moduleParameters_, 1);
    topLayout->setContentsMargins(0, 0, 0, 0);
}
