/* 
 *
 *  Created on: 4.2.2011
 *      Author: Antti Kamppi
 * 		filename: comboselector.cpp
 */

#include "comboselector.h"
#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/component.h>

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

QString ComboSelector::execDialog() {

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

    return selector.execDialog();
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
	for (unsigned int i = 0; i < General::FILE_TYPE_COUNT; ++i) {
		comboItems.append(QString(General::FILE_TYPES[i]));
	}
	selector.setComboBoxItems(comboItems);

	selector.setTextValue(selected);

    return selector.execDialog();
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

    return selector.execDialog();
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

    return selector.execDialog();
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

    return selector.execDialog();
}
