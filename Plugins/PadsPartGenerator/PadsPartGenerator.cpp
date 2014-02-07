//-----------------------------------------------------------------------------
// File: PadsPartGenerator
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 5.11.2013
//
// Description:
// Generator for PADS parts.
//-----------------------------------------------------------------------------

#include "PadsPartGenerator.h"

#include "PadsAsciiSyntax.h"
#include "PadsPartGeneratorDialog.h"

#include <common/KactusAttribute.h>
#include <LibraryManager/libraryinterface.h>
#include <models/component.h>
#include <models/fileset.h>
#include <models/file.h>
#include <models/generaldeclarations.h>
#include <PluginSystem/IPlugin.h>
#include <PluginSystem/IPluginUtility.h>
#include <PluginSystem/PluginSettingsWidget.h>

#include <QDate>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QRect>
#include <QTextStream>
#include <QTime>
#include <QtPlugin>

namespace
{
    // Label options.
    const int LABEL_DEFAULT_COUNT = 4;  //<! Part type, part reference and two free text labels are displayed.
    const int LABEL_DEFAULT_WIDTH = 10;
    const int LABEL_DEFAULT_HEIGHT = 97;
    const int LABEL_JUSTIFICATION = 17;    

    // Item rotation options.
    const int NO_ROTATION = 0;
    const int LEFT_MIRROR = 0;
    const int RIGHT_MIRROR = 2;

    // CAE item settings.
    const int LAYER = 0;                //!< Use all layers.
    const int DEFAULT_VISIBILITY = 2;   //!< Part type invisibility.

    // General justification options of items.
    const int JUST_BOTTOM_RIGHT = 1;
    const int JUST_TOP_LEFT = 2;
    const int JUST_TOP_CENTER = 6;
    const int JUST_MIDDLE_CENTER = 12;
    
    // Pinname options.
    const int PINNAME_X = 230;  //!< Distance in x-direction relative to terminal position.
    const int PINNAME_Y = 0;    //!< Distance in y-direction relative to terminal position.

    // Pin options.
    const int PIN_SPAN = 100;               //!< Distance in y-direction between adjacent pins.    
    const int PIN_WIDTH = 200;              //!< Width of PIN decal.    
    const int PIN_JUSTIFICATION = 16;
    const int NETNAME_DISTANCE = -80;       //!< Distance in x-direction relative to terminal position.
    const int PINNUMBER_DISTANCE = 140;     //!< Distance in x-direction relative to terminal position.
    const int PIN_PROPERTY_DISTANCE = -520; //!< Distance in x-direction relative to terminal position.        

