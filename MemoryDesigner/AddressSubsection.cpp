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
#include <common/DesignDiagram.h>
#include <common/DesignWidget.h>
#include <common/graphicsItems/GraphicsLineEdit.h>
#include <common/validators/AddressValidator.h>

#include <QFont>
#include <QTextDocument>
#include <QMessageBox>
#include <QCoreApplication>

//-----------------------------------------------------------------------------
// Function: AddressSubsection::AddressSubsection()
//-----------------------------------------------------------------------------
AddressSubsection::AddressSubsection(AddressSectionItem* parent, int offsetX,
                                     int top, int bottom, unsigned int startAddress, unsigned int endAddress)
    : top_(top),
      bottom_(bottom),
      startAddress_(startAddress),
      endAddress_(endAddress),
      startAddressLabel_(new GraphicsLineEdit(parent)),
      startAddressValidator_(new AddressValidator(this)),
      endAddressLabel_(new QGraphicsTextItem(parent))
{
    QFont font = startAddressLabel_->font();
    font.setWeight(QFont::Bold);

    startAddressLabel_->setFont(font);
    startAddressLabel_->setAcceptHoverEvents(false);
    startAddressLabel_->setPos(QPointF(offsetX, top_));
    startAddressLabel_->setHtml(toHexString(startAddress_));
    startAddressLabel_->setValidator(startAddressValidator_);

    startAddressValidator_->setMaxAddress(endAddress);

    endAddressLabel_->setFont(font);
    endAddressLabel_->setAcceptHoverEvents(false);
    endAddressLabel_->setPos(QPointF(offsetX, bottom_ - 20));
    endAddressLabel_->setHtml(toHexString(endAddress_));

    setStartAddressFixed(false);

    connect(startAddressLabel_, SIGNAL(textEdited()), this, SLOT(onStartAddressEdited()));
    connect(startAddressLabel_, SIGNAL(invalidInputEntered()), this, SLOT(onInvalidInput()));
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
    startAddressValidator_->setMaxAddress(endAddress);
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::setStartAddressFixed()
//-----------------------------------------------------------------------------
void AddressSubsection::setStartAddressFixed(bool fixed)
{
    startAddressLabel_->setReadOnly(fixed);
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

//-----------------------------------------------------------------------------
// Function: AddressSubsection::onInvalidInput()
//-----------------------------------------------------------------------------
void AddressSubsection::onInvalidInput()
{
    QMessageBox msgBox(QMessageBox::Warning, QCoreApplication::applicationName(),
                       tr("Invalid address."),
                       QMessageBox::Ok, static_cast<DesignDiagram*>(startAddressLabel_->scene())->getParent());
    msgBox.exec();
}

//-----------------------------------------------------------------------------
// Function: AddressSubsection::setMinStartAddress()
//-----------------------------------------------------------------------------
void AddressSubsection::setMinStartAddress(unsigned int minAddress)
{
    startAddressValidator_->setMinAddress(minAddress);
}
