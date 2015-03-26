/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapselector.cpp
 *		Project: Kactus 2
 */

#include "memorymapselector.h"

#include <QPalette>

//-----------------------------------------------------------------------------
// Function: MemoryMapSelector::MemoryMapSelector()
//-----------------------------------------------------------------------------
MemoryMapSelector::MemoryMapSelector(QWidget *parent):
QComboBox(parent)
{
	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)),	this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapSelector::~MemoryMapSelector()
//-----------------------------------------------------------------------------
MemoryMapSelector::~MemoryMapSelector()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapSelector::refresh()
//-----------------------------------------------------------------------------
void MemoryMapSelector::refresh(QStringList const& memoryMapNames)
{
	disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)));

	// Remove the previous items
	clear();

	// Add an empty item and list of memory maps.
	addItem("");
	addItems(memoryMapNames);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapSelector::selectMemoryMap()
//-----------------------------------------------------------------------------
void MemoryMapSelector::selectMemoryMap(QString const& memoryMapName)
{
	disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)));

	int index = findText(memoryMapName);
    if (!memoryMapName.isEmpty() && index == -1)
    {
        setTextColor(Qt::red);

        addItem(memoryMapName);
        index = findText(memoryMapName);

        setItemData(index, QColor(Qt::red), Qt::TextColorRole);
        setItemIcon(index, QIcon(QPixmap(":/icons/common/graphics/exclamation.png")));
    }
    else
    {
        setTextColor(Qt::black);
    }

    setCurrentIndex(index);

	connect(this, SIGNAL(currentIndexChanged(int)),	this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapSelector::onIndexChange()
//-----------------------------------------------------------------------------
void MemoryMapSelector::onIndexChange(int newIndex)
{
    if (itemIcon(newIndex).isNull())
    {
        setTextColor(Qt::black);
    }
    else
    {
        setTextColor(Qt::red);
    }

	// find the text for the index
	QString text = itemText(newIndex);
	emit memoryMapSelected(text);
}

//-----------------------------------------------------------------------------
// Function: MemoryMapSelector::setTextColor()
//-----------------------------------------------------------------------------
void MemoryMapSelector::setTextColor(QColor const& color)
{
    QPalette coloredPalette = palette();
    coloredPalette.setColor(QPalette::Text, color);
    setPalette(coloredPalette);
}
