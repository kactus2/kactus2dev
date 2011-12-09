/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: comboselector.cpp
 */

#include "comboselector.h"

#include <models/component.h>

#include <QStringList>

ComboSelector::ComboSelector(QWidget *parent /* = 0*/,
							 const QString selected /*= QString()*/,							  
							 Qt::WindowFlags f /*= ´Qt::Dialog*/):
QInputDialog(parent, f) {

	setCancelButtonText(tr("Cancel"));
	setComboBoxEditable(true);
	setInputMode(QInputDialog::TextInput);
	setLabelText(tr("Select group type or create your own group type"));
	setOkButtonText(tr("Ok"));
	setTextValue(selected);
}

ComboSelector::~ComboSelector() {
}

QString ComboSelector::exec() {

	// if user clicked cancel
	if (!QInputDialog::exec()) {
		return QString();
	}
	// otherwise return the string that was selected
	return textValue();
}

QString ComboSelector::selectGroupType(QWidget* parent /* = 0*/,
									   const QString selected,
									   const QString label) {

	// create the selector instance
	ComboSelector selector(parent, selected);

	// set options for the selector box
	selector.setCancelButtonText(tr("Cancel"));
	selector.setComboBoxEditable(true);
	selector.setInputMode(QInputDialog::TextInput);
	selector.setLabelText(label);
	selector.setOkButtonText(tr("Ok"));

	// add the items to the box
	QStringList comboItems;
	comboItems.append(tr("diagnostics"));
	comboItems.append(tr("documentation"));
	comboItems.append(tr("projectFiles"));
	comboItems.append(tr("simulation"));
	comboItems.append(tr("sourceFiles"));
	selector.setComboBoxItems(comboItems);

	selector.setTextValue(selected);

	return selector.exec();
}

QString ComboSelector::selectFileType(QWidget* parent /*= 0*/, 
									  const QString selected /*= QString()*/, 
const QString label /*= tr("Select a file type or set your own file type")*/ ) {

	// create the selector instance
	ComboSelector selector(parent, selected);

	// set options for the selector box
	selector.setCancelButtonText(tr("Cancel"));
	selector.setComboBoxEditable(true);
	selector.setInputMode(QInputDialog::TextInput);
	selector.setLabelText(label);
	selector.setOkButtonText(tr("Ok"));

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

	selector.setComboBoxItems(comboItems);

	selector.setTextValue(selected);

	return selector.exec();
}

QString ComboSelector::selectBusInterface(
	QSharedPointer<Component> component, 
	QWidget* parent /*= 0*/, 
	const QString selected /*= QString()*/, 
	const QString label /*= tr("Select a bus interface to include in the channel")*/ ) {

	// create the selector instance
	ComboSelector selector(parent, selected);

	// set options for the selector box
	selector.setCancelButtonText(tr("Cancel"));
	selector.setComboBoxEditable(true);
	selector.setInputMode(QInputDialog::TextInput);
	selector.setLabelText(label);
	selector.setOkButtonText(tr("Ok"));

	// add items to the box
	QStringList interfaceNames = component->getBusInterfaceNames();

	selector.setComboBoxItems(interfaceNames);

	selector.setTextValue(selected);

	return selector.exec();
}

QString ComboSelector::selectFileSet( QSharedPointer<Component> component, 
									 QWidget* parent /*= 0*/, 
									 const QString selected /*= QString()*/, 
									 const QString label /*= tr("Select a file set")*/ ) {

	// create the selector instance
	ComboSelector selector(parent, selected);

	// set options for the selector box
	selector.setCancelButtonText(tr("Cancel"));
	selector.setComboBoxEditable(true);
	selector.setInputMode(QInputDialog::TextInput);
	selector.setLabelText(label);
	selector.setOkButtonText(tr("Ok"));

	// add items to the box
	QStringList fileSetNames = component->getFileSetNames();

	selector.setComboBoxItems(fileSetNames);

	selector.setTextValue(selected);

	return selector.exec();
}

QString ComboSelector::selectView( QSharedPointer<Component> component,
								  QWidget* parent /*= 0*/, 
								  const QString selected /*= QString()*/,
								  const QString label /*= tr("Select a view")*/) {

	// create the selector instance
	ComboSelector selector(parent, selected);

	// set the options for the selector box
	selector.setCancelButtonText(tr("Cancel"));
	selector.setComboBoxEditable(false);
	selector.setInputMode(QInputDialog::TextInput);
	selector.setLabelText(label);
	selector.setOkButtonText(tr("Ok"));

	// add items to the box
	QStringList viewNames = component->getViewNames();

	selector.setComboBoxItems(viewNames);

	selector.setTextValue(selected);

	return selector.exec();
}
