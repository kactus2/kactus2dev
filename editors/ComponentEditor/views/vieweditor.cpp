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
    QSharedPointer<ComponentInstantiation> componentInstantiation,
    QSharedPointer<DesignInstantiation> designInstantiation,
    QSharedPointer<DesignConfigurationInstantiation> designConfigurationInstantiation,
    LibraryInterface* libHandler, QSharedPointer<ParameterFinder> parameterFinder,
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget *parent /* = 0 */):
ItemEditor(component, libHandler, parent), 
view_(view),
nameEditor_(view, this, tr("View name and description")),
envIdentifier_(view, this),
componentInstantiationEditor_(component, view, componentInstantiation, parameterFinder, expressionFormatter, this),
hierarchyReferenceEditor_(view, designInstantiation, designConfigurationInstantiation, libHandler,
                          component->getChoices(), parameterFinder, expressionFormatter, this)
{
	setupLayout();

	connect(&nameEditor_, SIGNAL(contentChanged()),	this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	
    connect(&envIdentifier_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&hierarchyReferenceEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&hierarchyReferenceEditor_, SIGNAL(helpUrlRequested(const QString&)),
        this, SIGNAL(helpUrlRequested(const QString&)), Qt::UniqueConnection);
    connect(&hierarchyReferenceEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&hierarchyReferenceEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&hierarchyReferenceEditor_, SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

    connect(&componentInstantiationEditor_, SIGNAL(contentChanged()),	this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&componentInstantiationEditor_, SIGNAL(helpUrlRequested(const QString&)),
		this, SIGNAL(helpUrlRequested(const QString&)), Qt::UniqueConnection);
    connect(&componentInstantiationEditor_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&componentInstantiationEditor_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&componentInstantiationEditor_, SIGNAL(openReferenceTree(QString)),
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
    return nameEditor_.isValid() || envIdentifier_.isValid() || componentInstantiationEditor_.isValid() ||
        hierarchyReferenceEditor_.isValid();
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::refresh()
//-----------------------------------------------------------------------------
void ViewEditor::refresh()
{
	nameEditor_.refresh();
	envIdentifier_.refresh();

    componentInstantiationEditor_.refresh();

    hierarchyReferenceEditor_.refresh();
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
    editorsLayout->addWidget(&componentInstantiationEditor_, 0);
    editorsLayout->addWidget(&hierarchyReferenceEditor_, 0);

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