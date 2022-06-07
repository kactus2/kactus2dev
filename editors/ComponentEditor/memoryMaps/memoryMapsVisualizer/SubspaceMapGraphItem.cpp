//-----------------------------------------------------------------------------
// File: SubspaceMapGraphItem.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.03.2022
//
// Description:
// The graphical item that represents one subspace map.
//-----------------------------------------------------------------------------

#include "SubspaceMapGraphItem.h"

#include <KactusAPI/include/ExpressionParser.h>

#include <common/KactusColors.h>

#include <QStringBuilder>
#include <QBrush>

//-----------------------------------------------------------------------------
// Function: AddressBlockGraphItem::AddressBlockGraphItem()
//-----------------------------------------------------------------------------
SubspaceMapGraphItem::SubspaceMapGraphItem(SubspaceMapInterface* submapInterface, QString const& subspaceName,
    QSharedPointer<ExpressionParser> expressionParser, QGraphicsItem *parent):
MemoryBlockGraphItem(expressionParser, parent),
submapInterface_(submapInterface),
subspaceName_(subspaceName.toStdString())
{
    setDefaultBrush(QBrush(KactusColors::MIRROREDSYSTEM_INTERFACE));
    updateDisplay();
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapGraphItem::getOffset()
//-----------------------------------------------------------------------------
quint64 SubspaceMapGraphItem::getOffset() const
{
    quint64 offset = QString::fromStdString(submapInterface_->getBaseAddressValue(subspaceName_, 10)).toULongLong();
    return offset;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapGraphItem::getBitWidth()
//-----------------------------------------------------------------------------
int SubspaceMapGraphItem::getBitWidth() const
{
    quint64 bitWidth = QString::fromStdString(submapInterface_->getWidthValue(subspaceName_, 10)).toULongLong();
    return bitWidth;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapGraphItem::isPresent()
//-----------------------------------------------------------------------------
bool SubspaceMapGraphItem::isPresent() const
{
    QString isPresentValue = QString::fromStdString(submapInterface_->getIsPresentValue(subspaceName_));

    return isPresentValue.isEmpty() || isPresentValue.toULongLong() == 1;
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapGraphItem::onNameChanged()
//-----------------------------------------------------------------------------
void SubspaceMapGraphItem::onNameChanged(QString const& oldName, QString const& newName)
{
    if (subspaceName_ == oldName.toStdString())
    {
        subspaceName_ = newName.toStdString();
        updateDisplay();
    }
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapGraphItem::getName()
//-----------------------------------------------------------------------------
QString SubspaceMapGraphItem::getName() const
{
    return QString::fromStdString(subspaceName_);
}

//-----------------------------------------------------------------------------
// Function: SubspaceMapGraphItem::getRange()
//-----------------------------------------------------------------------------
quint64 SubspaceMapGraphItem::getRange() const
{
    quint64 range = QString::fromStdString(submapInterface_->getRangeValue(subspaceName_)).toULongLong();
    return range;
}
