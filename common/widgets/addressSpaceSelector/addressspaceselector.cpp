/* 
 *  	Created on: 19.6.2012
 *      Author: Antti Kamppi
 * 		filename: addressspaceselector.cpp
 *		Project: Kactus 2
 */

#include "addressspaceselector.h"

AddressSpaceSelector::AddressSpaceSelector(QSharedPointer<Component> component,
										   QWidget *parent):
QComboBox(parent),
component_(component) {

	Q_ASSERT(component_);

	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

AddressSpaceSelector::~AddressSpaceSelector() {
}

void AddressSpaceSelector::refresh() {

	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));

	// remove the previous items
	clear();

	// add an empty item and list of address spaces
	QStringList addrSpaceNames = component_->getAddressSpaceNames();
	addItem("");
	addItems(addrSpaceNames);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void AddressSpaceSelector::selectAddressSpace( const QString& addrSpaceName ) {
	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));
	int index = findText(addrSpaceName);
	setCurrentIndex(index);
	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void AddressSpaceSelector::onIndexChange(int newIndex) {
	// find the text for the index
	QString text = itemText(newIndex);
	emit addressSpaceSelected(text);
}
