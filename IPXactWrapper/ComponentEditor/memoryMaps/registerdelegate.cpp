/* 
 *  	Created on: 25.8.2012
 *      Author: Antti Kamppi
 * 		filename: registerdelegate.cpp
 *		Project: Kactus 2
 */

#include "registerdelegate.h"
#include <common/widgets/booleanComboBox/booleancombobox.h>
#include <common/widgets/accessComboBox/accesscombobox.h>
#include <common/widgets/modWriteComboBox/modwritecombobox.h>
#include <common/widgets/readActionComboBox/readactioncombobox.h>
#include <common/widgets/testConstraintComboBox/testconstraintcombobox.h>

#include <QLineEdit>
#include <QSpinBox>

RegisterDelegate::RegisterDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

RegisterDelegate::~RegisterDelegate() {
}

QWidget* RegisterDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case RegisterDelegate::NAME_COLUMN:
		case RegisterDelegate::DESC_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
											}
		case RegisterDelegate::OFFSET_COLUMN: {
			QSpinBox* spinBox = new QSpinBox(parent);
			connect(spinBox, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			spinBox->setRange(0, 4096);
			return spinBox;
											  }
		case RegisterDelegate::WIDTH_COLUMN: {
			QSpinBox* spinBox = new QSpinBox(parent);
			connect(spinBox, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			spinBox->setRange(1, 4096);
			return spinBox;
											 }
		case RegisterDelegate::VOLATILE_COLUMN:
		case RegisterDelegate::TESTABLE_COLUMN: {
			BooleanComboBox* boolBox = new BooleanComboBox(parent);
			return boolBox;
												}
		case RegisterDelegate::ACCESS_COLUMN: {
			AccessComboBox* accessBox = new AccessComboBox(parent);
			return accessBox;
											  }
		case RegisterDelegate::MOD_WRITE_COLUMN: {
			ModWriteComboBox* modBox = new ModWriteComboBox(parent);
			return modBox;
												 }
		case RegisterDelegate::READ_ACTION_COLUMN: {
			ReadActionComboBox* readBox = new ReadActionComboBox(parent);
			return readBox;
												   }
		case RegisterDelegate::TEST_CONSTR_COLUMN: {
			TestConstraintComboBox* testBox = new TestConstraintComboBox(parent);
			return testBox;
												   }
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void RegisterDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case RegisterDelegate::NAME_COLUMN:
		case RegisterDelegate::DESC_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
											}
		case RegisterDelegate::OFFSET_COLUMN:
		case RegisterDelegate::WIDTH_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int value = index.model()->data(index, Qt::DisplayRole).toUInt();
			spinBox->setValue(value);
			break;
											 }
		case RegisterDelegate::VOLATILE_COLUMN:
		case RegisterDelegate::TESTABLE_COLUMN: {
			BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
			Q_ASSERT(boolBox);

			bool value = index.model()->data(index, Qt::DisplayRole).toBool();
			boolBox->setCurrentValue(value);
			break;
												}
		case RegisterDelegate::ACCESS_COLUMN: {
			AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
			Q_ASSERT(accessBox);

			General::Access access = General::str2Access(index.model()->data(
				index, Qt::DisplayRole).toString(), General::ACCESS_COUNT);
			accessBox->setCurrentValue(access);
			break;
											  }
		case RegisterDelegate::MOD_WRITE_COLUMN: {
			ModWriteComboBox* modBox = qobject_cast<ModWriteComboBox*>(editor);
			Q_ASSERT(modBox);

			General::ModifiedWrite modWrite = General::str2ModifiedWrite(
				index.model()->data(index, Qt::DisplayRole).toString());
			modBox->setCurrentValue(modWrite);
			break;
												 }
		case RegisterDelegate::READ_ACTION_COLUMN: {
			ReadActionComboBox* readBox = qobject_cast<ReadActionComboBox*>(editor);
			Q_ASSERT(readBox);

			General::ReadAction readAction = General::str2ReadAction(
				index.model()->data(index, Qt::DisplayRole).toString());
			readBox->setCurrentValue(readAction);
			break;
												   }
		case RegisterDelegate::TEST_CONSTR_COLUMN: {
			TestConstraintComboBox* testBox = qobject_cast<TestConstraintComboBox*>(editor);
			Q_ASSERT(testBox);

			General::TestConstraint testConstr = General::str2TestConstraint(
				index.model()->data(index, Qt::DisplayRole).toString());
			testBox->setCurrentValue(testConstr);
			break;
												   }
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}
}

void RegisterDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case RegisterDelegate::NAME_COLUMN:
		case RegisterDelegate::DESC_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
											}
		case RegisterDelegate::OFFSET_COLUMN:
		case RegisterDelegate::WIDTH_COLUMN: {
			QSpinBox* spinBox = qobject_cast<QSpinBox*>(editor);
			Q_ASSERT(spinBox);

			unsigned int value = spinBox->value();
			model->setData(index, value, Qt::EditRole);
			break;
											 }
		case RegisterDelegate::VOLATILE_COLUMN:
		case RegisterDelegate::TESTABLE_COLUMN: {
			BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
			Q_ASSERT(boolBox);

			bool value = boolBox->getCurrentValue();
			model->setData(index, value, Qt::EditRole);
			break;
												}
		case RegisterDelegate::ACCESS_COLUMN: {
			AccessComboBox* accessBox = qobject_cast<AccessComboBox*>(editor);
			Q_ASSERT(accessBox);

			General::Access access = accessBox->getCurrentValue();
			model->setData(index, General::access2Str(access), Qt::EditRole);
			break;
											  }
		case RegisterDelegate::MOD_WRITE_COLUMN: {
			ModWriteComboBox* modBox = qobject_cast<ModWriteComboBox*>(editor);
			Q_ASSERT(modBox);

			General::ModifiedWrite modWrite = modBox->getCurrentValue();
			model->setData(index, General::modifiedWrite2Str(modWrite), Qt::EditRole);
			break;
												 }
		case RegisterDelegate::READ_ACTION_COLUMN: {
			ReadActionComboBox* readBox = qobject_cast<ReadActionComboBox*>(editor);
			Q_ASSERT(readBox);

			General::ReadAction readAction = readBox->getCurrentValue();
			model->setData(index, General::readAction2Str(readAction), Qt::EditRole);
			break;
												   }
		case RegisterDelegate::TEST_CONSTR_COLUMN: {
			TestConstraintComboBox* testBox = qobject_cast<TestConstraintComboBox*>(editor);
			Q_ASSERT(testBox);

			General::TestConstraint testConstr = testBox->getCurrentValue();
			model->setData(index, General::testConstraint2Str(testConstr), Qt::EditRole);
			break;
												   }
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void RegisterDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
