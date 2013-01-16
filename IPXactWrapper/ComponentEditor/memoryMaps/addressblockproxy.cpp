/* 
 *  	Created on: 16.1.2013
 *      Author: Antti Kamppi
 * 		filename: addressblockproxy.cpp
 *		Project: Kactus 2
 */

#include "addressblockproxy.h"
#include "addressblockdelegate.h"
#include <common/utils.h>

AddressBlockProxy::AddressBlockProxy(QObject *parent):
QSortFilterProxyModel(parent) {

}

AddressBlockProxy::~AddressBlockProxy() {
}

bool AddressBlockProxy::lessThan( const QModelIndex& left, const QModelIndex& right ) const {
	Q_ASSERT(left.column() == right.column());

	switch (left.column()) {
		case AddressBlockDelegate::OFFSET_COLUMN: 
		case AddressBlockDelegate::RESET_VALUE_COLUMN:
		case AddressBlockDelegate::RESET_MASK_COLUMN: {

			// convert the data on left index into number
			QString leftStr = left.model()->data(left, Qt::DisplayRole).toString();
			quint64 leftNumber = Utils::str2Int(leftStr);

			// convert data on right index into number
			QString rightStr = right.model()->data(right, Qt::DisplayRole).toString();
			quint64 rightNumber = Utils::str2Int(rightStr);

			// compare the numbers instead of strings to provide correct comparison results
			return leftNumber < rightNumber;
											  }
		default: {
			return QSortFilterProxyModel::lessThan(left, right);
				 }
	}
}
