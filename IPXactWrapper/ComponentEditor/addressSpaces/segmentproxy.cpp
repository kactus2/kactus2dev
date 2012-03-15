/* 
 *  	Created on: 15.3.2012
 *      Author: Antti Kamppi
 * 		filename: segmentproxy.cpp
 *		Project: Kactus 2
 */

#include "segmentproxy.h"

#include <common/utils.h>

SegmentProxy::SegmentProxy(QObject *parent):
QSortFilterProxyModel(parent) {
}

SegmentProxy::~SegmentProxy() {
}

bool SegmentProxy::lessThan( const QModelIndex& left, const QModelIndex& right ) const {

	Q_ASSERT(left.column() == right.column());

	switch (left.column()) {
		
		// the offset and range require special sorting
		case 1:
		case 2: {
			
			// get the data from the model
			QString leftStr = left.model()->data(left).toString();
			QString rightStr = right.model()->data(right).toString();

			// convert the data from string to integer
			quint64 leftValue = Utils::str2Int(leftStr);
			quint64 rightValue = Utils::str2Int(rightStr);

			return leftValue < rightValue;
				}
		
		// others use the default sorting
		default: {
			return QSortFilterProxyModel::lessThan(left, right);
				 }
	}
}
