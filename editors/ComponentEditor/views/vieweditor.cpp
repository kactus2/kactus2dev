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
parameters_(view->getParameters(), component, parameterFinder, expressionFormatter, this),
fileBuilders_(view->getDefaultFileBuilders(), this),
hierarchyRef_(view, component, libHandler, &typeDependentEditors_)
{
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


	// when user changes the view type the correct editor is displayed
    connect(&viewTypeSelector_, SIGNAL(currentIndexChanged(int)),
        this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&viewTypeSelector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onStackChange(int)), Qt::UniqueConnection);

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
		return generalTab_.isValid() && parameters_.isValid() && fileBuilders_.isValid();
	}
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::setupLayout()
//-----------------------------------------------------------------------------
void ViewEditor::setupLayout()
{
    nameEditor_.setMaximumHeight(200);
    envIdentifier_.setMaximumHeight(nameEditor_.height() + viewTypeSelector_.height());

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

    QHBoxLayout* typeDependentLayout = new QHBoxLayout();
    typeDependentLayout->addWidget(&typeDependentEditors_);
    typeDependentLayout->addWidget(&fileBuilders_);

    topLayout->addLayout(typeDependentLayout);
    topLayout->addWidget(&parameters_);
    topLayout->addStretch(1);

	scrollArea->setWidget(topWidget);
}

//-----------------------------------------------------------------------------
// Function: ViewEditor::onStackChange()
//-----------------------------------------------------------------------------
void ViewEditor::onStackChange(int index)
{
	typeDependentEditors_.setCurrentIndex(index);

    fileBuilders_.setVisible(index == FLAT);
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
		viewTypeSelector_.setCurrentIndex(HIERARCHICAL);
		hierarchyRef_.refresh();
	}
	else
    {
		viewTypeSelector_.setCurrentIndex(FLAT);
		generalTab_.refresh();
		parameters_.refresh();
	}
}
