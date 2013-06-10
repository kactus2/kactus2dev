/* 
 *	Created on:	10.6.2013
 *	Author:		Antti Kamppi
 *	File name:	vhdlimporteditor.cpp
 *	Project:		Kactus 2
*/

#include "vhdlimporteditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>

VhdlImportEditor::VhdlImportEditor(const QString& basePath,
	QSharedPointer<Component> component, 
	LibraryInterface* handler,
	QWidget *parent):
QWidget(parent),
	vhdlParser_(new QTextEdit(this)),
modelParams_(new ModelParameterEditor(component, handler, this)),
ports_(new PortsEditor(component, handler, false, this)) {


	// The layout on the left side of the GUI displaying the file selector and
	// VHDL source code.
	QVBoxLayout* vhdlLayout = new QVBoxLayout();
	vhdlLayout->addWidget(vhdlParser_);

	// The layout on the right side of the GUI displaying the editors.
	QVBoxLayout* editorLayout = new QVBoxLayout();
	editorLayout->addWidget(modelParams_);
	editorLayout->addWidget(ports_);

	// The top layout which owns other layouts
	QHBoxLayout* topLayout = new QHBoxLayout(this);
	topLayout->addLayout(vhdlLayout);
	topLayout->addLayout(editorLayout);
}

VhdlImportEditor::~VhdlImportEditor() {
}

void VhdlImportEditor::initializeFileSelection() {
	qDebug() << "Initializing the files";
}

bool VhdlImportEditor::checkEditorValidity() {
	// check both editors
	if (!modelParams_->isValid()) {
		return false;
	}
	else if (!ports_->isValid()) {
		return false;
	}
	return true;
}
