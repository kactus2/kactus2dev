/* 
 *  	Created on: 21.6.2012
 *      Author: Antti Kamppi
 * 		filename: bridgeseditor.cpp
 *		Project: Kactus 2
 */

#include "bridgeseditor.h"

BridgesEditor::BridgesEditor(QSharedPointer<SlaveInterface> slave, 
							 QWidget *parent):
QGroupBox(tr("Bridges"), parent),
view_(this),
proxy_(this),
model_(slave, this) {

}

BridgesEditor::~BridgesEditor() {
}

bool BridgesEditor::isValid() const {
	return model_.isValid();
}

void BridgesEditor::refresh( QSharedPointer<SlaveInterface> slave ) {
	model_.refresh(slave);
}
