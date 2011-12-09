/* 
 *  	Created on: 9.12.2011
 *      Author: Antti Kamppi
 * 		filename: groupmanagerdelegate.cpp
 *		Project: Kactus 2
 */

#include "groupmanagerdelegate.h"

#include <QComboBox>
#include <QStringList>

GroupManagerDelegate::GroupManagerDelegate(QObject *parent):
ComboDelegate(parent) {
}

GroupManagerDelegate::~GroupManagerDelegate() {
}

QWidget* GroupManagerDelegate::createEditor( QWidget* parent, 
											const QStyleOptionViewItem& option,
											const QModelIndex& index ) const {
	
	QComboBox* combo = qobject_cast<QComboBox*>(ComboDelegate::createEditor(parent, option, index));
	Q_ASSERT(combo);
	
	// add the items to the box
	QStringList comboItems;
	comboItems.append(tr("diagnostics"));
	comboItems.append(tr("documentation"));
	comboItems.append(tr("projectFiles"));
	comboItems.append(tr("simulation"));
	comboItems.append(tr("sourceFiles"));

	combo->addItems(comboItems);
	combo->setEditable(true);
	return combo;
}
