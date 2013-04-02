/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: filebuildersdelegate.cpp
 */

#include "filebuildersdelegate.h"

#include <common/widgets/booleanComboBox/booleancombobox.h>

#include <QComboBox>
#include <QLineEdit>
#include <QStringList>

FileBuildersDelegate::FileBuildersDelegate(QObject *parent): 
QStyledItemDelegate(parent) {
}

FileBuildersDelegate::~FileBuildersDelegate() {
}

QWidget* FileBuildersDelegate::createEditor( QWidget* parent, 
											const QStyleOptionViewItem& option, 
											const QModelIndex& index ) const {

	// if editor for file type or replace default flags
	if (index.column() == 0) {
		QComboBox* typeEditor = new QComboBox(parent);
		
		// user can set user file types so this must be editable
		typeEditor->setEditable(true);
		typeEditor->setMaxVisibleItems(25);
		typeEditor->setMinimumContentsLength(30);

		// add the file type choices to the combo box
		// add items to the box
		QStringList comboItems;
		comboItems.append("asmSource");
		comboItems.append("cSource");
		comboItems.append("cppSource");
		comboItems.append("eSource");
		comboItems.append("OVASource");
		comboItems.append("perlSource");
		comboItems.append("pslSource");
		comboItems.append("SVASource");
		comboItems.append("tclSource");
		comboItems.append("veraSource");
		comboItems.append("systemCSource");
		comboItems.append("systemCSource-2.0");
		comboItems.append("systemCSource-2.0.1");
		comboItems.append("systemCSource-2.1");
		comboItems.append("systemCSource-2.2");
		comboItems.append("systemVerilogSource");
		comboItems.append("systemVerilogSource-3.0");
		comboItems.append("systemVerilogSource-3.1");
		comboItems.append("systemVerilogSource-3.1a");
		comboItems.append("verilogSource");
		comboItems.append("verilogSource-95");
		comboItems.append("verilogSource-2001");
		comboItems.append("vhdlSource");
		comboItems.append("vhdlSource-87");
		comboItems.append("vhdlSource-93");

		comboItems.append("swObject");
		comboItems.append("swObjectLibrary");

		comboItems.append("vhdlBinaryLibrary");
		comboItems.append("verilogBinaryLibrary");

		comboItems.append("executableHdl");
		comboItems.append("unelaboratedHdl");

		comboItems.append("SDC");

		comboItems.append("unknown");

		typeEditor->addItems(comboItems);

		return typeEditor;
	}
	// if editor for command or flags
	else if (index.column() == 1 || index.column() == 2) {
		QLineEdit* lineEditor = new QLineEdit(parent);
		connect(lineEditor, SIGNAL(editingFinished()),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return lineEditor;
	}
	// if editor for replace default flags
	else if (index.column() == 3) {
		BooleanComboBox* boolBox = new BooleanComboBox(parent);
		connect(boolBox, SIGNAL(currentIndexChanged(int)),
			this, SLOT(commitAndCloseEditor()), Qt::UniqueConnection);
		return boolBox;
	}
	// use the default delegate
	else {
		return QStyledItemDelegate::createEditor(parent, option, index);
	}
}

void FileBuildersDelegate::setEditorData( QWidget* editor, 
										 const QModelIndex& index ) const {

	// if editor for file type
	if (index.column() == 0) {

		QString text = index.model()->data(index, Qt::DisplayRole).toString();
		QComboBox* combo = qobject_cast<QComboBox*>(editor);

		int comboIndex = combo->findText(text);
		combo->setCurrentIndex(comboIndex);
	}
	// if editor for command or flags
	else if (index.column() == 1 || index.column() == 2) {

		QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
		QString value = index.model()->data(index, Qt::DisplayRole).toString();
		defaultEdit->setText(value);
	}
	// if editor for replace default flags
	else if (index.column() == 3) {

		BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
		bool value = index.model()->data(index, Qt::DisplayRole).toBool();
		boolBox->setCurrentValue(value);
	}
	// use the default delegate
	else {
	 QStyledItemDelegate::setEditorData(editor, index);
	}
}

void FileBuildersDelegate::setModelData( QWidget* editor, 
										QAbstractItemModel* model, 
										const QModelIndex& index ) const {

	// if editor for file type
	if (index.column() == 0) {

		QComboBox* combo = qobject_cast<QComboBox*>(editor);
		QString text = combo->currentText();
		model->setData(index, text, Qt::EditRole);
	}
	// if editor for command or flags
	else if (index.column() == 1 || index.column() == 2) {

		QLineEdit* defaultEdit = qobject_cast<QLineEdit*>(editor);
		QString value = defaultEdit->text();
		model->setData(index, value, Qt::EditRole);
	}
	// if editor for replace default flags
	else if (index.column() == 3) {

		BooleanComboBox* boolBox = qobject_cast<BooleanComboBox*>(editor);
		bool value = boolBox->getCurrentValue();
		model->setData(index, value, Qt::EditRole);
	}
	// use the default delegate
	else {
		QStyledItemDelegate::setModelData(editor, model, index);
	}
}

void FileBuildersDelegate::commitAndCloseEditor() {

	QComboBox* comboEditor = qobject_cast<QComboBox*>(sender());
	QLineEdit* lineEditor = qobject_cast<QLineEdit*>(sender());
	QWidget* editor = qobject_cast<QWidget*>(sender());

	if (comboEditor) {
		emit commitData(comboEditor);
		emit closeEditor(comboEditor);
	}
	else if (lineEditor) {
		emit commitData(lineEditor);
		emit closeEditor(lineEditor);
	}
	else {
		emit commitData(editor);
		emit closeEditor(editor);
	}
}
