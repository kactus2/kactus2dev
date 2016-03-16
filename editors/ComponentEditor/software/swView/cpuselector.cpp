//-----------------------------------------------------------------------------
// File: cpuselector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 03.04.2013
//
// Description:
// Combo box to select one of the CPU elements of a component.
//-----------------------------------------------------------------------------

#include "cpuselector.h"

#include <IPXACTmodels/Component/Component.h>

//-----------------------------------------------------------------------------
// Function: CpuSelector::CpuSelector()
//-----------------------------------------------------------------------------
CpuSelector::CpuSelector(QSharedPointer<Component> component, QWidget *parent):
QComboBox(parent),
    component_(component)
{
    Q_ASSERT(component_);

	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: CpuSelector::~CpuSelector()
//-----------------------------------------------------------------------------
CpuSelector::~CpuSelector()
{
}

//-----------------------------------------------------------------------------
// Function: CpuSelector::refresh()
//-----------------------------------------------------------------------------
void CpuSelector::refresh()
{
	disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)));

	// remove the previous items
	clear();

	// add an empty item and list of CPU names
	QStringList cpuNames = component_->getCpuNames();
	addItem("");
	addItems(cpuNames);

	connect(this, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: CpuSelector::selectCpu()
//-----------------------------------------------------------------------------
void CpuSelector::selectCpu(QString const& cpuName)
{
	disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)));
	
	int index = findText(cpuName);

	// if the item is not found
	if (index < 0)
    {
		addItem(cpuName);
		index = count() - 1;
	}

	setCurrentIndex(index);
	
	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: CpuSelector::onIndexChange()
//-----------------------------------------------------------------------------
void CpuSelector::onIndexChange(int newIndex)
{
	emit cpuSelected(itemText(newIndex));
}
