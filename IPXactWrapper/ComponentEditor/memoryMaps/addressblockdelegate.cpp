/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockdelegate.cpp
 *		Project: Kactus 2
 */

#include "addressblockdelegate.h"
#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>
#include <models/generaldeclarations.h>

#include <QLineEdit>
#include <QSpinBox>

AddressBlockDelegate::AddressBlockDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

AddressBlockDelegate::~AddressBlockDelegate() {
}

QWidget* AddressBlockDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case AddressBlockDelegate::NAME_COLUMN:
		case AddressBlockDelegate::OFFSET_COLUMN:
		case AddressBlockDelegate::DESC_COLUMN:
		case AddressBlockDelegate::RESET_VALUE_COLUMN:
		case AddressBlockDelegate::RESET_MASK_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
													  }
		case AddressBlockDelegate::SIZE_COLUMN: {
			QSpinBox* spinBox = new QSpinBox(parent);
			connect(spinBox, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			spinBox->setRange(1, 4096);
			return spinBox;
												}
		case AddressBlockDelegate::DIM_COLUMN: {
			QSpinBox* spinBox = new QSpinBox(parent);
			connect(spinBox, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			spinBox->setRange(0, 4096);
			return spinBox;
											   }
		case AddressBlockDelegate::VOLATILE_COLUMN: {
			BooleanComboBox* boolBox = new BooleanComboBox(parent);
			return boolBox;
													}
		case AddressBlockDelegate::ACCESS_COLUMN: {
			AccessComboBox* accessBox = new AccessComboBox(parent);
			return accessBox;
												  }
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void AddressBlockDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case AddressBlockDelegate::NAME_COLUMN:
		case AddressBlockDelegate::OFFSET_COLUMN:
		case AddressBlockDelegate::DESC_COLUMN:
		case AddressBlockDelegate::RESET_VALUE_COLUMN:
		case AddressBlockDelegate::RESET_MASK_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
													  }
		case AddressBlockDelegate::SIZE_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int value = index.model()->data(index, Qt::DisplayRole).toUInt();
			spinBox->setValue(value);
			break;
												}
		case AddressBlockDelegate::DIM_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int value = index.model()->data(index, Qt::DisplayRole).toUInt();
			spinBox->setValue(value);
			break;
											   }
		case AddressBlockDelegate::VOLATILE_COLUMN: {
			BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
			Q_ASSERT(boolBox);

			bool value = index.model()->data(index, Qt::DisplayRole).toBool();
			boolBox->setCurrentValue(value);
			break;
													}
		case AddressBlockDelegate::ACCESS_COLUMN: {
			AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
			Q_ASSERT(accessBox);

			General::Access access = General::str2Access(index.model()->data(
				index, Qt::DisplayRole).toString(), General::ACCESS_COUNT);
			accessBox->setCurrentValue(access);
			break;
												  }
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}
}

void AddressBlockDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case AddressBlockDelegate::NAME_COLUMN:
		case AddressBlockDelegate::OFFSET_COLUMN:
		case AddressBlockDelegate::DESC_COLUMN:
		case AddressBlockDelegate::RESET_VALUE_COLUMN:
		case AddressBlockDelegate::RESET_MASK_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
													  }
		case AddressBlockDelegate::SIZE_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int value = spinBox->value();
			model->setData(index, value, Qt::EditRole);
			break;
												}
		case AddressBlockDelegate::DIM_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int value = spinBox->value();
			model->setData(index, value, Qt::EditRole);
			break;
											   }
		case AddressBlockDelegate::VOLATILE_COLUMN: {
			BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
			Q_ASSERT(boolBox);

			bool value = boolBox->getCurrentValue();
			model->setData(index, value, Qt::EditRole);
			break;
													}
		case AddressBlockDelegate::ACCESS_COLUMN: {
			AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
			Q_ASSERT(accessBox);

			General::Access access = accessBox->getCurrentValue();
			model->setData(index, General::access2Str(access), Qt::EditRole);
			break;
												  }
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void AddressBlockDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
