//-----------------------------------------------------------------------------
// File: AddressSubsection.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.9.2012
//
// Description:
// Implements the address subsection class.
//-----------------------------------------------------------------------------

#include "AddressSubsection.h"

#include "AddressSectionItem.h"

#include <common/utils.h>

#include <QFont>
#include <QTextDocument>

//-----------------------------------------------------------------------------
// Function: AddressSubsection::AddressSubsection()
//-----------------------------------------------------------------------------
AddressSubsection::AddressSubsection(AddressSectionItem* parent, int offsetX,
                                     int top, int bottom, unsigned int startAddress, unsigned int endAddress)
    : top_(top),
      bottom_(bottom),
      startAddress_(startAddress),
      endAddress_(endAddress),
      startAddressLabel_(new QGraphicsTextItem(toHexString(startAddress), parent)),
      endAddressLabel_(new QGraphicsTextItem(toHexString(endAddress), parent))
{
    QFont font = startAddressLabel_->font();
    font.setWeight(QFont::Bold);

    startAddressLabel_->setFont(font);
    startAddressLabel_->setAcceptHoverEvents(false);
    startAddressLabel_->setPos(QPointF(offsetX, top_));
    startAddressLabel_->setHtml(toHexString(startAddress_));

    endAddressLabel_->setFont(font);
    endAddressLabel_->setAcceptHoverEvents(false);
    endAddressLabel_->setPos(QPointF(offsetX, bottom_ - 20));
    endAddressLabel_->setHtml(toHexString(endAddress_));    

    setStartAddressFixed(false);

    connect(startAddressLabel_->document(), SIGNAL(contentsChanged()), this, SLOT(onStartAddressEdited()));
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::~AddressSubsection()
//-----------------------------------------------------------------------------
AddressSubsection::~AddressSubsection()
{

}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::setTop()
//-----------------------------------------------------------------------------
void AddressSubsection::setTop(int top)
{
    top_ = top;
    startAddressLabel_->setY(top_);
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::setBottom()
//-----------------------------------------------------------------------------
void AddressSubsection::setBottom(int bottom)
{
    bottom_ = bottom;
    endAddressLabel_->setY(bottom_ - 20);
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::setStartAddress()
//-----------------------------------------------------------------------------
void AddressSubsection::setStartAddress(unsigned int startAddress)
{
    startAddress_ = startAddress;
    startAddressLabel_->setHtml(toHexString(startAddress));
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::setEndAddress()
//-----------------------------------------------------------------------------
void AddressSubsection::setEndAddress(unsigned int endAddress)
{
    endAddress_ = endAddress;
    endAddressLabel_->setHtml(toHexString(endAddress));
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::setStartAddressFixed()
//-----------------------------------------------------------------------------
void AddressSubsection::setStartAddressFixed(bool fixed)
{
    if (fixed)
    {
        startAddressLabel_->setTextInteractionFlags(Qt::NoTextInteraction);
    }
    else
    {
        startAddressLabel_->setTextInteractionFlags(Qt::TextEditable);
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::getTop()
//-----------------------------------------------------------------------------
int AddressSubsection::getTop() const
{
    return top_;
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::getBottom()
//-----------------------------------------------------------------------------
int AddressSubsection::getBottom() const
{
    return bottom_;
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::getStartAddress()
//-----------------------------------------------------------------------------
unsigned int AddressSubsection::getStartAddress() const
{
    return startAddress_;
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::getEndAddress()
//-----------------------------------------------------------------------------
unsigned int AddressSubsection::getEndAddress() const
{
    return endAddress_;
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::getRange()
//-----------------------------------------------------------------------------
unsigned int AddressSubsection::getRange() const
{
    return (endAddress_ - startAddress_ + 1);
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::onStartAddressEdited()
//-----------------------------------------------------------------------------
void AddressSubsection::onStartAddressEdited()
{
    startAddress_ = Utils::str2Int(startAddressLabel_->toPlainText());
    emit startAddressEdited(this);
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::toHexString()
//-----------------------------------------------------------------------------
QString AddressSubsection::toHexString(unsigned int address)
{
    return QString("0x") + QString("%1").arg(address, 8, 16, QChar('0')).toUpper();
}