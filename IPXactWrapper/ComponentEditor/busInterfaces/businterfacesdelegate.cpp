/* 
 *  	Created on: 15.6.2012
 *      Author: Antti Kamppi
 * 		filename: businterfacesdelegate.cpp
 *		Project: Kactus 2
 */

#include "businterfacesdelegate.h"
#include <common/widgets/interfaceModeSelector/interfacemodeselector.h>

#include <QLineEdit>

BusInterfacesDelegate::BusInterfacesDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

BusInterfacesDelegate::~BusInterfacesDelegate() {
}

QWidget* BusInterfacesDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case BusInterfacesDelegate::NAME_COLUMN:
		case BusInterfacesDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
														}
		case BusInterfacesDelegate::BUSDEF_COLUMN:
		case BusInterfacesDelegate::ABSDEF_COLUMN: {
			Q_ASSERT(false);
			return NULL;
												   }
		case BusInterfacesDelegate::IF_MODE_COLUMN: {
			InterfaceModeSelector* selector = new InterfaceModeSelector(parent);
			connect(selector, SIGNAL(currentIndexChanged(int)),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return selector;
													}
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void BusInterfacesDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case BusInterfacesDelegate::NAME_COLUMN:
		case BusInterfacesDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
														}
		case BusInterfacesDelegate::BUSDEF_COLUMN:
		case BusInterfacesDelegate::ABSDEF_COLUMN: {
			Q_ASSERT(false);
			break;
												   }
		case BusInterfacesDelegate::IF_MODE_COLUMN: {
			InterfaceModeSelector* selector = qobject_cast<InterfaceModeSelector*>(editor);
			Q_ASSERT(selector);

			QString modeName = index.model()->data(index, Qt::DisplayRole).toString();
			selector->setMode(modeName);
			break;
													}
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
				 }
	}
}

void BusInterfacesDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case BusInterfacesDelegate::NAME_COLUMN:
		case BusInterfacesDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
														}
		case BusInterfacesDelegate::BUSDEF_COLUMN:
		case BusInterfacesDelegate::ABSDEF_COLUMN: {
			Q_ASSERT(false);
			break;
												   }
		case BusInterfacesDelegate::IF_MODE_COLUMN: {
			InterfaceModeSelector* selector = qobject_cast<InterfaceModeSelector*>(editor);
			Q_ASSERT(selector);

			General::InterfaceMode mode = selector->selected();
			model->setData(index, mode, Qt::EditRole);
			break;
													}	
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
				 }
	}
}

void BusInterfacesDelegate::commitAndCloseEditor() {

	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	QComboBox* combo = qobject_cast<QComboBox*>(edit);

	if (combo) {
		emit commitData(combo);
	}
	else {
		emit commitData(edit);
		emit closeEditor(edit);
	}
}
