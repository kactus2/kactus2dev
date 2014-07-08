/* 
 *	Created on:	10.6.2013
 *	Author:		Antti Kamppi
 *	File name:	vhdlimporteditor.cpp
 *	Project:		Kactus 2
*/

#include "vhdlimporteditor.h"
#include <common/widgets/FileSelector/fileselector.h>
#include <IPXACTmodels/generaldeclarations.h>
#include <common/widgets/vhdlParser/VhdlParserWidget.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QDesktopServices>

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor()
//-----------------------------------------------------------------------------
VhdlImportEditor::VhdlImportEditor(const QString& basePath,
	QSharedPointer<Component> component, 
	LibraryInterface* handler,
	QWidget *parent):
    QWidget(parent),
    splitter_(Qt::Vertical, this),
	basePath_(basePath),
    vhdlPath_(),
    parserWidget_(new VhdlParserWidget(basePath,component, &splitter_)),    
    modelParams_(new ModelParameterEditor(component, handler, &splitter_)),
    ports_(new PortsEditor(component, handler, &splitter_))
{
	Q_ASSERT(component);

	// CSV import/export is disabled in the wizard
	modelParams_->setAllowImportExport(false);
	ports_->setAllowImportExport(false);

    // Make the splitter handles visible. Handle 0 is always invisible.
    const int handles = 3;
    for ( int i = 1; i < handles; i++)
    {
        QSplitterHandle* handle = splitter_.handle(i);
        QVBoxLayout *layout = new QVBoxLayout(handle);
        layout->setSpacing(0);
        layout->setMargin(0);

        QFrame* line = new QFrame(handle);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        layout->addWidget(line);
    }

    // Connections between model parameter editor and vhdlParser.
	connect(modelParams_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(parserWidget_, SIGNAL(addModelParameter(QSharedPointer<ModelParameter>)),
		modelParams_, SLOT(addModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
	connect(parserWidget_, SIGNAL(removeModelParameter(QSharedPointer<ModelParameter>)),
		modelParams_, SLOT(removeModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
    connect(modelParams_, SIGNAL(parameterChanged(QSharedPointer<ModelParameter>)),
             parserWidget_, SLOT(editorChangedModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
    connect(modelParams_, SIGNAL(modelParameterRemoved(QSharedPointer<ModelParameter>)),
            parserWidget_, SLOT(editorRemovedModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);

    // Connections between port editor and vhdlParser.
	connect(ports_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(parserWidget_, SIGNAL(addPort(QSharedPointer<Port>)),
		ports_, SLOT(addPort(QSharedPointer<Port>)), Qt::UniqueConnection);
	connect(parserWidget_, SIGNAL(removePort(QSharedPointer<Port>)),
		ports_, SLOT(removePort(QSharedPointer<Port>)), Qt::UniqueConnection);
    connect(ports_, SIGNAL(lockedPortRemoved(QSharedPointer<Port>)),
		parserWidget_, SLOT(editorRemovedPort(QSharedPointer<Port>)), Qt::UniqueConnection);

    setupLayout();
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
    parserWidget_->initializeFileSelection();
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
// Function: setupLayout()
//-----------------------------------------------------------------------------
void VhdlImportEditor::setupLayout()
{
	QVBoxLayout* topLayout = new QVBoxLayout(this);    
    topLayout->addWidget(&splitter_);
    topLayout->setContentsMargins(0, 0, 0, 0);

    splitter_.addWidget(parserWidget_);
    splitter_.addWidget(modelParams_);
    splitter_.addWidget(ports_);
    splitter_.setStretchFactor(0, 1);
}