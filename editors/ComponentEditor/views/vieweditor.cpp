/* 
 *
 *  Created on: 14.4.2011
 *      Author: Antti Kamppi
 * 		filename: vieweditor.cpp
 */

#include "vieweditor.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/view.h>
#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/filebuilder.h>

#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSizePolicy>
#include <QGroupBox>
#include <QScrollArea>

ViewEditor::ViewEditor( QSharedPointer<Component> component, 
					   QSharedPointer<View> view, 
					   LibraryInterface* libHandler,
					   QWidget *parent ): 
ItemEditor(component, libHandler, parent), 
view_(view),
nameEditor_(view->getNameGroup(), this, tr("View name and description")),
viewTypeSelector_(),
envIdentifier_(view, this),
stack_(this),
flatElements_(&stack_),
generalTab_(component, view, &flatElements_),
parameters_(view->getParameters(), this),
fileBuilders_(view->getDefaultFileBuilders(), this),
hierarchyRef_(view, component, libHandler, &stack_) {

	// set the possible options to the view type selector.
	viewTypeSelector_.addItem(tr("hierarchical"));
	viewTypeSelector_.addItem(tr("non-hierarchical"));

	// add the widget to edit hierarchy reference to index 0 in the stackWidget
	stack_.addWidget(&hierarchyRef_);
	// ans tab widget that contains widgets to edit flat views to index 1
	stack_.addWidget(&flatElements_);

	// create a layout for the flat elements and add the editors
	QVBoxLayout* flatLayout = new QVBoxLayout(&flatElements_);
	flatLayout->addWidget(&generalTab_);
	flatLayout->addWidget(&fileBuilders_);
	flatLayout->addWidget(&parameters_);
	flatLayout->setContentsMargins(0, 0, 0, 0);

	setupLayout();

	connect(&nameEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&envIdentifier_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&hierarchyRef_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&hierarchyRef_, SIGNAL(helpUrlRequested(const QString&)),
		this, SIGNAL(helpUrlRequested(const QString&)), Qt::UniqueConnection);
	connect(&viewTypeSelector_, SIGNAL(currentIndexChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&generalTab_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&generalTab_, SIGNAL(helpUrlRequested(const QString&)),
		this, SIGNAL(helpUrlRequested(const QString&)), Qt::UniqueConnection);
	connect(&fileBuilders_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parameters_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);

	// when user changes the view type the correct editor is displayed
	connect(&viewTypeSelector_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onStackChange(int)), Qt::UniqueConnection);

	refresh();
}

ViewEditor::~ViewEditor() {
}

bool ViewEditor::isValid() const {
	
	// if name group is not valid
	if (!nameEditor_.isValid())
		return false;

	// if environment identifiers have not been defined
	else if (!envIdentifier_.isValid())
		return false;
	
	// if view is hierarchical make sure the vlnv ref is valid
	if (viewTypeSelector_.currentIndex() == 0)
		return hierarchyRef_.isValid();

	// if view is not hierarchical make sure all it's elements are valid
	else {
		return generalTab_.isValid() && parameters_.isValid() &&
		fileBuilders_.isValid();
	}
}

void ViewEditor::setupLayout() {

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

	QFormLayout* viewTypeLayout = new QFormLayout();
	viewTypeLayout->addRow(tr("View type"), &viewTypeSelector_);

	// create the layout for the top widget
	QVBoxLayout* topLayout = new QVBoxLayout(topWidget);
	topLayout->addWidget(&nameEditor_);
	topLayout->addWidget(&envIdentifier_, 1);
	topLayout->addLayout(viewTypeLayout);
	topLayout->addWidget(&stack_);
	topLayout->setContentsMargins(0, 0, 0, 0);

	scrollArea->setWidget(topWidget);
}

void ViewEditor::onStackChange( int index ) {
	stack_.setCurrentIndex(index);

	// if the new index is for hierarchical view then refresh the hierarchical editor
	if (index == 0) {
		hierarchyRef_.refresh();
	}
	// if the flat view is selected then clear the hierarchy ref
	else {
		hierarchyRef_.clear();
		generalTab_.refresh();
		parameters_.refresh();

        // Update order in tree view.
        emit contentChanged();
	}
}

void ViewEditor::refresh() {
	nameEditor_.refresh();
	envIdentifier_.refresh();

	// if view is hierarchical then set it to be selected
	if (view_->isHierarchical()) {
		viewTypeSelector_.setCurrentIndex(0);
		hierarchyRef_.refresh();
	}
	else {
		viewTypeSelector_.setCurrentIndex(1);
		generalTab_.refresh();
		parameters_.refresh();
	}
}
