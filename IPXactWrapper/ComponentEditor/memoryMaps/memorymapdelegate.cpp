/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapdelegate.cpp
 *		Project: Kactus 2
 */

#include "memorymapdelegate.h"
#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/usageComboBox/usagecombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>

#include <QLineEdit>
#include <QSpinBox>

MemoryMapDelegate::MemoryMapDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

MemoryMapDelegate::~MemoryMapDelegate() {
}

QWidget* MemoryMapDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case MemoryMapDelegate::USAGE_COLUMN: {
			UsageComboBox* usageBox = new UsageComboBox(parent);
			return usageBox;
											  }
		case MemoryMapDelegate::NAME_COLUMN: 
		case MemoryMapDelegate::BASE_COLUMN:
		case MemoryMapDelegate::RANGE_COLUMN:
		case MemoryMapDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
													}
		case MemoryMapDelegate::WIDTH_COLUMN: {
			QSpinBox* spinBox = new QSpinBox(parent);
			connect(spinBox, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			spinBox->setRange(0, 4096);
			return spinBox;
											  }
		case MemoryMapDelegate::ACCESS_COLUMN: {
			AccessComboBox* accessBox = new AccessComboBox(parent);
			return accessBox;
											   }
		case MemoryMapDelegate::VOLATILE_COLUMN: {
			BooleanComboBox* boolBox = new BooleanComboBox(parent);
			return boolBox;
											  }
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void MemoryMapDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case MemoryMapDelegate::USAGE_COLUMN: {
			UsageComboBox* usageBox = qobject_cast<UsageComboBox*>(editor);
			Q_ASSERT(usageBox);

			General::Usage usage = General::str2Usage(
				index.model()->data(index, Qt::DisplayRole).toString(), General::USAGE_COUNT);
			usageBox->setCurrentValue(usage);
			break;
											  }
		case MemoryMapDelegate::NAME_COLUMN: 
		case MemoryMapDelegate::BASE_COLUMN:
		case MemoryMapDelegate::RANGE_COLUMN:
		case MemoryMapDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
													}
		case MemoryMapDelegate::WIDTH_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int value = index.model()->data(index, Qt::DisplayRole).toUInt();
			spinBox->setValue(value);
			break;
											  }
		case MemoryMapDelegate::ACCESS_COLUMN: {
			AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
			Q_ASSERT(accessBox);

			General::Access access = General::str2Access(index.model()->data(
				index, Qt::DisplayRole).toString(), General::ACCESS_COUNT);
			accessBox->setCurrentValue(access);
			break;
											   }
		case MemoryMapDelegate::VOLATILE_COLUMN: {
			BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
			Q_ASSERT(boolBox);

			bool value = index.model()->data(index, Qt::DisplayRole).toBool();
			boolBox->setCurrentValue(value);
			break;
											  }
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}
}

void MemoryMapDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case MemoryMapDelegate::USAGE_COLUMN: {
			UsageComboBox* usageBox = qobject_cast<UsageComboBox*>(editor);
			Q_ASSERT(usageBox);

			General::Usage usage = usageBox->getCurrentValue();
			model->setData(index, General::usage2Str(usage), Qt::EditRole);
			break;
											  }
		case MemoryMapDelegate::NAME_COLUMN: 
		case MemoryMapDelegate::BASE_COLUMN:
		case MemoryMapDelegate::RANGE_COLUMN:
		case MemoryMapDelegate::DESCRIPTION_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
													}
		case MemoryMapDelegate::WIDTH_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int value = spinBox->value();
			model->setData(index, value, Qt::EditRole);
			break;
											  }
		case MemoryMapDelegate::ACCESS_COLUMN: {
			AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
			Q_ASSERT(accessBox);

			General::Access access = accessBox->getCurrentValue();
			model->setData(index, General::access2Str(access), Qt::EditRole);
			break;
											   }
		case MemoryMapDelegate::VOLATILE_COLUMN: {
			BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
			Q_ASSERT(boolBox);

			bool value = boolBox->getCurrentValue();
			model->setData(index, value, Qt::EditRole);
			break;
											  }
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void MemoryMapDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
