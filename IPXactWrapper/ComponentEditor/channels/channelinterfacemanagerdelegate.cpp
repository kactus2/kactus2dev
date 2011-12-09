/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: channelinterfacemanagerdelegate.cpp
 *		Project: Kactus 2
 */

#include "channelinterfacemanagerdelegate.h"

#include <QComboBox>
#include <QStringList>

ChannelInterfaceManagerDelegate::ChannelInterfaceManagerDelegate( QObject *parent,
																 QSharedPointer<Component> component ):
ComboDelegate(parent),
component_(component) {
	Q_ASSERT(component_);
}

ChannelInterfaceManagerDelegate::~ChannelInterfaceManagerDelegate() {
}

QWidget* ChannelInterfaceManagerDelegate::createEditor( QWidget* parent, 
													   const QStyleOptionViewItem& option, 
													   const QModelIndex& index ) const {

	Q_ASSERT(component_);
	QComboBox* combo = qobject_cast<QComboBox*>(
		ComboDelegate::createEditor(parent, option, index));
	Q_ASSERT(combo);

	QStringList interfaceNames = component_->getBusInterfaceNames();
	combo->addItems(interfaceNames);
	return combo;
}
