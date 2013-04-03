/* 
 *	Created on:	3.4.2013
 *	Author:		Antti Kamppi
 *	File name:	cpuselector.cpp
 *	Project:		Kactus 2
*/

#include "cpuselector.h"

CpuSelector::CpuSelector( QSharedPointer<Component> component, QWidget *parent ):
QComboBox(parent),
component_(component) {

	Q_ASSERT(component_);

	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

CpuSelector::~CpuSelector() {
}

void CpuSelector::refresh() {
	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));

	// remove the previous items
	clear();

	// add an empty item and list of CPU names
	QStringList cpuNames = component_->getCpuNames();
	addItem("");
	addItems(cpuNames);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void CpuSelector::selectCpu( const QString& cpuName ) {
	disconnect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)));
	
	int index = findText(cpuName);

	// if the item is not found
	if (index < 0) {
		addItem(cpuName);
		index = count() - 1;
	}

	setCurrentIndex(index);
	
	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

void CpuSelector::onIndexChange( int newIndex ) {
	// find the text for the index
	QString text = itemText(newIndex);
	emit cpuSelected(text);
}
