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

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/view.h>
#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/filebuilder.h>

#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QGroupBox>
#include <QScrollArea>

namespace
{
    //! Enumeration of view types.
    enum ViewType
    {
        HIERARCHICAL = 0,
        FLAT
    };
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::ViewEditor()
//-----------------------------------------------------------------------------
ViewEditor::ViewEditor( QSharedPointer<Component> component, QSharedPointer<View> view, 
    LibraryInterface* libHandler, QSharedPointer<ParameterFinder> parameterFinder, 
    QSharedPointer<ExpressionFormatter> expressionFormatter, QWidget *parent ): 
ItemEditor(component, libHandler, parent), 
view_(view),
nameEditor_(view->getNameGroup(), this, tr("View name and description")),
viewTypeSelector_(),
envIdentifier_(view, this),
typeDependentEditors_(this),
generalTab_(component, view, &typeDependentEditors_),
moduleParameters_(view->getModuleParameters(), component, parameterFinder, expressionFormatter, this),
parameters_(view->getParameters(), component, parameterFinder, expressionFormatter, this),
fileBuilders_(view->getDefaultFileBuilders(), this),
hierarchyRef_(view, component, libHandler, &typeDependentEditors_)
{
    moduleParameters_.setTitle(tr("Module parameters"));
    parameters_.setTitle(tr("View-specific generator parameters"));

	// set the possible options to the view type selector.
	viewTypeSelector_.insertItem(HIERARCHICAL, tr("hierarchical"));
	viewTypeSelector_.insertItem(FLAT, tr("non-hierarchical"));

	// Add the widget to edit hierarchy reference to index 0 in the stackWidget.
	typeDependentEditors_.insertWidget(HIERARCHICAL, &hierarchyRef_);
	// Add tab widget that contains widgets to edit flat views to index 1.
	typeDependentEditors_.insertWidget(FLAT, &generalTab_);

	setupLayout();

	connect(&nameEditor_, SIGNAL(contentChanged()),	this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	
    connect(&envIdentifier_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	
    connect(&hierarchyRef_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&hierarchyRef_, SIGNAL(helpUrlRequested(const QString&)),
		this, SIGNAL(helpUrlRequested(const QString&)), Qt::UniqueConnection);
	
    connect(&generalTab_, SIGNAL(contentChanged()),	this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&generalTab_, SIGNAL(helpUrlRequested(const QString&)),
		this, SIGNAL(helpUrlRequested(const QString&)), Qt::UniqueConnection);
	
    connect(&fileBuilders_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);

    connect(&parameters_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&parameters_, SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

    connect(&moduleParameters_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
    connect(&moduleParameters_, SIGNAL(increaseReferences(QString)),
        this, SIGNAL(increaseReferences(QString)), Qt::UniqueConnection);
    connect(&moduleParameters_, SIGNAL(decreaseReferences(QString)),
        this, SIGNAL(decreaseReferences(QString)), Qt::UniqueConnection);
    connect(&moduleParameters_, SIGNAL(openReferenceTree(QString)),
        this, SIGNAL(openReferenceTree(QString)), Qt::UniqueConnection);

	// when user changes the view type the correct editor is displayed
    connect(&viewTypeSelector_, SIGNAL(currentIndexChanged(int)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&viewTypeSelector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onViewTypeChanged(int)), Qt::UniqueConnection);

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
	if (!nameEditor_.isValid() || !envIdentifier_.isValid())
    {
		return false;
    }

	// if view is hierarchical make sure the vlnv ref is valid
	if (viewTypeSelector_.currentIndex() == HIERARCHICAL)
    {
		return hierarchyRef_.isValid();
    }
	// if view is not hierarchical make sure all it's elements are valid
	else
    {
		return generalTab_.isValid() && parameters_.isValid() && moduleParameters_.isValid() && fileBuilders_.isValid();
	}
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::setupLayout()
//-----------------------------------------------------------------------------
void ViewEditor::setupLayout()
{
    envIdentifier_.setMaximumHeight(175);
    typeDependentEditors_.setMaximumHeight(200);
    fileBuilders_.setMaximumHeight(200);

	// create the scroll area
	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);
	scrollArea->setFrameShape(QFrame::NoFrame);

	QHBoxLayout* scrollLayout = new QHBoxLayout(this);
	scrollLayout->addWidget(scrollArea);
	scrollLayout->setContentsMargins(0, 0, 0, 0);

	// create the top widget and set it under the scroll area
	QWidget* topWidget = new QWidget(scrollArea);
	topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	// create the layout for the top widget
	QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);

    QFormLayout* viewTypeLayout = new QFormLayout();
    viewTypeLayout->addRow(tr("View type"), &viewTypeSelector_);

    QVBoxLayout* nameAndTypeLayout = new QVBoxLayout();
    nameAndTypeLayout->addWidget(&nameEditor_, 0);
    nameAndTypeLayout->addLayout(viewTypeLayout);

    QHBoxLayout* nameAndEnvIdentifiersLayout = new QHBoxLayout();
    nameAndEnvIdentifiersLayout->addLayout(nameAndTypeLayout);
    nameAndEnvIdentifiersLayout->addWidget(&envIdentifier_, 0, Qt::AlignTop);

    topLayout->addLayout(nameAndEnvIdentifiersLayout);

    QHBoxLayout* adjustingLayout = new QHBoxLayout();
    adjustingLayout->addWidget(&typeDependentEditors_);
    adjustingLayout->addWidget(&fileBuilders_);

    QVBoxLayout* typeDependentLayout = new QVBoxLayout();
    typeDependentLayout->addLayout(adjustingLayout);
    typeDependentLayout->addWidget(&moduleParameters_, 1);
    typeDependentLayout->addWidget(&parameters_);

    topLayout->addLayout(typeDependentLayout);
    topLayout->addStretch(1);

	scrollArea->setWidget(topWidget);
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::onStackChange()
//-----------------------------------------------------------------------------
void ViewEditor::onViewTypeChanged(int index)
{
	typeDependentEditors_.setCurrentIndex(index);

    fileBuilders_.setVisible(index == FLAT);
    moduleParameters_.setVisible(index == FLAT);
    parameters_.setVisible(index == FLAT);

	// if the new index is for hierarchical view then refresh the hierarchical editor
	if (index == HIERARCHICAL) 
    {
		hierarchyRef_.refresh();
	}
	// if the flat view is selected then clear the hierarchy ref
	else 
    {
		hierarchyRef_.clear();
        generalTab_.refresh();
		parameters_.refresh();

        // Update order in tree view.
        emit contentChanged();
	}
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::refresh()
//-----------------------------------------------------------------------------
void ViewEditor::refresh()
{
	nameEditor_.refresh();
	envIdentifier_.refresh();

	// if view is hierarchical then set it to be selected
	if (view_->isHierarchical())
    {
        fileBuilders_.hide();
        moduleParameters_.hide();
        parameters_.hide();

		viewTypeSelector_.setCurrentIndex(HIERARCHICAL);
		hierarchyRef_.refresh();
	}
	else
    {
        fileBuilders_.show();
        moduleParameters_.show();
        parameters_.show();

		viewTypeSelector_.setCurrentIndex(FLAT);
		generalTab_.refresh();
		parameters_.refresh();
	}
}
