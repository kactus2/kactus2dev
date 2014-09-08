/* 
 *	Created on:	10.6.2013
 *	Author:		Antti Kamppi
 *	File name:	vhdlimporteditor.cpp
 *	Project:		Kactus 2
*/

#include "vhdlimporteditor.h"

#include <common/widgets/vhdlParser/VhdlParserWidget.h>

#include <editors/ComponentEditor/modelParameters/modelparametereditor.h>
#include <editors/ComponentEditor/ports/portseditor.h>

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
    parserWidget_(new VhdlParserWidget(basePath, component, &splitter_)),    
    modelParameterEditor_(new ModelParameterEditor(component, handler, &splitter_)),
    portEditor_(new PortsEditor(component, handler, &splitter_))
{
	Q_ASSERT(component);

	// CSV import/export is disabled in the wizard
	modelParameterEditor_->setAllowImportExport(false);
	portEditor_->setAllowImportExport(false);

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
	connect(modelParameterEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(parserWidget_, SIGNAL(addModelParameter(QSharedPointer<ModelParameter>)),
		modelParameterEditor_, SLOT(addModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
	connect(parserWidget_, SIGNAL(removeModelParameter(QSharedPointer<ModelParameter>)),
		modelParameterEditor_, SLOT(removeModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
    connect(modelParameterEditor_, SIGNAL(parameterChanged(QSharedPointer<ModelParameter>)),
             parserWidget_, SLOT(editorChangedModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
    connect(modelParameterEditor_, SIGNAL(modelParameterRemoved(QSharedPointer<ModelParameter>)),
            parserWidget_, SLOT(editorRemovedModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);

    // Connections between port editor and vhdlParser.
	connect(portEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(parserWidget_, SIGNAL(addPort(QSharedPointer<Port>)),
		portEditor_, SLOT(addPort(QSharedPointer<Port>)), Qt::UniqueConnection);
	connect(parserWidget_, SIGNAL(removePort(QSharedPointer<Port>)),
		portEditor_, SLOT(removePort(QSharedPointer<Port>)), Qt::UniqueConnection);
    connect(portEditor_, SIGNAL(lockedPortRemoved(QSharedPointer<Port>)),
		parserWidget_, SLOT(editorRemovedPort(QSharedPointer<Port>)), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ~VhdlImportEditor()
//-----------------------------------------------------------------------------
VhdlImportEditor::~VhdlImportEditor()
{

}

//-----------------------------------------------------------------------------
// Function: initializeFileSelection()
//-----------------------------------------------------------------------------
void VhdlImportEditor::initializeFileSelection()
{
    parserWidget_->initializeFileSelection();
}

//-----------------------------------------------------------------------------
// Function: checkEditorValidity()
//-----------------------------------------------------------------------------
bool VhdlImportEditor::checkEditorValidity() const
{
    return modelParameterEditor_->isValid() && portEditor_->isValid();
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
    splitter_.addWidget(modelParameterEditor_);
    splitter_.addWidget(portEditor_);
    splitter_.setStretchFactor(0, 1);
}
