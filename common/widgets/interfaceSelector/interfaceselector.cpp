/* 
 *  	Created on: 26.6.2012
 *      Author: Antti Kamppi
 * 		filename: interfaceselector.cpp
 *		Project: Kactus 2
 */

#include "interfaceselector.h"

InterfaceSelector::InterfaceSelector(QSharedPointer<Component> component,
									 QWidget *parent,
									 General::InterfaceMode mode /*= General::INTERFACE_MODE_COUNT*/):
QComboBox(parent),
mode_(mode),
component_(component) {

	Q_ASSERT(component_);

	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

InterfaceSelector::~InterfaceSelector() {
}

void InterfaceSelector::setInterfaceMode( General::InterfaceMode mode ) {
	mode_ = mode;
	refresh();
}

void InterfaceSelector::refresh() {

	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));

	// remove the previous items
	clear();

	// add an empty item and list of interface names
	QStringList interfaceNames = component_->getBusInterfaceNames();

	// the list to add to combo box
	QStringList list;

	// if mode is specified then check that the modes match
	if (mode_ != General::INTERFACE_MODE_COUNT) {

		// check that each name matches the mode
		foreach (QString name, interfaceNames) {
			
			// if theres a match then add the name to the list
			if (component_->getInterfaceMode(name) == mode_) {
				list.append(name);
			}
		}
	}
	// if mode is unspecified then add all
	else {
		list = interfaceNames;
	}

	addItems(list);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void InterfaceSelector::selectInterface( const QString& interfaceName ) {
	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));
	int index = findText(interfaceName);
	setCurrentIndex(index);
	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void InterfaceSelector::onIndexChange( int newIndex ) {
	// find the text for the index
	QString text = itemText(newIndex);
	emit interfaceSelected(text);
}
