/* 
 *  	Created on: 27.2.2012
 *      Author: Antti Kamppi
 * 		filename: addressspacerefdelegate.cpp
 *		Project: Kactus 2
 */

#include "addressspacerefdelegate.h"

#include <QStringList>
#include <QComboBox>

AddressSpaceRefDelegate::AddressSpaceRefDelegate(QObject *parent,
												 QSharedPointer<Component> component):
ComboDelegate(parent),
component_(component) {
	Q_ASSERT(component_);
}

AddressSpaceRefDelegate::~AddressSpaceRefDelegate()
{

}

QWidget* AddressSpaceRefDelegate::createEditor( QWidget* parent, 
											   const QStyleOptionViewItem& option, 
											   const QModelIndex& index ) const {
												  
	Q_ASSERT(component_);
	QComboBox* combo = qobject_cast<QComboBox*>(
		ComboDelegate::createEditor(parent, option, index));
	Q_ASSERT(combo);

	QStringList addrSpaceNames = component_->getAddressSpaceNames();
	combo->addItems(addrSpaceNames);
	return combo;
}
