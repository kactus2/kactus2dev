/* 
 *  	Created on: 15.1.2013
 *      Author: Antti Kamppi
 * 		filename: memorymapproxy.cpp
 *		Project: Kactus 2
 */

#include "memorymapproxy.h"
#include "memorymapdelegate.h"
#include <common/utils.h>

MemoryMapProxy::MemoryMapProxy(QObject *parent):
QSortFilterProxyModel(parent) {
}

MemoryMapProxy::~MemoryMapProxy() {
}

bool MemoryMapProxy::lessThan( const QModelIndex& left, const QModelIndex& right ) const {

	Q_ASSERT(left.column() == right.column());

	switch (left.column()) {

		case MemoryMapDelegate::BASE_COLUMN:
		case MemoryMapDelegate::RANGE_COLUMN: {
			
			// convert the data on left index into number
			QString leftStr = left.model()->data(left, Qt::DisplayRole).toString();
			quint64 leftNumber = Utils::str2Uint(leftStr);
			
			// convert data on right index into number
			QString rightStr = right.model()->data(right, Qt::DisplayRole).toString();
			quint64 rightNumber = Utils::str2Uint(rightStr);

			// compare the numbers instead of strings to provide correct comparison results
			return leftNumber < rightNumber;
											  }
		default: {
			return QSortFilterProxyModel::lessThan(left, right);
				 }
	}
}
