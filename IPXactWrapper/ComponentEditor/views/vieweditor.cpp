/* 
 *
 *  Created on: 14.4.2011
 *      Author: Antti Kamppi
 * 		filename: vieweditor.cpp
 */

#include "vieweditor.h"

#include <models/component.h>
#include <models/view.h>
#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/vlnv.h>
#include <models/filebuilder.h>

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
ItemEditor(component, parent), 
libHandler_(libHandler),
component_(component),
view_(view.data()),
nameGroup_(this, tr("View name and description")),
viewTypeSelector_(),
envIdentifier_(view_, this),
stack_(this),
flatElements_(&stack_),
generalTab_(component, view_, &flatElements_),
parametersTab_(view_, this),
fileBuildersTab_(view_->getDefaultFileBuilders(), this),
hierarchyRef_(view_, component_, libHandler, &stack_) {

	initialize();
}

void ViewEditor::initialize() {
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
	flatLayout->addWidget(&fileBuildersTab_);
	flatLayout->addWidget(&parametersTab_);
	flatLayout->setContentsMargins(0, 0, 0, 0);

	setupLayout();

	connect(&nameGroup_, SIGNAL(nameChanged(const QString&)),
		this, SIGNAL(nameChanged(const QString&)), Qt::UniqueConnection);
	connect(&nameGroup_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&envIdentifier_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&hierarchyRef_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&viewTypeSelector_, SIGNAL(currentIndexChanged(int)),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&generalTab_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&fileBuildersTab_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&parametersTab_, SIGNAL(contentChanged()),
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
	if (!nameGroup_.isValid())
		return false;

	// if environment identifiers have not been defined
	else if (!envIdentifier_.isValid())
		return false;
	
	// if view is hierarchical make sure the vlnv ref is valid
	if (viewTypeSelector_.currentIndex() == 0)
		return hierarchyRef_.isValid();

	// if view is not hierarchical make sure all it's elements are valid
	else 
		return generalTab_.isValid() && parametersTab_.isValid() &&
		fileBuildersTab_.isValid();
}

void ViewEditor::setupLayout() {

	// create the scroll area
	QScrollArea* scrollArea = new QScrollArea(this);
	scrollArea->setWidgetResizable(true);

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
	topLayout->addWidget(&nameGroup_);
	topLayout->addWidget(&envIdentifier_, 1);
	topLayout->addLayout(viewTypeLayout);
	topLayout->addWidget(&stack_);

	scrollArea->setWidget(topWidget);
}

void ViewEditor::removeModel() {
	component_->removeView(view_->getName());
	view_ = 0;
}

void ViewEditor::makeChanges() {
	view_->setName(nameGroup_.getName());
	view_->setDisplayName(nameGroup_.getDisplayName());
	view_->setDescription(nameGroup_.getDescription());

	envIdentifier_.applyChanges();

	// if hierarchical view is selected
	if (viewTypeSelector_.currentIndex() == 0) {
		hierarchyRef_.applyChanges();
		return;
	}
	// if view is not hierarchical
	else {
		generalTab_.applyChanges();
		parametersTab_.applyChanges();
	}
}

void ViewEditor::onStackChange( int index ) {
	stack_.setCurrentIndex(index);

	// if the new index is for hierarchical view then refresh the hierarchical editor
	if (index == 0) {
		hierarchyRef_.refresh();
	}
}

void ViewEditor::refresh() {
	nameGroup_.setName(view_->getName());
	nameGroup_.setDisplayName(view_->getDisplayName());
	nameGroup_.setDescription(view_->getDescription());

	envIdentifier_.restoreChanges();	

	// if view is hierarchical then set it to be selected
	if (view_->isHierarchical()) {
		viewTypeSelector_.setCurrentIndex(0);
		hierarchyRef_.restoreChanges();
	}
	else {
		viewTypeSelector_.setCurrentIndex(1);
		generalTab_.restoreChanges();
		parametersTab_.restoreChanges();
	}
}
