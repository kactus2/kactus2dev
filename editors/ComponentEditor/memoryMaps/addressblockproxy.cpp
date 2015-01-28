/* 
 *  	Created on: 16.1.2013
 *      Author: Antti Kamppi
 * 		filename: addressblockproxy.cpp
 *		Project: Kactus 2
 */

#include "addressblockproxy.h"

#include "AddressBlockColumns.h"

#include <common/utils.h>

//-----------------------------------------------------------------------------
// Function: AddressBlockProxy::AddressBlockProxy()
//-----------------------------------------------------------------------------
AddressBlockProxy::AddressBlockProxy(QObject *parent):
QSortFilterProxyModel(parent)
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockProxy::~AddressBlockProxy()
//-----------------------------------------------------------------------------
AddressBlockProxy::~AddressBlockProxy()
{

}

//-----------------------------------------------------------------------------
// Function: AddressBlockProxy::lessThan()
//-----------------------------------------------------------------------------
bool AddressBlockProxy::lessThan(QModelIndex const& left, QModelIndex const& right ) const
{
    Q_ASSERT(left.column() == right.column());

    if (left.column() == AddressBlockColumns::OFFSET_COLUMN ||
        left.column() == AddressBlockColumns::RESET_VALUE_COLUMN ||
        left.column() == AddressBlockColumns::RESET_MASK_COLUMN)
    {
        // convert the data on left index into number
        QString leftStr = left.model()->data(left, Qt::DisplayRole).toString();
        quint64 leftNumber = Utils::str2Uint(leftStr);

        // convert data on right index into number
        QString rightStr = right.model()->data(right, Qt::DisplayRole).toString();
        quint64 rightNumber = Utils::str2Uint(rightStr);

        // compare the numbers instead of strings to provide correct comparison results
        return leftNumber < rightNumber;
    }
    else
    {
        return QSortFilterProxyModel::lessThan(left, right);
    }
}
