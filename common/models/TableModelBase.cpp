//-----------------------------------------------------------------------------
// File: TableModelBase.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 30.03.2026
//
// Description:
// Base class for various table models found in Kactus2. Handles common functionality
// of table models.
//-----------------------------------------------------------------------------

#include "TableModelBase.h"

#include <common/KactusUtils.h>

#include <KactusAPI/include/KactusColors.h>

TableModelBase::TableModelBase(QObject* parent) :
    QAbstractTableModel(parent)
{

}

QVariant TableModelBase::data(QModelIndex const& index, int role) const
{
    if (role == Qt::BackgroundRole)
    {
        if (indexIsMandatory(index))
        {
            return KactusColors::MANDATORY_FIELD;
        }
        else if (index.flags() == (Qt::ItemIsSelectable | Qt::ItemIsEnabled))
        {
            return KactusColors::DISABLED_FIELD;
        }

        return KactusColors::REGULAR_FIELD;
    }
    else if (role == Qt::ForegroundRole)
    {
        return blackForValidOrRedForInvalidIndex(index);
    }

    // The role checks here can and should be overridden in inheriting classes as well.

    return QVariant();
}

QVariant TableModelBase::blackForValidOrRedForInvalidIndex(QModelIndex const& index) const
{
    if (validateIndex(index))
    {
        if (indexIsGreyedOut(index))
        {
            return KactusColors::DISABLED_TEXT;
        }

        // Return black text for light mode or if index is mandatory (always light background)
        if (KactusUtils::darkThemeEnabled() == false || indexIsMandatory(index))
        {
            return QColor(Qt::black);
        }

        // Otherwise return defined color
        return KactusColors::REGULAR_TEXT;
    }
    else
    {
        return KactusColors::ERROR_COLOR;
    }
}

bool TableModelBase::indexIsMandatory(QModelIndex const& index) const
{
    // default implementation
    return false;
}

bool TableModelBase::indexIsGreyedOut(QModelIndex const& index) const
{
    // default implementation
    return false;
}
