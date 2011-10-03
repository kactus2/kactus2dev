/* 
 *
 *  Created on: 7.4.2011
 *      Author: Antti Kamppi
 * 		filename: busifinterfacemodeeditor.cpp
 */

#include "busifinterfacemodeeditor.h"

BusIfInterfaceModeEditor::BusIfInterfaceModeEditor(BusInterface* busif, 
												   QWidget *parent): 
QWidget(parent), busif_(busif) {
}

BusIfInterfaceModeEditor::~BusIfInterfaceModeEditor() {
}

BusInterface* BusIfInterfaceModeEditor::busIf() {
	return busif_;
}

void BusIfInterfaceModeEditor::updateDisplay() {
	return;
}
