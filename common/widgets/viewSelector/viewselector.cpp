/* 
 *	Created on:	28.3.2013
 *	Author:		Antti Kamppi
 *	File name:	viewselector.cpp
 *	Project:		Kactus 2
*/

#include "viewselector.h"

ViewSelector::ViewSelector( ViewType mode, QSharedPointer<Component> component, QWidget *parent ):
QComboBox(parent),
mode_(mode),
component_(component) {

	Q_ASSERT(component_);

	setFixedWidth(ViewSelector::WIDTH);

	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

ViewSelector::~ViewSelector() {
}

void ViewSelector::refresh() {
	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));

	// remove the previous items
	clear();

	QStringList viewNames;

	// the mode specifes the available view types
	switch (mode_) {
	case ViewSelector::FLAT_HW_VIEWS: {
		viewNames = component_->getFlatViews();
		break;
												 }
	case ViewSelector::HIER_HW_VIEWS: {
		viewNames = component_->getHierViews();
		break;
												 }
	case ViewSelector::BOTH_HW_VIEWS: {
		viewNames = component_->getFlatViews();
		viewNames += component_->getHierViews();
		break;
												 }
	case ViewSelector::SW_VIEWS: {
		viewNames = component_->getSWViewNames();
		break;
										  }
	case ViewSelector::SYSTEM_VIEWS: {
		viewNames = component_->getSystemViewNames();
		break;
												}
	default: {
		break;
				}
	}	
	
	addItem("");
	addItems(viewNames);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void ViewSelector::selectView( const QString& viewName ) {
	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));
	int index = findText(viewName);
	
	// if the item is not found
	if (index < 0) {
		addItem(viewName);
		index = count() - 1;
	}

	setCurrentIndex(index);
	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void ViewSelector::onIndexChange( int newIndex ) {
	// find the text for the index
	QString text = itemText(newIndex);
	emit viewSelected(text);
}
