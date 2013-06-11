/* 
 *	Created on:	10.6.2013
 *	Author:		Antti Kamppi
 *	File name:	vhdlimporteditor.cpp
 *	Project:		Kactus 2
*/

#include "vhdlimporteditor.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QFileDialog>
#include <QPushButton>
#include <QDebug>

VhdlImportEditor::VhdlImportEditor(const QString& basePath,
	QSharedPointer<Component> component, 
	LibraryInterface* handler,
	QWidget *parent):
QWidget(parent),
	handler_(handler),
	basePath_(basePath),
	vhdlParser_(new QTextEdit(this)),
modelParams_(new ModelParameterEditor(component, handler, this)),
ports_(new PortsEditor(component, handler, false, this)) {

	Q_ASSERT(handler_);
	Q_ASSERT(component);

	// TODO change this to combo box in final
	QPushButton* fileButton = new QPushButton(tr("Select file"), this);
	connect(fileButton, SIGNAL(clicked(bool)),
		this, SLOT(onFileButtonClick()), Qt::UniqueConnection);

	// The layout on the left side of the GUI displaying the file selector and
	// VHDL source code.
	QVBoxLayout* vhdlLayout = new QVBoxLayout();
	vhdlLayout->addWidget(fileButton);
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

void VhdlImportEditor::onFileButtonClick() {

	QString path = QFileDialog::getOpenFileName(this, tr("Select top-vhdl file"),
		basePath_);
	if (path.isEmpty()) {
		return;
	}

	// TODO add function call to set the path for vhdl parser

}
