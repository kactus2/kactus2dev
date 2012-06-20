/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemodeeditor.cpp
 */

#include "busifinterfacemodeeditor.h"

BusIfInterfaceModeEditor::BusIfInterfaceModeEditor(QSharedPointer<BusInterface> busif,
												   QSharedPointer<Component> component,
												   const QString& title, 
												   QWidget *parent):
QGroupBox(title, parent),
busif_(busif),
component_(component) {

	Q_ASSERT(busif);
	Q_ASSERT(component);
}

BusIfInterfaceModeEditor::~BusIfInterfaceModeEditor() {
}
