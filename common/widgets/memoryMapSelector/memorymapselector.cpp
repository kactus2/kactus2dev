/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapselector.cpp
 *		Project: Kactus 2
 */

#include "memorymapselector.h"

MemoryMapSelector::MemoryMapSelector(QSharedPointer<Component> component,
									 QWidget *parent):
QComboBox(parent),
component_(component) {

	Q_ASSERT(component_);

	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

MemoryMapSelector::~MemoryMapSelector() {
}

void MemoryMapSelector::refresh() {
	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));

	// remove the previous items
	clear();

	// add an empty item and list of address spaces
	QStringList memoryMapNames = component_->getMemoryMapNames();
	addItem("");
	addItems(memoryMapNames);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void MemoryMapSelector::selectMemoryMap( const QString& memoryMapName ) {
	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));
	int index = findText(memoryMapName);
	setCurrentIndex(index);
	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void MemoryMapSelector::onIndexChange( int newIndex ) {
	// find the text for the index
	QString text = itemText(newIndex);
	emit memoryMapSelected(text);
}
