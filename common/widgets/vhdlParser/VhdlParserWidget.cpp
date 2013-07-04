//-----------------------------------------------------------------------------
// File: VhdlParserWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.06.2013
//
// Description:
// Widget for viewing and parsing a vhdl file.
//-----------------------------------------------------------------------------

#include "VhdlParserWidget.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileDialog>
#include <QDesktopServices>
#include <QWizardPage>

#include <common/widgets/FileSelector/fileselector.h>
#include <models/generaldeclarations.h>
#include <models/component.h>
//-----------------------------------------------------------------------------
// Function: VhdlParserWidget()
//-----------------------------------------------------------------------------
VhdlParserWidget::VhdlParserWidget(const QString& basePath,	QSharedPointer<Component> component, 
    QWidget* parent):
    QWidget(parent),
    basePath_(basePath),
	vhdlParser_(new VhdlParser(this)),
	fileSelector_(new FileSelector(component, this)),
    editButton_(tr("Open editor"),this),
    refreshButton_(QIcon(":/icons/graphics/refresh.png"),"",this)
{
	Q_ASSERT(component);

	// only vhdl files are selected
	fileSelector_->addFilter("vhd");
	fileSelector_->addFilter("vhdl");
	connect(fileSelector_, SIGNAL(fileSelected(const QString&)),
		this, SLOT(onFileSelected(const QString&)), Qt::UniqueConnection);

    // Model parameter signals.
	connect(vhdlParser_, SIGNAL(addGeneric(QSharedPointer<ModelParameter>)),
		this, SIGNAL(addModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(removeGeneric(QSharedPointer<ModelParameter>)),
		this, SIGNAL(removeModelParameter(QSharedPointer<ModelParameter>)), Qt::UniqueConnection);
   

	// Port signals.
	connect(vhdlParser_, SIGNAL(addPort(QSharedPointer<Port>)),
		this, SIGNAL(addPort(QSharedPointer<Port>)), Qt::UniqueConnection);
	connect(vhdlParser_, SIGNAL(removePort(QSharedPointer<Port>)),
		this, SIGNAL(removePort(QSharedPointer<Port>)), Qt::UniqueConnection);
 

    // Pushbutton connections.
    connect(&refreshButton_, SIGNAL(clicked()),this, SLOT(onRefresh()), Qt::UniqueConnection);
    connect(&editButton_, SIGNAL(clicked()), this, SLOT(onOpenEditor()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: ~VhdlParserWidget()
//-----------------------------------------------------------------------------
VhdlParserWidget::~VhdlParserWidget() {
}

//-----------------------------------------------------------------------------
// Function: initializeFileSelection()
//-----------------------------------------------------------------------------
void VhdlParserWidget::initializeFileSelection() {
	fileSelector_->refresh();
}


//-----------------------------------------------------------------------------
// Function: onFileSelected()
//-----------------------------------------------------------------------------
void VhdlParserWidget::onFileSelected( const QString& filePath ) {
	if (filePath.isEmpty()) {
		return;
	}

	vhdlPath_ = General::getAbsolutePath(basePath_+"/", filePath);

	// if the absolute path can not be converted
	if (vhdlPath_.isEmpty()) {
		return;
	}

    vhdlParser_->parseFile(vhdlPath_);
    vhdlParser_->scrollToEntityBegin();   
}

//-----------------------------------------------------------------------------
// Function: onOpenEditor()
//-----------------------------------------------------------------------------
void VhdlParserWidget::onOpenEditor() 
{
    if ( !vhdlPath_.isEmpty() )
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(vhdlPath_));
    }
}

//-----------------------------------------------------------------------------
// Function: onRefresh()
//-----------------------------------------------------------------------------
void VhdlParserWidget::onRefresh() 
{
    if ( !vhdlPath_.isEmpty() )
    {
        vhdlParser_->parseFile(vhdlPath_);
        vhdlParser_->scrollToEntityBegin();
    }
}

//-----------------------------------------------------------------------------
// Function: editorChangedModelParameter()
//-----------------------------------------------------------------------------
void VhdlParserWidget::editorChangedModelParameter(QSharedPointer<ModelParameter> modelParam)
{
    vhdlParser_->editorChangedModelParameter(modelParam);
}

//-----------------------------------------------------------------------------
// Function: editorRemovedModelParameter()
//-----------------------------------------------------------------------------
void VhdlParserWidget::editorRemovedModelParameter(QSharedPointer<ModelParameter> modelParam)
{
    vhdlParser_->editorRemovedModelParameter(modelParam);
}

//-----------------------------------------------------------------------------
// Function: editorRemovedPort()
//-----------------------------------------------------------------------------
void VhdlParserWidget::editorRemovedPort(QSharedPointer<Port> port)
{
    vhdlParser_->editorRemovedPort(port);
}

//-----------------------------------------------------------------------------
// Function: setupLayout()
//-----------------------------------------------------------------------------
void VhdlParserWidget::setupLayout()
{
	QVBoxLayout* topLayout = new QVBoxLayout(this);    
    topLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout* selectorLayout = new QHBoxLayout();
    QVBoxLayout* vhdlLayout = new QVBoxLayout();
 
   QLabel* vhdltopLabel = new QLabel(this);
    vhdltopLabel->setText("Top-level VHDL file:");
    selectorLayout->addWidget(vhdltopLabel);
	selectorLayout->addWidget(fileSelector_,1);
	selectorLayout->addWidget(&editButton_);    
	selectorLayout->addWidget(&refreshButton_);    

	vhdlLayout->addWidget(vhdlParser_);

    topLayout->addLayout(selectorLayout);
    topLayout->addLayout(vhdlLayout);
}
