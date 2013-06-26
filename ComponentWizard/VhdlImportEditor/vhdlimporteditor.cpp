/* 
 *	Created on:	10.6.2013
 *	Author:		Antti Kamppi
 *	File name:	vhdlimporteditor.cpp
 *	Project:		Kactus 2
*/

#include "vhdlimporteditor.h"
#include <common/widgets/FileSelector/fileselector.h>
#include <models/generaldeclarations.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QLabel>
#include <QFileDialog>

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor()
//-----------------------------------------------------------------------------
VhdlImportEditor::VhdlImportEditor(const QString& basePath,
	QSharedPointer<Component> component, 
	LibraryInterface* handler,
	QWidget *parent):
    QWidget(parent),
   splitter_(Qt::Vertical, this),
	vhdlParser_(new VhdlParser(&splitter_)),
	basePath_(basePath),
	fileSelector_(new FileSelector(component, &splitter_)),
    modelParams_(new ModelParameterEditor(component, handler, &splitter_)),
    ports_(new PortsEditor(component, handler, false, &splitter_))
{
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
	connect(vhdlParser_, SIGNAL(removeGeneric(QSharedPointer<ModelParameter>)),
		modelParams_, SLOT(removeModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
    connect(modelParams_, SIGNAL(parameterChanged(QSharedPointer<ModelParameter>)),
             vhdlParser_, SLOT(editorChangedModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
    connect(modelParams_, SIGNAL(modelParameterRemoved(QSharedPointer<ModelParameter>)),
            vhdlParser_, SLOT(editorRemovedModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);

	connect(ports_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(addPort(QSharedPointer<Port>)),
		ports_, SLOT(addPort(QSharedPointer<Port>)), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(removePort(QSharedPointer<Port>)),
		ports_, SLOT(removePort(QSharedPointer<Port>)), Qt::UniqueConnection);
    connect(ports_, SIGNAL(lockedPortRemoved(QSharedPointer<Port>)),
		vhdlParser_, SLOT(editorRemovedPort(QSharedPointer<Port>)), Qt::UniqueConnection);

	// The layout on the left side of the GUI displaying the file selector and
	// VHDL source code.
    
    QWidget* topWidget = new QWidget(this);
    QVBoxLayout* vhdlLayout = new QVBoxLayout(topWidget);
    QHBoxLayout* selectorLayout = new QHBoxLayout();

    QLabel* vhdltopLabel = new QLabel(this);
    vhdltopLabel->setText("Top-level VHDL file:");
    selectorLayout->addWidget(vhdltopLabel);
	selectorLayout->addWidget(fileSelector_,1);
    vhdlLayout->addLayout(selectorLayout);
	vhdlLayout->addWidget(vhdlParser_);

	// The layout on the right side of the GUI displaying the editors.
	//QVBoxLayout* editorLayout = new QVBoxLayout();
	//editorLayout->addWidget(modelParams_);
	//editorLayout->addWidget(ports_);

	// The top layout which owns other layouts
	QVBoxLayout* topLayout = new QVBoxLayout(this);
    
	//topLayout->addLayout(vhdlLayout);
    topLayout->addWidget(&splitter_);
	//topLayout->addLayout(editorLayout);

    splitter_.addWidget(topWidget);
    splitter_.addWidget(modelParams_);
    splitter_.addWidget(ports_);
    splitter_.setStretchFactor(0, 1);
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
