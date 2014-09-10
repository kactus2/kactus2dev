/* 
 *	Created on:	10.6.2013
 *	Author:		Antti Kamppi
 *	File name:	vhdlimporteditor.cpp
 *	Project:		Kactus 2
*/

#include "vhdlimporteditor.h"

#include <common/widgets/FileSelector/fileselector.h>
#include <common/widgets/vhdlParser/VhdlParser.h>

#include <editors/ComponentEditor/modelParameters/modelparametereditor.h>
#include <editors/ComponentEditor/ports/portseditor.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QDesktopServices>

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::VhdlImportEditor()
//-----------------------------------------------------------------------------
VhdlImportEditor::VhdlImportEditor(QSharedPointer<Component> component, 
	LibraryInterface* handler,
	QWidget *parent):
    QWidget(parent),
    splitter_(Qt::Vertical, this),
	componentXmlPath_(handler->getPath(*component->getVlnv())),
    selectedSourceFile_(),
    fileSelector_(new FileSelector(component, this)),
    editButton_(new QPushButton(tr("Open editor"), this)),
    refreshButton_(new QPushButton(QIcon(":/icons/common/graphics/refresh.png"), "", this)),
    vhdlParser_(new VhdlParser(this)),
    modelParameterEditor_(new ModelParameterEditor(component, handler, &splitter_)),
    portEditor_(new PortsEditor(component, handler, &splitter_))
{
	Q_ASSERT(component);

	// CSV import/export is disabled in the wizard.
	modelParameterEditor_->setAllowImportExport(false);
	portEditor_->setAllowImportExport(false);

    fileSelector_->addFilter("vhd");
    fileSelector_->addFilter("vhdl");

    // Connections between model parameter editor and vhdlParser.
	connect(modelParameterEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(addGeneric(QSharedPointer<ModelParameter>)),
		modelParameterEditor_, SLOT(addModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(removeGeneric(QSharedPointer<ModelParameter>)),
		modelParameterEditor_, SLOT(removeModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
    connect(modelParameterEditor_, SIGNAL(parameterChanged(QSharedPointer<ModelParameter>)),
             vhdlParser_, SLOT(editorChangedModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
    connect(modelParameterEditor_, SIGNAL(modelParameterRemoved(QSharedPointer<ModelParameter>)),
            vhdlParser_, SLOT(editorRemovedModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);

    // Connections between port editor and vhdlParser.
	connect(portEditor_, SIGNAL(contentChanged()),
		this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(addPort(QSharedPointer<Port>)),
		portEditor_, SLOT(addPort(QSharedPointer<Port>)), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(removePort(QSharedPointer<Port>)),
		portEditor_, SLOT(removePort(QSharedPointer<Port>)), Qt::UniqueConnection);
    connect(portEditor_, SIGNAL(lockedPortRemoved(QSharedPointer<Port>)),
		vhdlParser_, SLOT(editorRemovedPort(QSharedPointer<Port>)), Qt::UniqueConnection);

    connect(fileSelector_, SIGNAL(fileSelected(const QString&)),
        this, SLOT(onFileSelected(const QString&)), Qt::UniqueConnection);

    connect(refreshButton_, SIGNAL(clicked()),this, SLOT(onRefresh()), Qt::UniqueConnection);
    connect(editButton_, SIGNAL(clicked()), this, SLOT(onOpenEditor()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::~VhdlImportEditor()
//-----------------------------------------------------------------------------
VhdlImportEditor::~VhdlImportEditor()
{

}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::initializeFileSelection()
//-----------------------------------------------------------------------------
void VhdlImportEditor::initializeFileSelection()
{
    fileSelector_->refresh();
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::checkEditorValidity()
//-----------------------------------------------------------------------------
bool VhdlImportEditor::checkEditorValidity() const
{
    return modelParameterEditor_->isValid() && portEditor_->isValid();
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::onFileSelected()
//-----------------------------------------------------------------------------
void VhdlImportEditor::onFileSelected( const QString& filePath )
{
    if (filePath.isEmpty())
    {
        return;
    }

    selectedSourceFile_ = General::getAbsolutePath(componentXmlPath_, filePath);

    onRefresh();
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::onOpenEditor()
//-----------------------------------------------------------------------------
void VhdlImportEditor::onOpenEditor() 
{
    if (!selectedSourceFile_.isEmpty())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(selectedSourceFile_));
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::onRefresh()
//-----------------------------------------------------------------------------
void VhdlImportEditor::onRefresh() 
{
    if (!selectedSourceFile_.isEmpty())
    {
        vhdlParser_->parseFile(selectedSourceFile_);
        vhdlParser_->scrollToEntityBegin();
    }
}

//-----------------------------------------------------------------------------
// Function: VhdlImportEditor::setupLayout()
//-----------------------------------------------------------------------------
void VhdlImportEditor::setupLayout()
{
    // Make the splitter handles visible by creating lines. Handle 0 is always invisible.
    const int handles = 3;
    for ( int i = 1; i < handles; i++)
    {
        QSplitterHandle* handle = splitter_.handle(i);

        QVBoxLayout* layout = new QVBoxLayout(handle);
        layout->setSpacing(0);
        layout->setMargin(0);

        QFrame* line = new QFrame(handle);
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        layout->addWidget(line);
    }

	QVBoxLayout* topLayout = new QVBoxLayout(this);    
    topLayout->addWidget(&splitter_);
    topLayout->setContentsMargins(0, 0, 0, 0);

    QWidget* sourceWidget = new QWidget(this);
    QVBoxLayout* sourceLayout = new QVBoxLayout(sourceWidget);    

    sourceLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* selectorLayout = new QHBoxLayout();
    QLabel* vhdlLabel = new QLabel(tr("Top-level VHDL file:"), this);
    selectorLayout->addWidget(vhdlLabel);
    selectorLayout->addWidget(fileSelector_, 1);
    selectorLayout->addWidget(editButton_);    
    selectorLayout->addWidget(refreshButton_);    

    sourceLayout->addLayout(selectorLayout);
    sourceLayout->addWidget(vhdlParser_);

    splitter_.addWidget(sourceWidget);
    splitter_.addWidget(modelParameterEditor_);
    splitter_.addWidget(portEditor_);
    splitter_.setStretchFactor(0, 1);
}
