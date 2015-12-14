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

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/common/NameGenerationPolicy.h>

#include <IPXACTmodels/Component/Component.h>
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
                       QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget *parent /* = 0 */):
ItemEditor(component, libHandler, parent),
view_(view),
nameEditor_(view, this, tr("View name and description")),
envIdentifier_(view, this),
componentInstantiationEditor_(0),
hierarchyReferenceEditor_(0)
{
    componentInstantiationEditor_ = new ComponentInstantiationEditor(component, view, getComponentInstantiation(),
        parameterFinder, expressionFormatter, this);

    hierarchyReferenceEditor_ = new HierarchyRefWidget(view, getDesignInstantiation(),
        getDesignConfigurationInstantiation(), libHandler,
        component->getChoices(), parameterFinder, expressionFormatter, this);

	setupLayout();

	connect(&nameEditor_, SIGNAL(contentChanged()),	this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	
    connect(&envIdentifier_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);


    connect(hierarchyReferenceEditor_, SIGNAL(contentChanged()), 
        this, SLOT(onHierarchyChanged()), Qt::UniqueConnection);

    connect(hierarchyReferenceEditor_, SIGNAL(contentChanged()), 
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(hierarchyReferenceEditor_, SIGNAL(helpUrlRequested(const QString&)),
        this, SIGNAL(helpUrlRequested(const QString&)), Qt::UniqueConnection);
    connect(hierarchyReferenceEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(hierarchyReferenceEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(hierarchyReferenceEditor_, SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

    connect(componentInstantiationEditor_, SIGNAL(contentChanged()), 
        this, SLOT(onComponentInstanceChanged()), Qt::UniqueConnection);

    connect(componentInstantiationEditor_, SIGNAL(contentChanged()),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(componentInstantiationEditor_, SIGNAL(helpUrlRequested(const QString&)),
		this, SIGNAL(helpUrlRequested(const QString&)), Qt::UniqueConnection);
    connect(componentInstantiationEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(componentInstantiationEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(componentInstantiationEditor_, SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

	refresh();
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::~ViewEditor()
//-----------------------------------------------------------------------------
ViewEditor::~ViewEditor()
{

}

//-----------------------------------------------------------------------------
// Function: ViewEditor::isValid()
//-----------------------------------------------------------------------------
bool ViewEditor::isValid() const
{
    return nameEditor_.isValid() || envIdentifier_.isValid() || componentInstantiationEditor_->isValid() ||
        hierarchyReferenceEditor_->isValid();
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::refresh()
//-----------------------------------------------------------------------------
void ViewEditor::refresh()
{
	nameEditor_.refresh();
	envIdentifier_.refresh();

    componentInstantiationEditor_->refresh();    

    hierarchyReferenceEditor_->refresh();
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::onComponentInstanceChanged()
//-----------------------------------------------------------------------------
void ViewEditor::onComponentInstanceChanged()
{
    QSharedPointer<ComponentInstantiation> instantiation = componentInstantiationEditor_->getComponentInstance();
    if (!component()->getComponentInstantiations()->contains(instantiation))
    {
        view_->setComponentInstantiationRef(instantiation->name());
        component()->getComponentInstantiations()->append(instantiation);
    }
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::onHierarchyChanged()
//-----------------------------------------------------------------------------
void ViewEditor::onHierarchyChanged()
{
    QSharedPointer<DesignInstantiation> designInstantiation = hierarchyReferenceEditor_->getDesignInstantiation();
    if (!component()->getDesignInstantiations()->contains(designInstantiation))
    {
        view_->setDesignInstantiationRef(designInstantiation->name());
        component()->getDesignInstantiations()->append(designInstantiation);
    }

    QSharedPointer<DesignConfigurationInstantiation> configInstantiation =
        hierarchyReferenceEditor_->getDesignConfigurationInstantiation();
    if (!component()->getDesignConfigurationInstantiations()->contains(configInstantiation))
    {
        view_->setDesignConfigurationInstantiationRef(configInstantiation->name());
        component()->getDesignConfigurationInstantiations()->append(configInstantiation);
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::getComponentInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstantiation> ViewEditor::getComponentInstantiation() const
{
    foreach (QSharedPointer<ComponentInstantiation> instantiation, *component()->getComponentInstantiations())
    {
        if (instantiation->name() == view_->getComponentInstantiationRef())
        {
            return instantiation;
        }
    }

    return QSharedPointer<ComponentInstantiation> (new ComponentInstantiation(view_->name() + "_component"));
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::getDesignInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignInstantiation> ViewEditor::getDesignInstantiation() const
{
    foreach (QSharedPointer<DesignInstantiation> instantiation, *component()->getDesignInstantiations())
    {
        if (instantiation->name() == view_->getDesignInstantiationRef())
        {
            return instantiation;
        }
    }

    return QSharedPointer<DesignInstantiation> (new DesignInstantiation(
        NameGenerationPolicy::designInstantiationName(view_->name())));
}

//-----------------------------------------------------------------------------
// Function: ComponentEditorViewItem::getDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfigurationInstantiation> ViewEditor::getDesignConfigurationInstantiation()
    const
{
    foreach (QSharedPointer<DesignConfigurationInstantiation> instantiation,
        *component()->getDesignConfigurationInstantiations())
    {
        if (instantiation->name() == view_->getDesignConfigurationInstantiationRef())
        {
            return instantiation;
        }
    }

    return QSharedPointer<DesignConfigurationInstantiation> (new DesignConfigurationInstantiation(
        NameGenerationPolicy::designConfigurationInstantiationName(view_->name())));
}


//-----------------------------------------------------------------------------
// Function: ViewEditor::setupLayout()
//-----------------------------------------------------------------------------
void ViewEditor::setupLayout()
{
    envIdentifier_.setMaximumHeight(175);

    // create the scroll area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    QHBoxLayout* scrollLayout = new QHBoxLayout(this);
    scrollLayout->addWidget(scrollArea);
    scrollLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* nameAndEnvIdentifiersLayout = new QHBoxLayout();
    nameAndEnvIdentifiersLayout->addWidget(&nameEditor_, 0, Qt::AlignTop);
    nameAndEnvIdentifiersLayout->addWidget(&envIdentifier_, 0, Qt::AlignTop);

    QVBoxLayout* editorsLayout = new QVBoxLayout();
    editorsLayout->addWidget(componentInstantiationEditor_, 0);
    editorsLayout->addWidget(hierarchyReferenceEditor_, 0);

    // create the top widget and set it under the scroll area
    QWidget* topWidget = new QWidget(scrollArea);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scrollArea->setWidget(topWidget);

    // create the layout for the top widget
    QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
    topLayout->addLayout(nameAndEnvIdentifiersLayout);
    topLayout->addLayout(editorsLayout);
    topLayout->addStretch(1);
    topLayout->setContentsMargins(0, 0, 0, 0);
}