    // Default width for decal rectangle.
    const int PART_DEFAULT_WIDTH = 1600;    
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::PadsPartGenerator()
//-----------------------------------------------------------------------------
PadsPartGenerator::PadsPartGenerator()
{  
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::~PadsPartGenerator()
//-----------------------------------------------------------------------------
PadsPartGenerator::~PadsPartGenerator()
{
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::getName()
//-----------------------------------------------------------------------------
QString const& PadsPartGenerator::getName() const
{
    static QString name(tr("PADS Part Generator"));
    return name;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::getVersion()
//-----------------------------------------------------------------------------
QString const& PadsPartGenerator::getVersion() const
{
    static QString version(tr("1.0"));
    return version;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::getDescription()
//-----------------------------------------------------------------------------
QString const& PadsPartGenerator::getDescription() const
{
    static QString description(tr("Generates PADS part for component."));
    return description;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::getVendor()
//-----------------------------------------------------------------------------
QString const& PadsPartGenerator::getVendor() const {
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::getLicence()
//-----------------------------------------------------------------------------
QString const& PadsPartGenerator::getLicence() const {
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& PadsPartGenerator::getLicenceHolder() const {
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* PadsPartGenerator::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon PadsPartGenerator::getIcon() const
{
    return QIcon(":icons/processor.png");
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool PadsPartGenerator::checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp, 
    QSharedPointer<LibraryComponent const> libDesConf /*= QSharedPointer<LibraryComponent const>()*/, 
    QSharedPointer<LibraryComponent const> libDes /*= QSharedPointer<LibraryComponent const>()*/) const
{
    // Pads part generation can only be run on component editor.
    if (libDesConf || libDes) {
        return false;
    }

    // Part generation can only run on HW chip components.
    QSharedPointer<const Component> comp = libComp.dynamicCast<const Component>();
    if (comp == 0 || 
        comp->getComponentImplementation() != KactusAttribute::KTS_HW || 
        comp->getComponentHierarchy() != KactusAttribute::KTS_CHIP)
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::runGenerator()
//-----------------------------------------------------------------------------
void PadsPartGenerator::runGenerator(IPluginUtility* utility, 
    QSharedPointer<LibraryComponent> libComp, 
    QSharedPointer<LibraryComponent> libDesConf /*= QSharedPointer<LibraryComponent>()*/, 
    QSharedPointer<LibraryComponent> libDes /*= QSharedPointer<LibraryComponent>()*/)
{
    // Part generation can only run on HW components
    QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
    if (comp == 0 || comp->getComponentImplementation() != KactusAttribute::KTS_HW)
    {
        return;
    }

    // Ask user for generation settings.
    PadsPartGeneratorDialog dialog(utility->getLibraryInterface(), comp, getName(), getVersion(), 
        utility->getParentWidget());
    if (dialog.exec() == QDialog::Rejected)
    {
        return;
    }

    utility->printInfo(QString("---------- Kactus2 " + getName() + " " + getVersion() + " ----------"));    
    utility->printInfo(QString(tr("Generation started ") + QDate::currentDate().toString("dd.MM.yyyy") + 
        " " + QTime::currentTime().toString()));

    QString partDescription(dialog.getPreviewText());
    QString filesetName =  dialog.getFileSetName();    
    QString basePath = utility->getLibraryInterface()->getPath(*comp->getVlnv());
    QFileInfo fileInfo(basePath); 
    QString targetDirectoryPath = fileInfo.absolutePath() + "/" + filesetName;
    QDir targetDirectory(targetDirectoryPath);
    if (!targetDirectory.exists())
    {
        targetDirectory.mkpath(".");
    }

    //-----------------------------------------------------------------------------
    // Generate part file.
    //-----------------------------------------------------------------------------
    QString partFileName = dialog.getPartName() + ".p";      
    QString partFilePath =  targetDirectoryPath + "/" + partFileName;
    QFile* partFile = new QFile(partFilePath);

    if (!generatePartFile(partFile, partDescription))
    {
        utility->printError(tr("Could not write file %1").arg(partFilePath));    
    }
   
    partFile->close();
    utility->printInfo(tr("Finished writing %1").arg(partFilePath));

    // Add part file to fileset.    
    if (addFileToFileset(comp->getFileSet(filesetName), partFilePath, basePath, QStringList("padsPart")))
    {
        utility->printInfo(tr("Added %1 to fileset %2").arg(partFileName).arg(filesetName)); 
    }
    else
    {        
        utility->printError(tr("Could not add %1 to fileset %2").arg(partFileName).arg(filesetName));         
    }

    //-----------------------------------------------------------------------------
    // Generate cae decal file.
    //-----------------------------------------------------------------------------
    QString caeFileName = dialog.getPartName() + ".c";   
    QString caeFilePath = targetDirectoryPath + "/" + caeFileName;    
    QFile* caeFile = new QFile(caeFilePath);

    if (!generateCAEFile(caeFile, comp->getVlnv()->toString().toUpper(), partDescription))
    {
        utility->printError(tr("Could not write file %1").arg(caeFilePath));    
    }
    
    caeFile->close();
    utility->printInfo(tr("Finished writing %1").arg(caeFilePath));

    // Add cae file to fileset.
    if (!addFileToFileset(comp->getFileSet(filesetName), caeFilePath, basePath, QStringList("padsCAEDecal")))
    {
        utility->printError(tr("Could not add %1 to fileset %2").arg(caeFileName).arg(filesetName));
    }
    else
    {
        utility->printInfo(tr("Added %1 to fileset %2").arg(caeFileName).arg(filesetName));          
    }

    // Write fileset changes to xml.
    utility->printInfo(tr("========== Part generation complete =========="));
    utility->getLibraryInterface()->writeModelToFile(comp);         
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> PadsPartGenerator::getProgramRequirements()
{
    return QList<IPlugin::ExternalProgramRequirement>();
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::generatePartFile()
//-----------------------------------------------------------------------------                        
bool PadsPartGenerator::generatePartFile(QFile* file, const QString& partDescription)
{
    // open the file and erase all old contents if any exists.
    // if file could not be opened, cancel.
    if (!file->isOpen() && !file->open(QFile::Truncate | QFile::WriteOnly)) {          
        return false;
    }

    // open file stream to write to.
    QTextStream fileStream(file);
    fileStream << partDescription;   
    return true;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::generateCAEFile()
//-----------------------------------------------------------------------------
bool PadsPartGenerator::generateCAEFile(QFile* file, const QString& partTitle, const QString& partDescription)
{
    // open the file and erase all old contents if any exists.
    // if file could not be opened, cancel.
    if (!file->isOpen() && !file->open(QFile::Truncate | QFile::WriteOnly)) {        
        return false;
    }

    QTextStream caeFileStream(file);

    // Write file header.
    caeFileStream << PadsAsciiSyntax::CAE_FILE_BEGIN << endl;
    caeFileStream << endl;
    
    // Get the timestamp from part.
    PadsAsciiSyntax::PART_HEADER_2ND_EXP.indexIn(partDescription);
    QString timestamp = PadsAsciiSyntax::PART_HEADER_2ND_EXP.cap(0);

    // Generate CAE decal for each gate.
    int pos = PadsAsciiSyntax::PART_GATE_EXP.indexIn(partDescription, 0);
    while(pos != -1)
    {
        QString gate = PadsAsciiSyntax::PART_GATE_EXP.cap(0);
        PadsAsciiSyntax::PART_GATE_NAME_EXP.indexIn(partDescription, pos + gate.length());
        QString gateName = PadsAsciiSyntax::PART_GATE_NAME_EXP.cap(0);
        int pinCount = PadsAsciiSyntax::PART_GATE_EXP.cap(PadsAsciiSyntax::NUM_PINS).toInt();

        QRect caeRect(0,0, PART_DEFAULT_WIDTH, pinCount/2 * PIN_SPAN + 5*PIN_SPAN);

        insertCAEHeader(caeFileStream, timestamp, gateName, pinCount);
        insertCAEDecal(caeFileStream, caeRect, partTitle, gateName, pinCount);
        insertCAETerminals(caeFileStream, caeRect, pinCount);
        caeFileStream << endl;

        // Find the next gate.
        pos = PadsAsciiSyntax::PART_GATE_EXP.indexIn(partDescription, pos + gate.length());
    }

    // Write file end.
    caeFileStream << PadsAsciiSyntax::PADS_FILE_END << endl;

    caeFileStream.flush();
    return true;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::generateCAEHeader()
//-----------------------------------------------------------------------------
void PadsPartGenerator::insertCAEHeader(QTextStream& output, QString const& timestamp, 
    QString const& gateName, int pinCount)
{
    // First line of CAE Header.
    QStringList header = PadsAsciiSyntax::CAE_HEADER_1ST_LINE.split(PadsAsciiSyntax::SEPARATOR);
    header.replace(PadsAsciiSyntax::CAENAME, gateName);    
    header.replace(PadsAsciiSyntax::CAE_POS_X, "0");
    header.replace(PadsAsciiSyntax::CAE_POS_Y, "0");
    header.replace(PadsAsciiSyntax::PINNUMBER_HEIGHT, QString::number(LABEL_DEFAULT_HEIGHT));
    header.replace(PadsAsciiSyntax::PINNUMBER_WIDTH, QString::number(LABEL_DEFAULT_WIDTH));
    header.replace(PadsAsciiSyntax::PINNAME_HEIGHT,QString::number(LABEL_DEFAULT_HEIGHT));
    header.replace(PadsAsciiSyntax::PINNAME_WIDTH, QString::number(LABEL_DEFAULT_WIDTH));
    header.replace(PadsAsciiSyntax::NUM_LABELS, QString::number(LABEL_DEFAULT_COUNT));
    header.replace(PadsAsciiSyntax::NUM_PIECES, "1");
    header.replace(PadsAsciiSyntax::NUM_TXT, "2");
    header.replace(PadsAsciiSyntax::NUM_TERMINALS, QString::number(pinCount));
    header.replace(PadsAsciiSyntax::VISIBILITY, QString::number(DEFAULT_VISIBILITY));

    output << header.join(PadsAsciiSyntax::SEPARATOR) << endl;

    // 2. line has the timestamp.
    output << timestamp << endl;

    // Font info on the 3. and 4. lines.
    output << PadsAsciiSyntax::DEFAULT_FONT << endl;
    output << PadsAsciiSyntax::DEFAULT_FONT << endl;
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::generateCAEDecal()
//-----------------------------------------------------------------------------
void PadsPartGenerator::insertCAEDecal(QTextStream& output, QRect const& drawSymbol, 
    QString const& gateTitle, QString const& gateName, int pinCount)
{   
    int partWidth = drawSymbol.width();
    int partHeight = drawSymbol.height();

    QStringList text = PadsAsciiSyntax::CAE_LABEL.split(PadsAsciiSyntax::SEPARATOR);
    text.replace(PadsAsciiSyntax::LABEL_POS_X, QString::number(partWidth/2));
    text.replace(PadsAsciiSyntax::LABEL_POS_Y, QString::number(partHeight + 2*PIN_SPAN));
    text.replace(PadsAsciiSyntax::LABEL_ROTATION,QString::number(NO_ROTATION));
    text.replace(PadsAsciiSyntax::JUSTIFICATION,QString::number(JUST_TOP_CENTER));
    text.replace(PadsAsciiSyntax::LABEL_TEXT_HEIGHT, QString::number(LABEL_DEFAULT_HEIGHT));
    text.replace(PadsAsciiSyntax::LABEL_TEXT_WIDTH, QString::number(LABEL_DEFAULT_WIDTH));
    text.replace(PadsAsciiSyntax::LABEL_FONTINFO, PadsAsciiSyntax::DEFAULT_FONT);

    // Reference designator location definition.
    output << text.join(PadsAsciiSyntax::SEPARATOR) << endl;
    output << "REF-DES" << endl;

    // Part type name definition.
    text.replace(PadsAsciiSyntax::LABEL_POS_Y, QString::number(-2*PIN_SPAN));
    text.replace(PadsAsciiSyntax::JUSTIFICATION,QString::number(JUST_TOP_CENTER));
    output << text.join(PadsAsciiSyntax::SEPARATOR) << endl;
    output << "PART-TYPE" << endl;

    // First part attribute value definition.
    text.replace(PadsAsciiSyntax::LABEL_POS_Y, QString::number(-PIN_SPAN));
    output << text.join(PadsAsciiSyntax::SEPARATOR) << endl;
    output << "*" << endl;

    // Second part attribute value definition.
    text.replace(PadsAsciiSyntax::LABEL_POS_Y, QString::number(partHeight + PIN_SPAN));
    output << text.join(PadsAsciiSyntax::SEPARATOR) << endl;
    output << "*" << endl;

    // Drawing symbol, rectangle, corner points. First point twice.
    output << PadsAsciiSyntax::CAE_RECT_PIECE << endl;
    output << drawSymbol.topLeft().x() << " " << drawSymbol.topLeft().y() << endl;
    output << drawSymbol.bottomLeft().x() << " " << drawSymbol.bottomLeft().y() << endl;        
    output << drawSymbol.bottomRight().x() << " " << drawSymbol.bottomRight().y() << endl;
    output << drawSymbol.topRight().x() << " " << drawSymbol.topRight().y() << endl;
    output << drawSymbol.topLeft().x() << " " << drawSymbol.topLeft().y() << endl;

    // First label for gate title.
    QStringList overheadLabel = PadsAsciiSyntax::TEXT_ITEM.split(PadsAsciiSyntax::SEPARATOR);             
    overheadLabel.replace(PadsAsciiSyntax::TEXT_POS_X, QString::number(drawSymbol.center().x()));
    overheadLabel.replace(PadsAsciiSyntax::TEXT_POS_Y, QString::number(drawSymbol.bottom() - PIN_SPAN));
    overheadLabel.replace(PadsAsciiSyntax::TEXT_ROTATION, QString::number(NO_ROTATION));
    overheadLabel.replace(PadsAsciiSyntax::LAYER, QString::number(LAYER));
    overheadLabel.replace(PadsAsciiSyntax::TEXT_WIDTH, QString::number(LABEL_DEFAULT_WIDTH));
    overheadLabel.replace(PadsAsciiSyntax::TEXT_HEIGHT, QString::number(LABEL_DEFAULT_HEIGHT));
    overheadLabel.replace(PadsAsciiSyntax::TEXT_MIRRORING, QString::number(LEFT_MIRROR));
    overheadLabel.replace(PadsAsciiSyntax::TEXT_JUSTIFICATION, QString::number(LABEL_JUSTIFICATION));
    overheadLabel.replace(PadsAsciiSyntax::PCB_DRAW_NUMBER, "0");
    overheadLabel.replace(PadsAsciiSyntax::FIELD_FLAG, "0");
    overheadLabel.replace(PadsAsciiSyntax::TEXT_FONTINFO, PadsAsciiSyntax::DEFAULT_FONT);
    output << overheadLabel.join(PadsAsciiSyntax::SEPARATOR) << endl;                                        
    output << gateTitle << endl;

    // Second label for gate name.
    overheadLabel.replace(PadsAsciiSyntax::TEXT_POS_Y, QString::number(drawSymbol.bottom() - 2*PIN_SPAN));
    output << overheadLabel.join(PadsAsciiSyntax::SEPARATOR) << endl;         
    output << gateName << endl;        
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::generateCAETerminals()
//-----------------------------------------------------------------------------
void PadsPartGenerator::insertCAETerminals(QTextStream& output, QRect const& drawSymbol, int pinCount)
{
    int halfPins = pinCount/2;
    if (pinCount % 2 != 0)
    {
        halfPins++;
    }

    int x = -PIN_WIDTH;
    int y = drawSymbol.bottom() - 4*PIN_SPAN;

    QStringList terminal = PadsAsciiSyntax::CAE_TERMINAL.split(PadsAsciiSyntax::SEPARATOR);
    terminal.replace(PadsAsciiSyntax::TERMINAL_POS_X, QString::number(x));
    terminal.replace(PadsAsciiSyntax::TERMINAL_ROTATION, QString::number(NO_ROTATION));
    terminal.replace(PadsAsciiSyntax::TERMINAL_MIRROR, QString::number(LEFT_MIRROR));
    terminal.replace(PadsAsciiSyntax::TERMINAL_PINNUM_X, QString::number(PINNUMBER_DISTANCE));
    terminal.replace(PadsAsciiSyntax::TERMINAL_PINNUM_Y, QString::number(20));
    terminal.replace(PadsAsciiSyntax::TERMINAL_PINNUM_ROTATION, QString::number(NO_ROTATION));
    terminal.replace(PadsAsciiSyntax::TERMINAL_PINNUM_JUSTIFICATION, QString::number(JUST_TOP_LEFT));
    terminal.replace(PadsAsciiSyntax::TERMINAL_PINNAME_X, QString::number(PINNAME_X));
    terminal.replace(PadsAsciiSyntax::TERMINAL_PINNAME_Y, QString::number(PINNAME_Y));
    terminal.replace(PadsAsciiSyntax::TERMINAL_PINNAME_ROTATION, QString::number(NO_ROTATION));
    terminal.replace(PadsAsciiSyntax::TERMINAL_PINNAME_JUSTIFICATION, QString::number(PIN_JUSTIFICATION));
    terminal.replace(PadsAsciiSyntax::TERMINAL_PINNAME, "PIN");

    QStringList pinProperty = PadsAsciiSyntax::CAE_PIN_PROPERTY.split(PadsAsciiSyntax::SEPARATOR);
    pinProperty.replace(PadsAsciiSyntax::PINPROPERTY_POS_X, QString::number(PIN_PROPERTY_DISTANCE));
    pinProperty.replace(PadsAsciiSyntax::PINPROPERTY_POS_Y, "0");
    pinProperty.replace(PadsAsciiSyntax::PINPROPERTY_ROTATION, QString::number(NO_ROTATION));
    pinProperty.replace(PadsAsciiSyntax::PINPROPERTY_JUSTIFICATION, QString::number(JUST_TOP_LEFT));
    pinProperty.replace(PadsAsciiSyntax::NETNAME_POS_X, QString::number(NETNAME_DISTANCE));
    pinProperty.replace(PadsAsciiSyntax::NETNAME_POS_Y, "0");
    pinProperty.replace(PadsAsciiSyntax::NETNAME_ROTATION, QString::number(NO_ROTATION));
    pinProperty.replace(PadsAsciiSyntax::NETNAME_JUSTIFICATION, QString::number(JUST_TOP_LEFT));
    pinProperty.replace(PadsAsciiSyntax::PINPROPERTY_FLAGS, "0");


    // Left side terminals.
    for (int i = 0; i < halfPins; i++)
    {
        terminal.replace(PadsAsciiSyntax::TERMINAL_POS_Y,QString::number(y));

        output << "T" << terminal.join(PadsAsciiSyntax::SEPARATOR) << endl;
        output << "P" << pinProperty.join(PadsAsciiSyntax::SEPARATOR) << endl;

        y -= PIN_SPAN;
    }

    // Right side terminals.
    y = drawSymbol.bottom() - 4*PIN_SPAN;
    x = drawSymbol.right() + PIN_WIDTH;
 
    terminal.replace(PadsAsciiSyntax::TERMINAL_MIRROR, QString::number(RIGHT_MIRROR));
    terminal.replace(PadsAsciiSyntax::TERMINAL_POS_X, QString::number(x));
    for (int i = halfPins; i < pinCount; i++)
    {
        terminal.replace(PadsAsciiSyntax::TERMINAL_POS_Y,QString::number(y));

        output << "T" << terminal.join(PadsAsciiSyntax::SEPARATOR) << endl;
        output << "P" << pinProperty.join(PadsAsciiSyntax::SEPARATOR) << endl;

        y -= PIN_SPAN;
    }
}

//-----------------------------------------------------------------------------
// Function: PadsPartGenerator::addFileToFileset()
//-----------------------------------------------------------------------------
bool PadsPartGenerator::addFileToFileset(QSharedPointer<FileSet> fileSet, 
    const QString& filePath, QString const& basePath, const QStringList& fileTypes)
{
    QString relativePath = General::getRelativePath(basePath, filePath);
    if (relativePath.isEmpty())
    {        
        return false;
    }

    // Add a new file to fileset.
    QSharedPointer<File> partFile(new File(relativePath, 0));    
    partFile->setIncludeFile(false);        
    partFile->setAllFileTypes(fileTypes);
  
    fileSet->addFile(partFile);
    return true;
}