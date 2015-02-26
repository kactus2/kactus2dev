/* 
 *  	Created on: 15.1.2013
 *      Author: Antti Kamppi
 * 		filename: memorymapproxy.cpp
 *		Project: Kactus 2
 */

#include "memorymapproxy.h"

#include "MemoryMapColumns.h"

#include <common/utils.h>

//-----------------------------------------------------------------------------
// Function: MemoryMapProxy::MemoryMapProxy()
//-----------------------------------------------------------------------------
MemoryMapProxy::MemoryMapProxy(QObject *parent):
QSortFilterProxyModel(parent)
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapProxy::~MemoryMapProxy()
//-----------------------------------------------------------------------------
MemoryMapProxy::~MemoryMapProxy()
{

}

//-----------------------------------------------------------------------------
// Function: MemoryMapProxy::lessThan()
//-----------------------------------------------------------------------------
bool MemoryMapProxy::lessThan( const QModelIndex& left, const QModelIndex& right ) const
{
    Q_ASSERT(left.column() == right.column());

    if (left.column() == MemoryMapColumns::BASE_COLUMN || left.column() == MemoryMapColumns::RANGE_COLUMN)
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
