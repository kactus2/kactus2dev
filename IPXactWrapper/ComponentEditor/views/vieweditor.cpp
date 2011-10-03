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

ViewEditor::ViewEditor(QSharedPointer<Component> component, 
					   void* dataPointer,
					   LibraryInterface* libHandler,
					   QWidget *parent): 
ItemEditor(component, parent), 
libHandler_(libHandler),
component_(component),
view_(static_cast<View*>(dataPointer)),
nameGroup_(this, tr("View name and description")),
viewTypeSelector_(),
envIdentifier_(view_, this),
stack_(this),
flatTabs_(&stack_),
generalTab_(component, view_, &flatTabs_),
parametersTab_(view_, this),
fileBuildersTab_(&view_->getDefaultFileBuilders(), this),
hierarchyRef_(view_, component_, libHandler, &stack_) {

	Q_ASSERT_X(dataPointer, "ViewEditor constructor",
		"Null data pointer given as parameter");

	// set the possible options to the view type selector.
	viewTypeSelector_.addItem(tr("hierarchical"));
	viewTypeSelector_.addItem(tr("non-hierarchical"));

	// add the widget to edit hierarchy reference to index 0 in the stackWidget
	stack_.addWidget(&hierarchyRef_);
	// ans tab widget that contains widgets to edit flat views to index 1
	stack_.addWidget(&flatTabs_);

	// set up the tab widget for flat tabs
	flatTabs_.addTab(&generalTab_, tr("General"));
	flatTabs_.addTab(&fileBuildersTab_, tr("Default file builders"));
	flatTabs_.addTab(&parametersTab_, tr("Parameters"));

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
		&stack_, SLOT(setCurrentIndex(int)), Qt::UniqueConnection);

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
		fileBuildersTab_.restoreChanges();
	}
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

	QFormLayout* viewTypeLayout = new QFormLayout();
	viewTypeLayout->addRow(tr("View type"), &viewTypeSelector_);

	QVBoxLayout* topLayout = new QVBoxLayout(this);
	topLayout->addWidget(&nameGroup_);
	topLayout->addWidget(&envIdentifier_, 1);
	topLayout->addLayout(viewTypeLayout);
	topLayout->addWidget(&stack_);
}

void ViewEditor::removeModel() {
	component_->removeView(view_->getName());
	view_ = 0;
}

void ViewEditor::makeChanges() {
	// if not valid do nothing
	if (!isValid())
		return;

	view_->setName(nameGroup_.getName());
	view_->setDisplayName(nameGroup_.getDisplayName());
	view_->setDescription(nameGroup_.getDescription());

	envIdentifier_.applyChanges();

	// if hierarchical view is selected
	if (viewTypeSelector_.currentIndex() == 0) {
		hierarchyRef_.applyChanges();
		return;
	}
	// if view is not hierarhical
	else {
		generalTab_.applyChanges();
		parametersTab_.applyChanges();
		fileBuildersTab_.applyChanges();
	}
}
