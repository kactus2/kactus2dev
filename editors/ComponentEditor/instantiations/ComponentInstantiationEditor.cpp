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
        LibraryInterface* library, QSharedPointer<ComponentInstantiation> componentInstantiation,
        QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget *parent):
ItemEditor(component, library, parent),
component_(component),
componentInstantiation_(componentInstantiation),
nameGroupEditor_(componentInstantiation, this, tr("Component instance name and description")),
languageEditor_(this), 
languageStrict_(tr("Strict"), this),
libraryEditor_(this),
packageEditor_(this),
modulelNameEditor_(this),
architectureEditor_(this),
configurationEditor_(this),
fileSetRefs_(component, tr("File set references"), this),
fileBuilders_(componentInstantiation->getDefaultFileBuilders(), parameterFinder, expressionParser,
              expressionFormatter, this),
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

    connect(&fileBuilders_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&fileBuilders_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);

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
    blockSignals(true);

    nameGroupEditor_.refresh();

    languageEditor_.setText(componentInstantiation_->getLanguage());
    languageStrict_.setChecked(componentInstantiation_->isLanguageStrict());

    libraryEditor_.setText(componentInstantiation_->getLibraryName());

    packageEditor_.setText(componentInstantiation_->getPackageName());

    modulelNameEditor_.setText(componentInstantiation_->getModuleName());

    architectureEditor_.setText(componentInstantiation_->getArchitectureName());

    configurationEditor_.setText(componentInstantiation_->getConfigurationName());

    fileSetRefs_.setItems(*componentInstantiation_->getFileSetReferences());

    fileBuilders_.refresh();

    moduleParameters_.refresh();

    blockSignals(false);
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

    componentInstantiation_->getFileSetReferences()->append(fileSetRefs_.items());


	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::showEvent()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/componentInstantiation.html");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::setupLayout()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::setupLayout()
{
    QGroupBox* implementationGroup = new QGroupBox(tr("Implementation details"), this);

    QGridLayout* implementationLayout = new QGridLayout(implementationGroup);
    implementationLayout->addWidget(new QLabel(tr("Language:"), this), 0, 0, 1, 1);
    implementationLayout->addWidget(&languageEditor_, 0, 1, 1, 1);
    implementationLayout->addWidget(&languageStrict_, 0, 2, 1, 1);

    implementationLayout->addWidget(new QLabel(tr("Library:"), this), 1, 0, 1, 1);
    implementationLayout->addWidget(&libraryEditor_, 1, 1, 1, 2);
    
    implementationLayout->addWidget(new QLabel(tr("Package:"), this), 2, 0, 1, 1);
    implementationLayout->addWidget(&packageEditor_, 2, 1, 1, 2);

    implementationLayout->addWidget(new QLabel(tr("Module name:"), this), 3, 0, 1, 1);
    implementationLayout->addWidget(&modulelNameEditor_, 3, 1, 1, 2);

    implementationLayout->addWidget(new QLabel(tr("Architecture:"), this), 4, 0, 1, 1);
    implementationLayout->addWidget(&architectureEditor_, 4, 1, 1, 2);

    implementationLayout->addWidget(new QLabel(tr("Configuration:"), this), 5, 0, 1, 1);
    implementationLayout->addWidget(&configurationEditor_, 5, 1, 1, 2);

    QHBoxLayout* nameAndImplementationLayout = new QHBoxLayout();
    nameAndImplementationLayout->addWidget(&nameGroupEditor_, 0, Qt::AlignTop);
    nameAndImplementationLayout->addWidget(implementationGroup);

    QHBoxLayout* fileSetAndBuildLayout = new QHBoxLayout();
    fileSetAndBuildLayout->addWidget(&fileSetRefs_);
    fileSetAndBuildLayout->addWidget(&fileBuilders_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addLayout(nameAndImplementationLayout);
    topLayout->addLayout(fileSetAndBuildLayout);
    topLayout->addWidget(&moduleParameters_, 1);
    topLayout->setContentsMargins(0, 0, 0, 0);
}
