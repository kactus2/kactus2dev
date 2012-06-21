/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgesdelegate.cpp
 *		Project: Kactus 2
 */

#include "bridgesdelegate.h"

BridgesDelegate::BridgesDelegate(QSharedPointer<Component> component, 
								 QObject *parent):
QStyledItemDelegate(parent),
component_(component) {

}

BridgesDelegate::~BridgesDelegate() {
}

QWidget* BridgesDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	return NULL;
}

void BridgesDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {

}

void BridgesDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {

}

void BridgesDelegate::commitAndCloseEditor() {

}
