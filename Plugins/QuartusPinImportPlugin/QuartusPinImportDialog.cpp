//-----------------------------------------------------------------------------
// File: QuartusPinImportDialog.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 20.11.2013
//
// Description:
// Dialog for file selection and pin import.
//-----------------------------------------------------------------------------

#include "QuartusPinImportDialog.h"
#include "QuartusPinSyntax.h"

#include <common/widgets/FileSelector/fileselector.h>
#include <library/LibraryManager/libraryinterface.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QTime>
#include <QDate>
#include <QLabel>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTextStream>
#include <QTextBlock>
#include <QFile>
#include <QScrollBar>

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::QuartusPinImportDialog()
//-----------------------------------------------------------------------------
QuartusPinImportDialog::QuartusPinImportDialog(QSharedPointer<Component> component,
    LibraryInterface* libraryHandler,
    QWidget *parent)
    : QDialog(parent),
    handler_(libraryHandler),
    component_(component),
    fileSelector_(new FileSelector(component, this)),
    output_(new QPlainTextEdit(this)),
    ports_(),
    noticeFormat_(),
    errorFormat_()
{
    setWindowTitle("Quartus Pin Import");

    fileSelector_->addFilter("pin");

    output_->setLineWrapMode(QPlainTextEdit::NoWrap);
    output_->setMaximumBlockCount(10000);
    output_->setReadOnly(true);

    noticeFormat_.setForeground(QBrush(QColor("black")));
    errorFormat_.setForeground(QBrush(QColor("red")));

    connect(fileSelector_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onFileSelected()), Qt::UniqueConnection);

    setupLayout();
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::~QuartusPinImportDialog()
//-----------------------------------------------------------------------------
QuartusPinImportDialog::~QuartusPinImportDialog()
{
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::onRunClicked()
//-----------------------------------------------------------------------------
void QuartusPinImportDialog::onSaveClicked()
{
    if (fileSelector_->currentText().isEmpty())
    {
        return;
    }

    accept();
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::onFileSelected()
//-----------------------------------------------------------------------------
void QuartusPinImportDialog::onFileSelected()
{
    if (fileSelector_->currentText().isEmpty())
    {
        return;
    }
    ports_.clear();

    // Move text cursor to end of output.
    QTextCursor cursor = output_->textCursor();
    if(!cursor.atEnd())
    {
        cursor.movePosition(QTextCursor::End);
        output_->setTextCursor(cursor);
    }

    printNotice(QString("---------- Quartus II Pin Import ----------"));    
    printNotice(QString(tr("Generation started ") + QDate::currentDate().toString("dd.MM.yyyy") + 
        " " + QTime::currentTime().toString()));

    // Open pin file for reading.
    QString filePath = fileSelector_->currentText();
    QString basePath = handler_->getPath(*component_->getVlnv());
    QString absFilePath = QFileInfo(basePath).absolutePath() + "/" + filePath;    

    QFile* file = new QFile(absFilePath);
    if (!file->open(QIODevice::ReadOnly))
    {
        printError(tr("Failed to open file %1.").arg(absFilePath));
        return;
    }

    // Import filestream and statistics.
    QTextStream fileStream(file);    
    PortImportStatistics statistics;

    readFile(fileStream, statistics);

    file->close();   

    // Print import summary.
    printSummary(statistics);

    // Scroll to end.
    output_->verticalScrollBar()->setSliderPosition(output_->verticalScrollBar()->maximum());
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::printNotice()
//-----------------------------------------------------------------------------
void QuartusPinImportDialog::printNotice(QString const& text)
{   
    output_->textCursor().insertText(text, noticeFormat_);
    output_->textCursor().insertBlock();
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::printError()
//-----------------------------------------------------------------------------
void QuartusPinImportDialog::printError(QString const& text)
{
    output_->textCursor().insertText(text, errorFormat_);
    output_->textCursor().insertBlock();
    emit printMessage(text);
}


//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::setupLayout()
//-----------------------------------------------------------------------------
void QuartusPinImportDialog::setupLayout()
{
    QVBoxLayout* topLayout = new QVBoxLayout(this);

    QGroupBox* fileGroup = new QGroupBox(tr("Select Quartus II pin file (.pin)"), this);    
    QHBoxLayout* fileLayout = new QHBoxLayout(fileGroup);
    fileLayout->addWidget(fileSelector_);
    topLayout->addWidget(fileGroup);

    QGroupBox* outputGroup = new QGroupBox(tr("Output"), this);
    QHBoxLayout* outputLayout = new QHBoxLayout(outputGroup);
    outputLayout->addWidget(output_);
    topLayout->addWidget(outputGroup, 1);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* runButton = new QPushButton(tr("Save"));
    QPushButton* cancelButton = new QPushButton(tr("Cancel"));
    buttonLayout->addStretch();
    buttonLayout->addWidget(runButton, 0, Qt::AlignRight);
    buttonLayout->addWidget(cancelButton, 0, Qt::AlignRight);
    topLayout->addLayout(buttonLayout);

    connect(runButton, SIGNAL(clicked(bool)),
        this, SLOT(onSaveClicked()), Qt::UniqueConnection);    
    connect(cancelButton, SIGNAL(clicked(bool)),
        this, SLOT(reject()), Qt::UniqueConnection);

    setMinimumWidth(400);
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::getPorts()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Port> > QuartusPinImportDialog::getPorts() const
{
    return ports_;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::checkConflicts()
//-----------------------------------------------------------------------------
bool QuartusPinImportDialog::isPortModified(QString const& portName, 
    General::Direction portDirection, QString const& usage)
{
    bool changed = false;

    if (component_->getPortDirection(portName) != portDirection)
    {        
        QString oldDir = General::direction2Str(component_->getPortDirection(portName));
        QString newDir = General::direction2Str(portDirection);

        printError(tr("Direction in port %1 has changed from '%2' to '%3'.").arg(portName, oldDir, newDir));
        changed = true;
    }

    if (component_->getPortWidth(portName) != 1)
    {
        printError(tr("Port %1 width has changed from %2 to %3.").arg(portName, 
            QString::number(component_->getPortWidth(portName)), "1"));
        changed = true;
    }

    if (QString::compare(component_->getPort(portName)->getDescription(), usage, Qt::CaseInsensitive) != 0)
    {
        printError(tr("Port %1 usage has changed from \"%2\" to \"%3\".").arg(portName, 
           component_->getPort(portName)->getDescription(), usage));
        changed = true;
    }

    return changed;
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::checkExistingPorts()
//-----------------------------------------------------------------------------
unsigned int QuartusPinImportDialog::removedPortCount()
{
    QStringList oldPorts = component_->getPortNames();

    foreach(QSharedPointer<Port> port, ports_)
    {
        foreach(QString oldPort, oldPorts)
        {
            if (port->getName() == oldPort)
            {
                oldPorts.removeAll(oldPort);
                break;
            }
        }
    }

    foreach(QString oldPort, oldPorts)
    {
        printError(tr("Port %1 was not found in the file and will be removed.").arg(oldPort));        
    }

    return oldPorts.size();
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::readFile()
//-----------------------------------------------------------------------------
void QuartusPinImportDialog::readFile(QTextStream& fileStream, PortImportStatistics& statistics)
{
    while(!fileStream.atEnd())
    {
        if (QuartusPinSyntax::pinDef.exactMatch(fileStream.readLine()))
        {
            QString portUsage = QuartusPinSyntax::pinDef.cap(QuartusPinSyntax::PINUSAGE);
            QString portName = QuartusPinSyntax::pinDef.cap(QuartusPinSyntax::LOCATION);
            QString direction = QuartusPinSyntax::pinDef.cap(QuartusPinSyntax::DIRECTION);
            General::Direction portDirection = General::INOUT;
            if (direction.compare("input", Qt::CaseInsensitive) == 0)
            {
                portDirection = General::IN;
                statistics.inPorts++;
            }
            else if (direction.compare("output", Qt::CaseInsensitive) == 0)
            {
                portDirection = General::OUT;
                statistics.outPorts++;
            }
            else if (direction.compare("bidir", Qt::CaseInsensitive) == 0)
            {
                portDirection = General::INOUT;
                statistics.otherPorts++;

            }
            else if (direction.compare("power", Qt::CaseInsensitive) == 0)
            {
                statistics.powerPorts++;
            }
            else if (direction.compare("gnd", Qt::CaseInsensitive) == 0)
            {
                statistics.gndPorts++;
            }
            else
            {
                portDirection = General::DIRECTION_PHANTOM;
                statistics.otherPorts++;
            }

            statistics.portCount++;

            QSharedPointer<Port> port(new Port(portName, portDirection, 0, 0, "", "", "", portUsage));
            ports_.append(port);

            if (component_->hasPort(portName))
            {
                if (isPortModified(portName, portDirection, portUsage))
                {
                    statistics.modifiedPorts++;
                }
            }
            else
            {
                printNotice(tr("Found port %1.").arg(portName));
                statistics.newPorts++;
            }
        }
    }
    
    statistics.removedPorts = removedPortCount();
}

//-----------------------------------------------------------------------------
// Function: QuartusPinImportDialog::printSummary()
//-----------------------------------------------------------------------------
void QuartusPinImportDialog::printSummary(PortImportStatistics &statistics)
{
    printNotice(QString("========== Quartus II Pin Import Summary =========="));
    printNotice(QString("User I/0: %1 + %2 = %3").arg(QString::number(statistics.inPorts), 
        QString::number(statistics.outPorts), 
        QString::number(statistics.inPorts + statistics.outPorts)));
    printNotice(QString("Power + Gnd: %1 + %2 = %3").arg(QString::number(statistics.powerPorts),
        QString::number(statistics.gndPorts), 
        QString::number(statistics.powerPorts + statistics.gndPorts)));
    printNotice(QString("Other: %1").arg(QString::number(statistics.otherPorts)));
    printNotice(QString("Total: %1").arg(QString::number(statistics.portCount)));

    QString importHeader("========== Quartus II Pin Import Completed ==========");
    printNotice(importHeader);
    emit printMessage(importHeader);

    QString newSummary(tr("New ports: ") + QString::number(statistics.newPorts));
    printNotice(newSummary);
    if (statistics.newPorts > 0)
    {
        emit printMessage(newSummary);
    }
    
    QString modifiedSummary(tr("Modified ports: ") + QString::number(statistics.modifiedPorts));    
    printNotice(modifiedSummary);
    if (statistics.modifiedPorts > 0)
    {
        emit printMessage(modifiedSummary);
    }    

    QString removedSummary(tr("Removed ports: ") + QString::number(statistics.removedPorts));
    printNotice(removedSummary);
    if (statistics.removedPorts > 0)
    {
        emit printMessage(removedSummary);
    }
}