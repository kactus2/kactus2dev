//-----------------------------------------------------------------------------
// File: ComponentInstantiationEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
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

#include <KactusAPI/include/ComponentInstantiationInterface.h>

#include <QGridLayout>
#include <QFormLayout>
#include <QLabel>
#include <QScrollArea>
#include <QStringList>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::ComponentInstantiationEditor()
//-----------------------------------------------------------------------------
ComponentInstantiationEditor::ComponentInstantiationEditor(QSharedPointer<Component> component,
    LibraryInterface* library, QSharedPointer<ComponentInstantiation> componentInstantiation,
    QSharedPointer<ParameterFinder> parameterFinder, QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter,
    ComponentInstantiationInterface* instantiationInterface, QWidget *parent):
ParameterItemEditor(component, library, parent),
component_(component),
nameGroupEditor_(componentInstantiation, this, tr("Component instance name and description")),
languageEditor_(this), 
languageStrict_(tr("strict"), this),
libraryEditor_(this),
packageEditor_(this),
modulelNameEditor_(this),
architectureEditor_(this),
configurationEditor_(this),
fileSetRefs_(instantiationInterface->getFileSetInterface(), tr("File set references"), this),
fileBuilders_(instantiationInterface->getFileBuilderInterface(), parameterFinder, expressionParser,
              expressionFormatter, componentInstantiation->getDefaultFileBuilders(), this),
moduleParameters_(componentInstantiation, component->getChoices(), parameterFinder, expressionFormatter,
    instantiationInterface->getModuleParameterInterface(), this),
parameters_(componentInstantiation->getParameters(), component->getChoices(), parameterFinder, expressionFormatter,
    this),
instantiationInterface_(instantiationInterface),
availableInstantiations_(component->getComponentInstantiations())
{
    instantiationInterface_->setComponentInstantiations(availableInstantiations_);

    fileSetRefs_.initialize();

    connect(&nameGroupEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

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
    connect(&moduleParameters_, SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    connect(&moduleParameters_,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        this,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        Qt::UniqueConnection);

    connect(&parameters_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(openReferenceTree(QString const&, QString const&)),
        this, SIGNAL(openReferenceTree(QString const&, QString const&)), Qt::UniqueConnection);

    connect(&parameters_,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        this,
        SIGNAL(recalculateReferencesToParameters(QVector<QString> const&, AbstractParameterInterface*)),
        Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::refresh()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::refresh()
{
    instantiationInterface_->setComponentInstantiations(availableInstantiations_);

    blockSignals(true);

    nameGroupEditor_.refresh();
    std::string instantiationName = nameGroupEditor_.name().toStdString();

    languageEditor_.setText(QString::fromStdString(instantiationInterface_->getLanguage(instantiationName)));
    languageStrict_.setChecked(instantiationInterface_->isLanguageStrict(instantiationName));

    libraryEditor_.setText(QString::fromStdString(instantiationInterface_->getLibraryName(instantiationName)));

    packageEditor_.setText(QString::fromStdString(instantiationInterface_->getPackageName(instantiationName)));

    modulelNameEditor_.setText(QString::fromStdString(instantiationInterface_->getModuleName(instantiationName)));

    architectureEditor_.setText(
        QString::fromStdString(instantiationInterface_->getArchitectureName(instantiationName)));

    configurationEditor_.setText(
        QString::fromStdString(instantiationInterface_->getConfigurationName(instantiationName)));

    QStringList fileSetReferences;
    for (auto fileReference : instantiationInterface_->getFileSetReferences(instantiationName))
    {
        fileSetReferences.append(QString::fromStdString(fileReference));
    }
    fileSetRefs_.setItems(fileSetReferences);

    fileBuilders_.refresh();

    moduleParameters_.refresh();

    parameters_.refresh();

    blockSignals(false);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onLanguageChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onLanguageChange()
{
    std::string instantiationName = nameGroupEditor_.name().toStdString();

    instantiationInterface_->setLanguage(instantiationName, languageEditor_.text().toStdString());
    instantiationInterface_->setLanguageStrictness(instantiationName, languageStrict_.isChecked());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onLibraryChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onLibraryChange()
{
    instantiationInterface_->setLibraryName(nameGroupEditor_.name().toStdString(),
        libraryEditor_.text().toStdString());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onPackageChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onPackageChange()
{
    instantiationInterface_->setPackageName(nameGroupEditor_.name().toStdString(),
        packageEditor_.text().toStdString());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onModelNameChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onModelNameChange()
{
    instantiationInterface_->setModuleName(nameGroupEditor_.name().toStdString(),
        modulelNameEditor_.text().toStdString());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onArchitectureChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onArchitectureChange()
{
    instantiationInterface_->setArchitectureName(nameGroupEditor_.name().toStdString(),
        architectureEditor_.text().toStdString());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onConfigurationChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onConfigurationChange()
{
    instantiationInterface_->setConfigurationName(nameGroupEditor_.name().toStdString(),
        configurationEditor_.text().toStdString());
    emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationEditor::onFileSetRefChange()
//-----------------------------------------------------------------------------
void ComponentInstantiationEditor::onFileSetRefChange()
{
    std::vector<std::string> newReferences;
    for (auto setReference : fileSetRefs_.items())
    {
        newReferences.push_back(setReference.toStdString());
    }
    instantiationInterface_->setFileSetReferences(nameGroupEditor_.name().toStdString(), newReferences);

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
    nameGroupEditor_.setMaximumHeight(QWIDGETSIZE_MAX);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QVBoxLayout* scrollLayout = new QVBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setContentsMargins(0, 0, 0, 0);

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

    QGridLayout* topLayout = new QGridLayout(topWidget);
    topLayout->addWidget(&nameGroupEditor_, 0, 0, 1, 1);
    topLayout->addWidget(implementationGroup, 0, 1, 1, 1);
    topLayout->addWidget(&fileSetRefs_, 1, 0, 1, 1);
    topLayout->addWidget(&fileBuilders_, 1, 1, 1, 1);
    topLayout->addWidget(&moduleParameters_, 2, 0, 1, 2);
    topLayout->addWidget(&parameters_, 3, 0, 1, 2);
    topLayout->setContentsMargins(0, 0, 0, 0);

    topLayout->setRowStretch(0, 1);
    topLayout->setRowStretch(1, 2);
    topLayout->setRowStretch(2, 5);

    scrollArea->setWidget(topWidget);
}
