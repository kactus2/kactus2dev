//-----------------------------------------------------------------------------
// File: PadsPartGeneratorDialog
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.11.2013
//
// Description:
// Dialog for PADS part generation settings.
//-----------------------------------------------------------------------------

#include "PadsPartGeneratorDialog.h"
#include "PadsPartGenerator.h"
#include "PadsAsciiSyntax.h"

#include <LibraryManager/vlnv.h>
#include <LibraryManager/libraryinterface.h>
#include <models/abstractiondefinition.h>
#include <models/businterface.h>
#include <models/generaldeclarations.h>
#include <models/file.h>
#include <models/fileset.h>

#include <QApplication>
#include <QTextCharFormat>
#include <QComboBox>
#include <QDateTime>
#include <QFile>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QRadioButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextStream>
#include <QMap>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollBar>
#include <QVBoxLayout>

namespace
{
    // Unit indexes in unitSelector_.
    enum UNIT_INDEX
    {
        IMPERIAL = 0,
        METRIC
    };

    // Author, VLNV and plugin version are added as default attributes.
    const int PREGEN_ATTRS = 6;

    // Default file set name.
    const QString DEFAULT_FILESET = "PadsFiles";

    // Default gate name.
    const QString DEFAULT_GATENAME = "Default_Gate";

