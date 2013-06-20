/* 
 *	Created on:	10.6.2013
 *	Author:		Antti Kamppi
 *	File name:	vhdlimporteditor.cpp
 *	Project:		Kactus 2
*/

#include "vhdlimporteditor.h"
#include <common/widgets/FileSelector/fileselector.h>
#include <models/generaldeclarations.h>

#include <Qapplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDebug>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor()
//-----------------------------------------------------------------------------
VhdlImportEditor::VhdlImportEditor(const QString& basePath,
	QSharedPointer<Component> component, 
	LibraryInterface* handler,
	QWidget *parent):
    QWidget(parent),
	vhdlParser_(new VhdlParser(this)),
	basePath_(basePath),
	fileSelector_(new FileSelector(component, this)),
modelParams_(new ModelParameterEditor(component, handler, this)),
ports_(new PortsEditor(component, handler, false, this)) {

	Q_ASSERT(component);

	// only vhdl files are selected
	fileSelector_->addFilter("vhd");
	fileSelector_->addFilter("vhdl");
	connect(fileSelector_, SIGNAL(fileSelected(const QString&)),
		this, SLOT(onFileSelected(const QString&)), Qt::UniqueConnection);

	// CSV import/export is disabled in the wizard
	modelParams_->setAllowImportExport(false);
	ports_->setAllowImportExport(false);

	connect(modelParams_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(addGeneric(QSharedPointer<ModelParameter>)),
		modelParams_, SLOT(addModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(removeGeneric(const QString&)),
		modelParams_, SLOT(removeModelParameter(const QString&)), Qt::UniqueConnection);
    connect(modelParams_, SIGNAL(parameterChanged(QString const&)),
             vhdlParser_, SLOT(editorChangedModelParameter(QString const&)), Qt::UniqueConnection);
    connect(modelParams_, SIGNAL(modelParameterRemoved(QString const&)),
            vhdlParser_, SLOT(editorRemovedModelParameter(QString const&)), Qt::UniqueConnection);

	connect(ports_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(addPort(QSharedPointer<Port>)),
		ports_, SLOT(addPort(QSharedPointer<Port>)), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(removePort(const QString&)),
		ports_, SLOT(removePort(const QString&)), Qt::UniqueConnection);
    connect(ports_, SIGNAL(lockedPortRemoved(QString const&)),
		vhdlParser_, SLOT(editorRemovedPort(QString const&)), Qt::UniqueConnection);

	// The layout on the left side of the GUI displaying the file selector and
	// VHDL source code.
	QVBoxLayout* vhdlLayout = new QVBoxLayout();
	vhdlLayout->addWidget(fileSelector_);
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

//-----------------------------------------------------------------------------
// Function: ~VhdlImportEditor()
//-----------------------------------------------------------------------------
VhdlImportEditor::~VhdlImportEditor() {
}

//-----------------------------------------------------------------------------
// Function: initializeFileSelection()
//-----------------------------------------------------------------------------
void VhdlImportEditor::initializeFileSelection() {
	fileSelector_->refresh();
}

//-----------------------------------------------------------------------------
// Function: checkEditorValidity()
//-----------------------------------------------------------------------------
bool VhdlImportEditor::checkEditorValidity() const {
	// check both editors
	if (!modelParams_->isValid()) {
		return false;
	}
	else if (!ports_->isValid()) {
		return false;
	}
	return true;
}

//-----------------------------------------------------------------------------
// Function: onFileSelected()
//-----------------------------------------------------------------------------
void VhdlImportEditor::onFileSelected( const QString& filePath ) {
	if (filePath.isEmpty()) {
		return;
	}

	QString absPath = General::getAbsolutePath(basePath_+"/", filePath);

	// if the absolute path can not be converted
	if (absPath.isEmpty()) {
		return;
	}

    QApplication::setOverrideCursor(Qt::WaitCursor);
    vhdlParser_->readFile(absPath);
    QApplication::restoreOverrideCursor();
}
