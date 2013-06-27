//-----------------------------------------------------------------------------
// File: FileViewer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 26.06.2013
//
// Description:
// Widget for viewing files in a component fileset
//-----------------------------------------------------------------------------

#include "FileViewer.h"

#include <QGridLayout>
#include <QLabel>
#include <QSharedPointer>
#include <QTextEdit>
#include <QString>
#include <QMessageBox>
#include <QTextStream>

#include <common/widgets/FileSelector/fileselector.h>
#include <models/component.h>

//-----------------------------------------------------------------------------
// Function: FileViewer()
//-----------------------------------------------------------------------------
FileViewer::FileViewer(QSharedPointer<Component> component, QString const& basePath, QWidget* parent):
    QWidget(parent), basePath_(basePath), fileSelector_(new FileSelector(component, this)), editor_(new QTextEdit(this))
{
    editor_->setReadOnly(true);

    connect(fileSelector_, SIGNAL(fileSelected(const QString&)), 
        this, SLOT(onFileSelected(const QString&)),Qt::UniqueConnection);

    setupLayout();
}
  
//-----------------------------------------------------------------------------
// Function: ~FileViewer()
//-----------------------------------------------------------------------------
FileViewer::~FileViewer()
{
}

void FileViewer::refresh()
{
    fileSelector_->refresh();
}


//-----------------------------------------------------------------------------
// Function: onFileSelected()
//-----------------------------------------------------------------------------
void FileViewer::onFileSelected(const QString& filePath)
{
	if (filePath.isEmpty()) {
		editor_->setText(QString());
	}

    QString absPath = basePath_+"/"+filePath;
    QFile file(absPath);
    if ( !file.open(QIODevice::ReadOnly) )
    {
        QMessageBox::information(this, "Error", file.errorString()+": "+absPath);
        return;
    }

    QTextStream stream(&file);
    editor_->setText(stream.readAll());
    file.close();
}

//-----------------------------------------------------------------------------
// Function: setupLayout()
//-----------------------------------------------------------------------------
void FileViewer::setupLayout()
{
    QGridLayout* fileLayout = new QGridLayout(this);
    fileLayout->addWidget(new QLabel("Select file:" ,this),0,0,1,1);
    fileLayout->addWidget(fileSelector_,0,1,1,1);
    fileLayout->addWidget(editor_,1,0,1,2);
    fileLayout->setColumnStretch(1,1);
    
}
