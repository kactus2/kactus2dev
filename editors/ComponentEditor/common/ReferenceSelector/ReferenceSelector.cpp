/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapselector.cpp
 *		Project: Kactus 2
 */

#include "ReferenceSelector.h"

#include <QPalette>
#include <QEvent>

//-----------------------------------------------------------------------------
// Function: ReferenceSelector::ReferenceSelector()
//-----------------------------------------------------------------------------
ReferenceSelector::ReferenceSelector(QWidget *parent):
QComboBox(parent)
{
	setEditable(false);

	connect(this, SIGNAL(currentIndexChanged(int)),	this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ReferenceSelector::~ReferenceSelector()
//-----------------------------------------------------------------------------
ReferenceSelector::~ReferenceSelector()
{

}

//-----------------------------------------------------------------------------
// Function: ReferenceSelector::refresh()
//-----------------------------------------------------------------------------
void ReferenceSelector::refresh(QStringList const& itemNames)
{
	disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)));

	// Remove the previous items
	clear();

	// Add an empty item and list of memory maps.
	addItem("");
	addItems(itemNames);

	connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ReferenceSelector::selectMemoryMap()
//-----------------------------------------------------------------------------
void ReferenceSelector::selectItem(QString const& itemName)
{
	disconnect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onIndexChange(int)));

	int index = findText(itemName);
    if (!itemName.isEmpty() && index == -1)
    {
        setTextColor(Qt::red);

        addItem(itemName);
        index = findText(itemName);

        setItemData(index, QColor(Qt::red), Qt::TextColorRole);
        setItemIcon(index, QIcon(QPixmap(":/icons/common/graphics/exclamation.png")));
    }
    else if (!isEnabled())
    {
        setTextColor(Qt::gray);
    }
    else
    {
        setTextColor(Qt::black);
    }

    setCurrentIndex(index);

	connect(this, SIGNAL(currentIndexChanged(int)),	this, SLOT(onIndexChange(int)), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: ReferenceSelector::onIndexChange()
//-----------------------------------------------------------------------------
void ReferenceSelector::onIndexChange(int newIndex)
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
	emit itemSelected(text);
}

//-----------------------------------------------------------------------------
// Function: ReferenceSelector::setTextColor()
//-----------------------------------------------------------------------------
void ReferenceSelector::setTextColor(QColor const& color)
{
    QPalette coloredPalette = palette();
    coloredPalette.setColor(QPalette::Text, color);
    setPalette(coloredPalette);
}

//-----------------------------------------------------------------------------
// Function: ReferenceSelector::changeEvent()
//-----------------------------------------------------------------------------
void ReferenceSelector::changeEvent(QEvent *currentEvent)
{
    QComboBox::changeEvent(currentEvent);

    if (currentEvent->type() == QEvent::EnabledChange)
    {
        QPalette usedPalette = palette();
        if (usedPalette.color(QPalette::Text) != Qt::red)
        {
            if (isEnabled())
            {
                usedPalette.setColor(QPalette::Text, Qt::black);
            }
            else
            {
                usedPalette.setColor(QPalette::Text, Qt::gray);
            }
        }

        setPalette(usedPalette);
    }
}