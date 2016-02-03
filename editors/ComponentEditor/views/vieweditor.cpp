//-----------------------------------------------------------------------------
// File: vieweditor.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 14.4.2011
//
// Description:
// Editor to edit a view within a component.
//-----------------------------------------------------------------------------

#include "vieweditor.h"

#include "envidentifiereditor.h"

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/common/NameGenerationPolicy.h>

#include <editors/ComponentEditor/common/ReferenceSelector/ReferenceSelector.h>
#include <editors/ComponentEditor/instantiations/ComponentInstantiationDisplayer.h>
#include <editors/ComponentEditor/instantiations/ModuleParameterEditor.h>

#include <common/widgets/nameGroupEditor/namegroupeditor.h>
#include <common/widgets/vlnvDisplayer/vlnvdisplayer.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/common/FileBuilder.h>

#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QSizePolicy>
#include <QScrollArea>

//-----------------------------------------------------------------------------
// Function: ViewEditor::ViewEditor()
//-----------------------------------------------------------------------------
ViewEditor::ViewEditor(QSharedPointer<Component> component, QSharedPointer<View> view,
    LibraryInterface* libHandler, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget *parent):
ItemEditor(component, libHandler, parent),
    view_(view),
    nameEditor_(new NameGroupEditor(view, this, tr("View name and description"))),
    envIdentifier_(new EnvIdentifierEditor(view, this)),
    componentInstantiationSelector_(new ReferenceSelector(this)),
    designConfigurationInstantiationSelector_(new ReferenceSelector(this)),
    designInstantiationSelector_(new ReferenceSelector(this)),
    componentInstantiationDisplay_(new ComponentInstantiationDisplayer(this)),
    hierarchyGroup_(new QGroupBox(tr("Design and configuration"), this)),
    designConfigurationDisplay_(new VLNVDisplayer(this)),
    designDisplay_(new VLNVDisplayer(this)),
    moduleParameterEditor_(new ModuleParameterEditor(QSharedPointer<QList<QSharedPointer<ModuleParameter> > >(
    new QList<QSharedPointer<ModuleParameter> >()), component->getChoices(), parameterFinder, expressionFormatter,
    this))
{
    moduleParameterEditor_->disableEditing();

    setupLayout();

    refresh();

    connect(nameEditor_, SIGNAL(contentChanged()),	this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(envIdentifier_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(componentInstantiationSelector_, SIGNAL(itemSelected(QString const&)),
        this, SLOT(onComponentInstanceChanged(QString const&)), Qt::UniqueConnection);
    connect(designConfigurationInstantiationSelector_, SIGNAL(itemSelected(QString const&)),
        this, SLOT(onDesignConfigurationInstanceChanged(QString const&)), Qt::UniqueConnection);
    connect(designInstantiationSelector_, SIGNAL(itemSelected(QString const&)),
        this, SLOT(onDesignInstanceChanged(QString const&)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::~ViewEditor()
//-----------------------------------------------------------------------------
ViewEditor::~ViewEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ViewEditor::refresh()
//-----------------------------------------------------------------------------
void ViewEditor::refresh()
{
	nameEditor_->refresh();
	envIdentifier_->refresh();

    QStringList componentInstantiationNames;
    foreach(QSharedPointer<ComponentInstantiation> componentInstantiation, 
        *component()->getComponentInstantiations())
    {
        componentInstantiationNames.append(componentInstantiation->name());
    }
    componentInstantiationSelector_->refresh(componentInstantiationNames);
    componentInstantiationSelector_->selectItem(view_->getComponentInstantiationRef());
    onComponentInstanceChanged(view_->getComponentInstantiationRef());

    QStringList designConfigurationInstantiationNames;
    foreach(QSharedPointer<DesignConfigurationInstantiation> designConfigurationInstantiation, 
        *component()->getDesignConfigurationInstantiations())
    {
        designConfigurationInstantiationNames.append(designConfigurationInstantiation->name());
        if (designConfigurationInstantiation->name() == view_->getDesignConfigurationInstantiationRef())
        {
            designConfigurationDisplay_->setVLNV(
                *designConfigurationInstantiation->getDesignConfigurationReference());
        }
    }
    designConfigurationInstantiationSelector_->refresh(designConfigurationInstantiationNames);
    designConfigurationInstantiationSelector_->selectItem(view_->getDesignConfigurationInstantiationRef());
    onDesignConfigurationInstanceChanged(view_->getDesignConfigurationInstantiationRef());

    QStringList designInstantiationNames;
    foreach(QSharedPointer<DesignInstantiation> designInstantiation, *component()->getDesignInstantiations())
    {
        designInstantiationNames.append(designInstantiation->name());
        if (designInstantiation->name() == view_->getDesignInstantiationRef())
        {
            designDisplay_->setVLNV(*designInstantiation->getDesignReference());
        }
    }
    designInstantiationSelector_->refresh(designInstantiationNames);
    designInstantiationSelector_->selectItem(view_->getDesignInstantiationRef());
    onDesignInstanceChanged(view_->getDesignInstantiationRef()); 
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::showEvent()
//-----------------------------------------------------------------------------
void ViewEditor::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    emit helpUrlRequested("componenteditor/view.html");
    refresh();
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::onComponentInstanceChanged()
//-----------------------------------------------------------------------------
void ViewEditor::onComponentInstanceChanged(QString const& instanceName)
{
    QString previousInstance = view_->getComponentInstantiationRef();

    view_->setComponentInstantiationRef(instanceName);

    QSharedPointer<ComponentInstantiation> instantiation = 
        component()->getModel()->findComponentInstantiation(instanceName);
    componentInstantiationDisplay_->setInstantiation(instantiation);

    if (instantiation)
    {
        moduleParameterEditor_->setModuleParameters(instantiation->getModuleParameters());
    }
    moduleParameterEditor_->setVisible(instantiation);

    if (previousInstance != instanceName)
    {
        emit contentChanged();
    }
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::onDesignConfigurationInstanceChanged()
//-----------------------------------------------------------------------------
void ViewEditor::onDesignConfigurationInstanceChanged(QString const& instanceName)
{    
    QString previousInstance = view_->getDesignConfigurationInstantiationRef();

    view_->setDesignConfigurationInstantiationRef(instanceName);

    QSharedPointer<DesignConfigurationInstantiation> selectedInstantiation;

    foreach(QSharedPointer<DesignConfigurationInstantiation> configurationInstantiation, 
        *component()->getDesignConfigurationInstantiations())
    {
        if (configurationInstantiation->name() == instanceName)
        {
            selectedInstantiation = configurationInstantiation;
        }
    }

    VLNV selectedVLNV;
    if (selectedInstantiation)
    {
        selectedVLNV = *selectedInstantiation->getDesignConfigurationReference();
    }

    hierarchyGroup_->setVisible(!instanceName.isEmpty() || !designInstantiationSelector_->currentText().isEmpty());
    designConfigurationDisplay_->setVisible(selectedInstantiation);
    designConfigurationDisplay_->setVLNV(selectedVLNV);

    if (previousInstance != instanceName)
    {
        emit contentChanged();
    }    
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::onDesignInstanceChanged()
//-----------------------------------------------------------------------------
void ViewEditor::onDesignInstanceChanged(QString const& instanceName)
{    
    QString previousInstance = view_->getDesignInstantiationRef();

    view_->setDesignInstantiationRef(instanceName);

    QSharedPointer<DesignInstantiation> selectedInstantiation;

    foreach(QSharedPointer<DesignInstantiation> designInstantiation, *component()->getDesignInstantiations())
    {
        if (designInstantiation->name() == instanceName)
        {
            selectedInstantiation = designInstantiation;
        }
    }

    VLNV selectedVLNV;
    if (selectedInstantiation)
    {
        selectedVLNV = *selectedInstantiation->getDesignReference();
    }

    hierarchyGroup_->setVisible(!instanceName.isEmpty() || 
        !designConfigurationInstantiationSelector_->currentText().isEmpty());
    designDisplay_->setVisible(selectedInstantiation);
    designDisplay_->setVLNV(selectedVLNV);

    if (previousInstance != instanceName)
    {
        emit contentChanged();
    }    
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::setupLayout()
//-----------------------------------------------------------------------------
void ViewEditor::setupLayout()
{
    // create the scroll area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QGroupBox* instantiationsGroup = new QGroupBox(tr("Instantiations"), this);
    QFormLayout* instancesLayout = new QFormLayout(instantiationsGroup);
    instancesLayout->addRow(tr("Component instantiation:"), componentInstantiationSelector_);
    instancesLayout->addRow(tr("Design configuration instantiation:"), designConfigurationInstantiationSelector_);
    instancesLayout->addRow(tr("Design instantiation:"), designInstantiationSelector_);

    QVBoxLayout* componentLayout = new QVBoxLayout();
    componentLayout->addWidget(componentInstantiationDisplay_, Qt::AlignTop);

    QVBoxLayout* editorsLayout = new QVBoxLayout(hierarchyGroup_);
    editorsLayout->addWidget(designConfigurationDisplay_, 0, Qt::AlignTop);
    editorsLayout->addWidget(designDisplay_, 0, Qt::AlignTop);
    editorsLayout->addStretch();

    // create the top widget and set it under the scroll area
    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    // create the layout for the top widget
    QGridLayout* topLayout = new QGridLayout(topWidget);
    topLayout->addWidget(nameEditor_, 0, 0, 1, 1, Qt::AlignTop);
    topLayout->addWidget(envIdentifier_, 0, 1, 2, 1, Qt::AlignTop);
    topLayout->addWidget(instantiationsGroup, 1, 0, 1, 1, Qt::AlignTop);
    topLayout->addLayout(componentLayout, 2, 0, 1, 1);
    topLayout->addWidget(hierarchyGroup_, 2, 1, 1, 1);
    topLayout->addWidget(moduleParameterEditor_, 3, 0, 1, 2);
    topLayout->setRowStretch(3, 1);
    topLayout->setContentsMargins(0, 0, 0, 0);
}