    // Dialog settings.
    const int MINIMUM_WIDTH = 500;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::PadsPartGeneratorDialog()
//-----------------------------------------------------------------------------
PadsPartGeneratorDialog::PadsPartGeneratorDialog(LibraryInterface* libIf,
    QSharedPointer<const Component> component, const QString& generatorName, 
    const QString& generatorVersion, QWidget* parent)
    : QDialog(parent),
    libHandler_(libIf),
    component_(component),
    generatorName_(generatorName),
    generatorVersion_(generatorVersion),
    fileSetSelector_(new QComboBox(this)),
    nameEditor_(new QLineEdit(this)),
    unitSelector_(new QComboBox(this)),
    familyEditor_(new QLineEdit(this)),
    multiGateButton_(new QRadioButton(tr("Create one gate per bus definition"), this)),
    singleGateButton_(new QRadioButton(tr("Create single gate"), this)),
    gateNameEditor_(new QLineEdit(this)),
    preview_(new QPlainTextEdit(this)),
    gateGenerationType_(MULTIPLEGATES),
    gates_(),
    okFormat_(),
    errorFormat_()
{
    setWindowTitle(generatorName_);

    // Part name editor.
    QRegExpValidator* nameValidator = new QRegExpValidator(QRegExp("\\w{1,40}", Qt::CaseInsensitive), this); 
    nameEditor_->setValidator(nameValidator);    
    nameEditor_->setText(component_->getVlnv()->getName());    
    connect(nameEditor_, SIGNAL(textChanged(const QString&)),
        this, SLOT(onNameChanged()), Qt::UniqueConnection);

    // File set selector.
    fileSetSelector_->setEditable(false);
    fileSetSelector_->addItem(tr("<Create new fileset>"));
    fileSetSelector_->setCurrentIndex(0);
    
    // Add file sets to selector.
    foreach(QString fileSetName, component_->getFileSetNames())
    {
        fileSetSelector_->addItem(fileSetName);
        // If default file set is found, set it as selection.
        if (QString::compare(fileSetName, DEFAULT_FILESET) == 0)
        {
            fileSetSelector_->setCurrentIndex(fileSetSelector_->count() - 1);
        }
    }        

    // Part measurement unit selection.
    unitSelector_->setEditable(false);
    unitSelector_->addItem(tr("Imperial (mils)"));
    unitSelector_->addItem(tr("Metric (mm)"));
    unitSelector_->setCurrentIndex(IMPERIAL);
    connect(unitSelector_, SIGNAL(currentIndexChanged(int)),
        this, SLOT(onUnitChanged()), Qt::UniqueConnection);

    // Part logic family editor.
    QRegExpValidator* familyValidator = new QRegExpValidator(QRegExp("\\w{3}", Qt::CaseInsensitive), this); 
    familyEditor_->setValidator(familyValidator);
    familyEditor_->setText("MOS");
    connect(familyEditor_, SIGNAL(textChanged(const QString&)),
        this, SLOT(onFamilyChanged()), Qt::UniqueConnection);

    // Gate generation type.
    multiGateButton_->setChecked(true);
    connect(multiGateButton_, SIGNAL(toggled(bool)),
        this, SLOT(onGateSelectionChanged()), Qt::UniqueConnection);

    // Gate name editor.
    QRegExpValidator* gateNameValidator = new QRegExpValidator(QRegExp("\\w{1,40}", Qt::CaseInsensitive), this); 
    gateNameEditor_->setValidator(gateNameValidator);  
    gateNameEditor_->setText(DEFAULT_GATENAME);  
    gateNameEditor_->setEnabled(false);
    connect(gateNameEditor_, SIGNAL(textChanged(const QString&)),
        this, SLOT(onGateNameChanged()), Qt::UniqueConnection);

    // Part preview.
    preview_->setReadOnly(true);
    preview_->setLineWrapMode(QPlainTextEdit::NoWrap);

    setupLayout();

    // Formatting options.
    okFormat_.setForeground(QBrush("black"));
    errorFormat_.setForeground(QBrush("red"));
    
    refreshPreview();      
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::~PadsPartGeneratorDialog()
//-----------------------------------------------------------------------------
PadsPartGeneratorDialog::~PadsPartGeneratorDialog()
{

}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::getPartName()
//-----------------------------------------------------------------------------
QString PadsPartGeneratorDialog::getPartName() const
{
    return nameEditor_->text();
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::getTargetFile()
//-----------------------------------------------------------------------------
QString PadsPartGeneratorDialog::getFileSetName() const
{
    if (fileSetSelector_->currentIndex() != 0)
    {
        return fileSetSelector_->currentText();
    } 
    else
    {
        return DEFAULT_FILESET;
    }
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::getPreview()
//-----------------------------------------------------------------------------
QString PadsPartGeneratorDialog::getPreviewText()
{
    return preview_->toPlainText();
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::getGategenerationType()
//-----------------------------------------------------------------------------
PadsPartGeneratorDialog::gateGenerationType PadsPartGeneratorDialog::getGategenerationType() const
{
    return gateGenerationType_;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::updatePreview()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::refreshPreview()
{
    QApplication::setOverrideCursor(Qt::WaitCursor);
    gates_.clear();    
    preview_->clear();

    insertLine(PadsAsciiSyntax::PART_FILE_BEGIN, preview_->textCursor());

    generateHeader();
    generateGates();

    insertLine(PadsAsciiSyntax::PADS_FILE_END, preview_->textCursor());

    QApplication::restoreOverrideCursor();     

    // Scroll to beginning.
    QTextCursor cursor = preview_->textCursor();
    cursor.movePosition(QTextCursor::Start);
    preview_->setTextCursor(cursor);
    preview_->verticalScrollBar()->setSliderPosition(0);    
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::onRunClicked()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::onRunClicked()
{
    QStringList errorList;
    
    if (!nameEditor_->hasAcceptableInput())
    {
        errorList << tr("Part name is invalid.");
    }

    if (!familyEditor_->hasAcceptableInput())
    {
        errorList << tr("Part logic family is invalid.");
    }

    if (gateGenerationType_ == SINGLEGATE && !gateNameEditor_->hasAcceptableInput())
    {
        errorList << tr("Gate name is invalid.");
    }

    if (!errorList.isEmpty())
    {
        QMessageBox msgBox(QMessageBox::Warning, generatorName_, 
            tr("The generator cannot run because of the following error(s):\n") + errorList.join('\n'), 
            QMessageBox::Ok, this);
        msgBox.exec();
        return;
    } 
    else
    {
        accept();
    }    
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::onNameChanged()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::onNameChanged()
{    
    QTextCursor cursor = preview_->document()->find(PadsAsciiSyntax::PART_HEADER_1ST_EXP);
    if (!cursor.isNull())
    {
        QString newName = nameEditor_->text().toUpper();
        if (!newName.isEmpty())
        {
            // Replace old name with new name in the header.
            QStringList line = cursor.block().text().split(PadsAsciiSyntax::SEPARATOR, QString::SkipEmptyParts);
            line.replace(PadsAsciiSyntax::NAME, newName);
            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);        
            cursor.insertText(line.join(PadsAsciiSyntax::SEPARATOR));

            // Replace old name with new name in gate names.
            foreach(int row, gates_.keys())
            {
                QTextBlock gateBlock = preview_->document()->findBlockByLineNumber(row);
                cursor.setPosition(gateBlock.position());    
                //QString gate = gateBlock.text();
                int index = gateBlock.text().indexOf(gates_.value(row), 0, Qt::CaseInsensitive);  
                if (index != -1)
                {
                    cursor.setPosition(cursor.position() + index, QTextCursor::KeepAnchor);
                    cursor.insertText(newName + "_");
                }                                  
            }      
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::onUnitChanged()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::onUnitChanged()
{
    QTextCursor cursor = preview_->document()->find(PadsAsciiSyntax::PART_HEADER_1ST_EXP);
    if (!cursor.isNull())
    {
        // Replace old unit with new in the header.
        QStringList line = cursor.block().text().split(PadsAsciiSyntax::SEPARATOR, QString::SkipEmptyParts);
        if (unitSelector_->currentIndex() == IMPERIAL)
        {
            line.replace(PadsAsciiSyntax::UNITS, PadsAsciiSyntax::IMPERIAL);
        } 
        else //if (unitSelector_->currentIndex() == METRIC)
        {
            line.replace(PadsAsciiSyntax::UNITS, PadsAsciiSyntax::METRIC);
        }                
        cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
        cursor.insertText(line.join(PadsAsciiSyntax::SEPARATOR));
    }
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::onFamilyChanged()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::onFamilyChanged()
{
    if (familyEditor_->hasAcceptableInput())
    {
        // Replace old logic family with new in the header.
        QTextCursor cursor = preview_->document()->find(PadsAsciiSyntax::PART_HEADER_1ST_EXP);
        if (!cursor.isNull())
        {
            QStringList line = cursor.block().text().split(PadsAsciiSyntax::SEPARATOR, QString::SkipEmptyParts);
            line.replace(PadsAsciiSyntax::LOGFAMILY, familyEditor_->text());

            cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
            cursor.insertText(line.join(PadsAsciiSyntax::SEPARATOR));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::onGateSelectionChanged()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::onGateSelectionChanged()
{
    gateGenerationType oldSelection = gateGenerationType_;

    if (multiGateButton_->isChecked())
    {
        gateGenerationType_ = MULTIPLEGATES;
    } 
    else
    {
        gateGenerationType_ = SINGLEGATE;
    }

    // Enable/disable gate name editing.
    gateNameEditor_->setEnabled(singleGateButton_->isChecked());

    // Regenerate gates.
    if (gateGenerationType_ != oldSelection)
    {
        gates_.clear();
        generateGates();
    }
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::onGateNameChanged()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::onGateNameChanged()
{
    Q_ASSERT(gateGenerationType_ == SINGLEGATE);

    if (!gateNameEditor_->text().isEmpty() && gates_.size() > 0)
    {
        // Replace old gate name with new one.
        int row = gates_.keys().first();
        QString newName = gateNameEditor_->text();
        QTextCursor cursor = preview_->textCursor();
        QTextBlock gateBlock = preview_->document()->findBlockByLineNumber(row);
        QString block = gateBlock.text();
        QString gate = gates_.value(row);
        int index = gateBlock.text().lastIndexOf(gates_.value(row), -1, Qt::CaseInsensitive);
        if (index != -1)
        {
            cursor.setPosition(gateBlock.position() + index);        
            cursor.movePosition(QTextCursor::NextWord, QTextCursor::KeepAnchor);
            cursor.insertText(newName.toUpper());
            gates_.insert(row, newName);
        }        
    }
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::setupLayout()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::setupLayout()
{
    QGroupBox* kactusGroup = new QGroupBox(tr("Fileset options"), this);
    QFormLayout* kactusLayout = new QFormLayout(kactusGroup);
    kactusLayout->addRow(tr("Select target fileset"), fileSetSelector_);

    QGroupBox* headerGenerationGroup = new QGroupBox(tr("Header generation options"), this);
    QFormLayout* headerLayout = new QFormLayout(headerGenerationGroup);

    headerLayout->addRow(tr("Part name"), nameEditor_);
    headerLayout->addRow(tr("Coordinate units"), unitSelector_);
    headerLayout->addRow(tr("Logic family"), familyEditor_);

    QGroupBox* gateGenerationGroup = new QGroupBox(tr("Gate options"), this);
    QVBoxLayout* gateGenerationLayout = new QVBoxLayout(gateGenerationGroup);
    gateGenerationLayout->addWidget(multiGateButton_);
    gateGenerationLayout->addWidget(singleGateButton_);
    QFormLayout* gateNameLayout = new QFormLayout();
    gateNameLayout->addRow(tr("Gate name"), gateNameEditor_);
    gateGenerationLayout->addLayout(gateNameLayout);

    QGroupBox* previewGroup = new QGroupBox(tr("Preview"), this);
    QVBoxLayout* previewLayout = new QVBoxLayout(previewGroup);
    previewLayout->addWidget(preview_);

    QVBoxLayout* topLayout = new QVBoxLayout(this);
    topLayout->addWidget(kactusGroup);
    topLayout->addWidget(headerGenerationGroup);
    topLayout->addWidget(gateGenerationGroup);
    topLayout->addWidget(previewGroup);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* runButton = new QPushButton(tr("Run"));
    QPushButton* cancelButton = new QPushButton(tr("Cancel"));
    buttonLayout->addStretch();
    buttonLayout->addWidget(runButton, 0, Qt::AlignRight);
    buttonLayout->addWidget(cancelButton, 0, Qt::AlignRight);
    topLayout->addLayout(buttonLayout);

    connect(runButton, SIGNAL(clicked(bool)),
        this, SLOT(onRunClicked()), Qt::UniqueConnection);    
    connect(cancelButton, SIGNAL(clicked(bool)),
        this, SLOT(reject()), Qt::UniqueConnection);

    setMinimumWidth(MINIMUM_WIDTH);
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::generateHeader()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::generateHeader()
{
    QTextCursor cursor = preview_->document()->find(PadsAsciiSyntax::PART_HEADER_1ST_EXP);
    
    if (cursor.isNull())
    {
        cursor = preview_->textCursor();
        cursor.insertBlock();

        // Insert 1. header line.
        QStringList headerLine1 = PadsAsciiSyntax::PART_HEADER_1ST_LINE.split(PadsAsciiSyntax::SEPARATOR, 
            QString::SkipEmptyParts);    
        
        headerLine1.replace(PadsAsciiSyntax::NAME, nameEditor_->text().toUpper());        
        headerLine1.replace(PadsAsciiSyntax::DECALS, tr("NO_DECAL"));
        if (unitSelector_->currentIndex() == IMPERIAL)
        {
            headerLine1.replace(PadsAsciiSyntax::UNITS, PadsAsciiSyntax::IMPERIAL);
        } 
        else
        {
            headerLine1.replace(PadsAsciiSyntax::UNITS, PadsAsciiSyntax::METRIC);
        }
        headerLine1.replace(PadsAsciiSyntax::LOGFAMILY, familyEditor_->text());

        int numAttr = PREGEN_ATTRS + component_->getParameters().size();

        headerLine1.replace(PadsAsciiSyntax::NUM_ATTRS, QString::number(numAttr));
        int gateCount = 1;
        if (gateGenerationType_ == MULTIPLEGATES)
        {
            gateCount = component_->getBusInterfaces().size();
        } 
        headerLine1.replace(PadsAsciiSyntax::NUM_GATES, QString::number(gateCount));
        headerLine1.replace(PadsAsciiSyntax::NUM_SIGNPINS, "0");
        headerLine1.replace(PadsAsciiSyntax::NUM_PINMAPS, "0");
        headerLine1.replace(PadsAsciiSyntax::FLAG, "0");

        insertLine(headerLine1.join(PadsAsciiSyntax::SEPARATOR), cursor, PadsAsciiSyntax::PART_HEADER_1ST_EXP);         

        // Insert 2. header line.
        QString headerLine2 = PadsAsciiSyntax::PART_HEADER_2ND_LINE;

        QDateTime time = QDateTime::currentDateTime();
        headerLine2.replace("year", QString::number(time.date().year()));
        headerLine2.replace("month", QString::number(time.date().month()));
        headerLine2.replace("day", QString::number(time.date().day()));
        headerLine2.replace("hour", QString::number(time.time().hour()));
        headerLine2.replace("minute", QString::number(time.time().minute()));
        headerLine2.replace("second", QString::number(time.time().second()));

        cursor.insertText(headerLine2);
        cursor.insertBlock();        
        preview_->setTextCursor(cursor);

        // Insert attributes.
        insertAttributes(cursor);
        cursor.insertBlock();
    }
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::onGenerateGates()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::generateGates()
{
    QTextCursor cursor = preview_->document()->find(PadsAsciiSyntax::PART_GATE_EXP);

    if (!cursor.isNull())
    {
        // Clear previous gate information.               
        QTextCursor endCursor = preview_->document()->find(PadsAsciiSyntax::PADS_FILE_END);
        cursor.setPosition(endCursor.position(), QTextCursor::KeepAnchor);
        cursor.movePosition(QTextCursor::PreviousBlock, QTextCursor::KeepAnchor);
        cursor.removeSelectedText();
    }
    else
    {
        cursor = preview_->textCursor();
    }

    int gateCount = 0;

    if (gateGenerationType_ == MULTIPLEGATES)
    {
        QList<QSharedPointer<BusInterface> > busInterfaces = component_->getBusInterfaces();
        foreach(QSharedPointer<BusInterface> busInterface, busInterfaces)
        {
            int pinCount = 0;
            foreach(QSharedPointer<General::PortMap> portMap, busInterface->getPortMaps())
            {
                pinCount += portMap->logicalVector_->getSize();
            }
            insertGate(busInterface->getName(), 1, pinCount, 0, cursor);
            insertPins(busInterface, cursor);
            cursor.insertBlock();

            gateCount = busInterfaces.size();
        }
    } 

    else //if (gateGenerationType_ == SINGLEGATE)
    {
        int pinCount = 0;
        foreach(QSharedPointer<BusInterface> busInterface, component_->getBusInterfaces())
        {
            foreach(QSharedPointer<General::PortMap> portMap, busInterface->getPortMaps())
            {
                pinCount += portMap->logicalVector_->getSize();
            }
        }

        QString gateName = gateNameEditor_->text();
        if (gateName.isEmpty())
        {
            gateName = DEFAULT_GATENAME;
        }
        insertGate(gateName, 1, pinCount, 0, cursor);

        foreach(QSharedPointer<BusInterface> busInterface, component_->getBusInterfaces())
        {
            insertPins(busInterface, cursor);
        }        
           
        gateCount = 1;        
    }

    // Update 1. header line with correct amount of gates.
    cursor = preview_->document()->find(PadsAsciiSyntax::PART_HEADER_1ST_EXP);
    if (!cursor.isNull())
    {
        QStringList headerLine1 = cursor.block().text().split(PadsAsciiSyntax::SEPARATOR, 
            QString::SkipEmptyParts);    
        headerLine1.replace(PadsAsciiSyntax::NUM_GATES, QString::number(gateCount));
        cursor.movePosition(QTextCursor::NextBlock, QTextCursor::KeepAnchor);

        insertLine(headerLine1.join(PadsAsciiSyntax::SEPARATOR), 
            cursor, PadsAsciiSyntax::PART_HEADER_1ST_EXP);                
    }
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::insertGate()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::insertGate(QString const& name, int decals, int pins, int gateswap, QTextCursor& cursor)
{
    QStringList gateLine = PadsAsciiSyntax::PART_GATE_DECLARATION.split(PadsAsciiSyntax::SEPARATOR);;
    gateLine.replace(PadsAsciiSyntax::NUM_DECALS, QString::number(decals));
    gateLine.replace(PadsAsciiSyntax::NUM_PINS, QString::number(pins));
    gateLine.replace(PadsAsciiSyntax::GATESWAP, QString::number(gateswap));

    // Add gate declaration.  
    insertLine(gateLine.join(PadsAsciiSyntax::SEPARATOR), cursor, PadsAsciiSyntax::PART_GATE_EXP);    

    // Add gate name declaration on 2nd line.
    int row = cursor.block().firstLineNumber();
    gates_.insert(cursor.blockNumber(), name);    
    insertLine(nameEditor_->text().toUpper() + "_" + name.toUpper(), cursor, PadsAsciiSyntax::PART_GATE_NAME_EXP);        
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::insertPins()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::insertPins(QSharedPointer<BusInterface> busInterface, QTextCursor& cursor)
{
    // Get the abstract definition of the bus interface for resolving the logical signal size.
    QSharedPointer<LibraryComponent> libComp = libHandler_->getModel(busInterface->getAbstractionType());
    QSharedPointer<AbstractionDefinition> absDef = libComp.staticCast<AbstractionDefinition>();
    bool showLogicalIndex = false;     

    foreach(QSharedPointer<General::PortMap> portMap, busInterface->getPortMaps())
    {
        if (absDef)
        {
            showLogicalIndex = (absDef->getPortSize(portMap->logicalPort_, busInterface->getInterfaceMode()) > 1); 
        }

        QString pin = portMap->physicalPort_;
        QStringList line = PadsAsciiSyntax::PART_GATE_PIN.split(PadsAsciiSyntax::SEPARATOR);        
        
        int logLower = qMin(portMap->logicalVector_->getLeft(), portMap->logicalVector_->getRight());
        int logHigher = qMax(portMap->logicalVector_->getLeft(), portMap->logicalVector_->getRight());            
        for (int logIndex = logLower; logIndex <= logHigher; logIndex++)
        {                    
            line.replace(PadsAsciiSyntax::PINNUMBER, pin);
            line.replace(PadsAsciiSyntax::PINSWAP, "0");
            line.replace(PadsAsciiSyntax::PINTYPE, "U");
            if (showLogicalIndex)
            {
                line.replace(PadsAsciiSyntax::PINNAME, portMap->logicalPort_  + "_" + QString::number(logIndex));
            } 
            else
            {
                line.replace(PadsAsciiSyntax::PINNAME, portMap->logicalPort_ );
            }  

            insertLine(line.join(PadsAsciiSyntax::SEPARATOR), cursor, PadsAsciiSyntax::PART_GATE_PIN_EXP);                                        
        }
    }     
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::insertLine()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::insertLine(QString line, QTextCursor& cursor, 
    QRegExp const validatingExp /*= QRegExp() */)
{    
    if (validatingExp.pattern().isEmpty() || validatingExp.exactMatch(line))
    {
        cursor.insertText(line, okFormat_);            
    }
    else
    {                
        cursor.insertText(line, errorFormat_);
    } 
    cursor.insertBlock();
}

//-----------------------------------------------------------------------------
// Function: PadsPartGeneratorDialog::insertAttributes()
//-----------------------------------------------------------------------------
void PadsPartGeneratorDialog::insertAttributes(QTextCursor& cursor)
{
    // Insert default attributes.
    QStringList attr = PadsAsciiSyntax::ATTRIBUTE_DECLARATION.split(PadsAsciiSyntax::SEPARATOR, 
        QString::SkipEmptyParts);
    attr.replace(PadsAsciiSyntax::ATTRNAME, tr("\"Generated by\""));
    attr.replace(PadsAsciiSyntax::VALUE, "Kactus2 " + generatorName_ + tr(" version ") + 
        generatorVersion_);
    insertLine(attr.join(PadsAsciiSyntax::SEPARATOR), cursor, PadsAsciiSyntax::ATTRIBUTE_EXP);

    attr.replace(PadsAsciiSyntax::ATTRNAME, tr("\"Author\""));
    attr.replace(PadsAsciiSyntax::VALUE, component_->getAuthor());
    insertLine(attr.join(PadsAsciiSyntax::SEPARATOR), cursor, PadsAsciiSyntax::ATTRIBUTE_EXP);

    attr.replace(PadsAsciiSyntax::ATTRNAME, tr("\"Vendor\""));
    attr.replace(PadsAsciiSyntax::VALUE, component_->getVlnv()->getVendor());
    insertLine(attr.join(PadsAsciiSyntax::SEPARATOR), cursor, PadsAsciiSyntax::ATTRIBUTE_EXP);

    attr.replace(PadsAsciiSyntax::ATTRNAME, tr("\"Library\""));
    attr.replace(PadsAsciiSyntax::VALUE, component_->getVlnv()->getLibrary());
    insertLine(attr.join(PadsAsciiSyntax::SEPARATOR), cursor, PadsAsciiSyntax::ATTRIBUTE_EXP);

    attr.replace(PadsAsciiSyntax::ATTRNAME, tr("\"Name\""));
    attr.replace(PadsAsciiSyntax::VALUE, component_->getVlnv()->getName());
    insertLine(attr.join(PadsAsciiSyntax::SEPARATOR), cursor, PadsAsciiSyntax::ATTRIBUTE_EXP);

    attr.replace(PadsAsciiSyntax::ATTRNAME, tr("\"Version\""));
    attr.replace(PadsAsciiSyntax::VALUE, component_->getVlnv()->getVersion());
    insertLine(attr.join(PadsAsciiSyntax::SEPARATOR), cursor, PadsAsciiSyntax::ATTRIBUTE_EXP);

    // Insert all parameters as attributes.
    foreach(QSharedPointer<Parameter> parameter, component_->getParameters())
    {
        attr.replace(PadsAsciiSyntax::ATTRNAME, "\"" + parameter->getName() + "\"");
        attr.replace(PadsAsciiSyntax::VALUE, parameter->getValue());
        insertLine(attr.join(PadsAsciiSyntax::SEPARATOR), cursor, PadsAsciiSyntax::ATTRIBUTE_EXP);
    }
}