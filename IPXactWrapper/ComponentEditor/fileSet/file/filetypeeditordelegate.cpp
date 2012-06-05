/* 
 *  	Created on: 7.12.2011
 *      Author: Antti Kamppi
 * 		filename: filetypeeditordelegate.cpp
 *		Project: Kactus 2
 */

#include "filetypeeditordelegate.h"

#include <QComboBox>
#include <QStringList>

FileTypeEditorDelegate::FileTypeEditorDelegate(QObject *parent):
ComboDelegate(parent) {
}

FileTypeEditorDelegate::~FileTypeEditorDelegate() {
}

QWidget* FileTypeEditorDelegate::createEditor( QWidget* parent, 
											  const QStyleOptionViewItem& option,
											  const QModelIndex& index ) const {

	QComboBox* combo = qobject_cast<QComboBox*>(
		ComboDelegate::createEditor(parent, option, index));

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

	combo->addItems(comboItems);
	combo->setEditable(true);

	return combo;
}
