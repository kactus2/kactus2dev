/* 
 *  	Created on: 28.8.2012
 *      Author: Antti Kamppi
 * 		filename: enumeratedvaluedelegate.cpp
 *		Project: Kactus 2
 */

#include "enumeratedvaluedelegate.h"

#include <QLineEdit>
#include <QComboBox>

EnumeratedValueDelegate::EnumeratedValueDelegate(QObject *parent):
QStyledItemDelegate(parent) {
}

EnumeratedValueDelegate::~EnumeratedValueDelegate() {
}

QWidget* EnumeratedValueDelegate::createEditor( QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const {
	switch (index.column()) {
		case EnumeratedValueDelegate::NAME_COLUMN: 
		case EnumeratedValueDelegate::VALUE_COLUMN: {
			QLineEdit* edit = new QLineEdit(parent);
			connect(edit, SIGNAL(editingFinished()),
				this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
			return edit;
												   }
		case EnumeratedValueDelegate::USAGE_COLUMN: {
			QComboBox* combo = new QComboBox(parent);
			combo->addItem(tr("read-write"));
			combo->addItem(tr("read"));
			combo->addItem(tr("write"));
			return combo;
													}
		default: {
			return QStyledItemDelegate::createEditor(parent, option, index);
				 }
	}
}

void EnumeratedValueDelegate::setEditorData( QWidget* editor, const QModelIndex& index ) const {
	switch (index.column()) {
		case EnumeratedValueDelegate::NAME_COLUMN:
		case EnumeratedValueDelegate::VALUE_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			const QString text = index.model()->data(index, Qt::DisplayRole).toString();
			edit->setText(text);
			break;
												   }
		case EnumeratedValueDelegate::USAGE_COLUMN: {
			QComboBox* combo = qobject_cast<QComboBox*>(editor);
			Q_ASSERT(combo);

			QString text = index.model()->data(index, Qt::DisplayRole).toString();
			int comboIndex = combo->findText(text);
			combo->setCurrentIndex(comboIndex);
			break;
													}
		default: {
			QStyledItemDelegate::setEditorData(editor, index);
			break;
				 }
	}
}

void EnumeratedValueDelegate::setModelData( QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const {
	switch (index.column()) {
		case EnumeratedValueDelegate::NAME_COLUMN:
		case EnumeratedValueDelegate::VALUE_COLUMN: {
			QLineEdit* edit = qobject_cast<QLineEdit*>(editor);
			Q_ASSERT(edit);

			QString text = edit->text();
			model->setData(index, text, Qt::EditRole);
			break;
												   }
		case EnumeratedValueDelegate::USAGE_COLUMN: {
			QComboBox* combo = qobject_cast<QComboBox*>(editor);
			Q_ASSERT(combo);

			QString text = combo->currentText();
			model->setData(index, text, Qt::EditRole);
			break;
													}
		default: {
			QStyledItemDelegate::setModelData(editor, model, index);
			break;
				 }
	}
}

void EnumeratedValueDelegate::commitAndCloseEditor() {
	QWidget* edit = qobject_cast<QWidget*>(sender());
	Q_ASSERT(edit);

	emit commitData(edit);
	emit closeEditor(edit);
}
