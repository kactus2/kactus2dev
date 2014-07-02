//-----------------------------------------------------------------------------
// File: filegeneraltab.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileGeneralTab is a widget to edit File's general settings.
//-----------------------------------------------------------------------------

#include "filegeneraltab.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QStringList>
#include <QVBoxLayout>

//-----------------------------------------------------------------------------
// Function: FileGeneralTab::FileGeneralTab()
//-----------------------------------------------------------------------------
FileGeneralTab::FileGeneralTab(LibraryInterface* handler,
							   QSharedPointer<Component> component,
							   QSharedPointer<File> file, 
							   QWidget *parent ):
QWidget(parent), 
file_(file),
nameEditor_(file, this),
editButton_(new QPushButton(tr("Edit"), this)),
runButton_(new QPushButton(tr("Run"), this)),
generalEditor_(this, file),
fileTypeEditor_(this, file),
buildCommand_(this, handler, component, file)
{	
    setupLayout();

    fileTypeEditor_.initialize();

    connect(editButton_, SIGNAL(clicked(bool)), this, SIGNAL(editFile()), Qt::UniqueConnection);
    connect(runButton_, SIGNAL(clicked(bool)), this, SIGNAL(runFile()), Qt::UniqueConnection);
	connect(&generalEditor_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
	connect(&fileTypeEditor_, SIGNAL(contentChanged()),	this, SLOT(onFileTypesChanged()), Qt::UniqueConnection);
	connect(&buildCommand_, SIGNAL(contentChanged()), this, SIGNAL(contentChanged()), Qt::UniqueConnection);
}

//-----------------------------------------------------------------------------
// Function: FileGeneralTab::~FileGeneralTab()
//-----------------------------------------------------------------------------
FileGeneralTab::~FileGeneralTab()
{

}

//-----------------------------------------------------------------------------
// Function: FileGeneralTab::isValid()
//-----------------------------------------------------------------------------
bool FileGeneralTab::isValid() const
{	
	// at least one file type has to be defined and the file name must be specified.
	return (fileTypeEditor_.isValid() && nameEditor_.isValid());
}

//-----------------------------------------------------------------------------
// Function: FileGeneralTab::refresh()
//-----------------------------------------------------------------------------
void FileGeneralTab::refresh()
{
	nameEditor_.refresh();
	generalEditor_.refresh();
	fileTypeEditor_.restore();
	buildCommand_.refresh();
}

//-----------------------------------------------------------------------------
// Function: FileGeneralTab::onFileTypesChanged()
//-----------------------------------------------------------------------------
void FileGeneralTab::onFileTypesChanged()
{
	// get the file types from the editor to file model
	QStringList fileTypes = fileTypeEditor_.items();
	file_->setAllFileTypes(fileTypes);
	emit contentChanged();
}

//-----------------------------------------------------------------------------
// Function: FileGeneralTab::showEvent()
//-----------------------------------------------------------------------------
void FileGeneralTab::showEvent( QShowEvent* event )
{
	QWidget::showEvent(event);
	emit helpUrlRequested("componenteditor/filegeneral.html");
}

//-----------------------------------------------------------------------------
// Function: FileGeneralTab::setupLayout()
//-----------------------------------------------------------------------------
void FileGeneralTab::setupLayout()
{    
    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QHBoxLayout* nameLayout = new QHBoxLayout();
    nameLayout->addWidget(&nameEditor_);
    nameLayout->addWidget(editButton_);
    nameLayout->addWidget(runButton_);

    topLayout->addLayout(nameLayout);

    topLayout->addWidget(&fileTypeEditor_);
    topLayout->addWidget(&generalEditor_);

    QGroupBox* buildBox = new QGroupBox(tr("Build command"), this);
    QVBoxLayout* buildLayout = new QVBoxLayout(buildBox);
    buildLayout->addWidget(&buildCommand_);
    buildLayout->setContentsMargins(0, 0, 0, 0);

    topLayout->addWidget(buildBox);
}
