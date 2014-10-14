//-----------------------------------------------------------------------------
// File: MCAPIHeaderGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 13.10.2014
//
// Description:
// MCAPI generator for instance specific headers.
//-----------------------------------------------------------------------------

#include "MCAPIHeaderGenerator.h"

#include <QMessageBox>
#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>
#include <QObject>

#include <CSourceWriter.h>
#include <editors/CSourceEditor/CSourceTextEdit.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/file.h>

#include <library/LibraryManager/libraryinterface.h>

#include "IPXACTmodels/SWView.h"
#include "common/dialogs/comboSelector/comboselector.h"
#include "IPXACTmodels/SystemView.h"

//-----------------------------------------------------------------------------
// Function: MCAPIHeaderGenerator::MCAPIHeaderGenerator()
//-----------------------------------------------------------------------------
MCAPIHeaderGenerator::MCAPIHeaderGenerator( MCAPIParser& parser, IPluginUtility* utility ) : 
componentEndpoints_( parser.getComponentEndpoints() ), designNodes_( parser.getDesignNodes() ), utility_( utility )
{
}

//-----------------------------------------------------------------------------
// Function: MCAPIHeaderGenerator::~MCAPIHeaderGenerator()
//-----------------------------------------------------------------------------
MCAPIHeaderGenerator::~MCAPIHeaderGenerator()
{
}

//-----------------------------------------------------------------------------
// Function: MCAPIHeaderGenerator::generateTopLevel()
//-----------------------------------------------------------------------------
void MCAPIHeaderGenerator::generateTopLevel(QSharedPointer<Design> design, QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration const> desgConf, QString topDir)
{
    foreach ( MCAPIParser::NodeData nd, designNodes_ )
    {
        generateInstanceHeader(nd.directory, nd);

        addGeneratedMCAPIToFileset(General::getRelativePath(topDir,nd.directory), topComponent,
            nd.instance, desgConf);
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIHeaderGenerator::createIndentString()
//-----------------------------------------------------------------------------
QString MCAPIHeaderGenerator::createIndentString()
{
    QSettings settings;

    // Read indentation settings.
    IndentStyle style = static_cast<IndentStyle>(settings.value("Editor/IndentStyle",
        INDENT_STYLE_SPACES).toInt());
    unsigned int width = settings.value("Editor/IndentWidth", 4).toInt();

    QString indent;

    if (style == INDENT_STYLE_SPACES)
    {
        indent.fill(' ', width);
    }
    else
    {
        indent = "\t";
    }

    return indent;
}

//-----------------------------------------------------------------------------
// Function: generateInstanceHeader()
//-----------------------------------------------------------------------------
void MCAPIHeaderGenerator::generateInstanceHeader(QString& directory, MCAPIParser::NodeData& nodeData)
{
    // Create folder for the instance.
    QDir path;
    path.mkpath( directory );

    // Open the file for writing.
    QString const filename = directory + "/instanceheader.h";
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    writer.writeFileHeader();

    writer.writeLine("// DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeEmptyLine();

    writer.writeLine("#ifndef INSTANCEHEADER_H");
    writer.writeLine("#define INSTANCEHEADER_H");
    writer.writeEmptyLine();

    // List of items going to array definition.
    QStringList endpointDefs;

    writer.writeLine("//Local instance specific data:");
    writer.writeLine("#define LOCAL_NODE_ID " + nodeData.nodeID);
    writer.writeLine("#define LOCAL_DOMAIN_ID " + nodeData.domainID);

    writer.writeEmptyLine();

    QPair<MCAPIParser::EndPointData, MCAPIParser::EndPointData> conPair;
    foreach( conPair, nodeData.connections )
    {
        writeRemoteEndpoint(writer, conPair.first, conPair.second, endpointDefs);
    }

    writeEndpointDefList(writer, endpointDefs);

    writer.writeLine("#endif");

    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPIHeaderGenerator::addGeneratedMCAPIToFileset()
//-----------------------------------------------------------------------------
void MCAPIHeaderGenerator::addGeneratedMCAPIToFileset(QString directory, QSharedPointer<Component> topComponent,
    SWInstance& instance, QSharedPointer<DesignConfiguration const> desgConf)
{
    QString sysViewName;

    foreach( QSharedPointer<SystemView> view, topComponent->getSystemViews() )
    {
        if ( view->getHierarchyRef() == *desgConf->getVlnv() )
        {
           sysViewName = view->getName();
           break;
        }
    }

    // Add the files to the component metadata.
    QString fileSetName;

    // Check if the software instance has and existing fileSet reference. 
    if ( instance.getFileSetRef().isEmpty() )
    {
        // If not, make a new one.
        fileSetName = sysViewName + "_" + instance.getInstanceName() + "_headers";
        instance.setFileSetRef( fileSetName );
    }
    else
    {
        // If there is pre-existing reference, use it.
        fileSetName = instance.getFileSetRef();
    }

    // Obtain the the fileSet by name and set it as a source file group.
    QSharedPointer<FileSet> fileSet = topComponent->getFileSet(fileSetName);
    fileSet->setGroups("sourceFiles");

    // Path of the file, including file name.
    QString filePath = directory + "/instanceheader.h";

    // Create file if does not already exist.
    if ( !fileSet->contains(filePath) )
    {
        QSharedPointer<File> file;
        QStringList types;
        types.append("cSource");
        QSettings settings;
        file = fileSet->addFile(filePath, settings);
        file->setAllFileTypes( types );
        file->setIncludeFile( true );
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPIHeaderGenerator::writeRemoteEndpoint()
//-----------------------------------------------------------------------------
void MCAPIHeaderGenerator::writeRemoteEndpoint(CSourceWriter &writer, MCAPIParser::EndPointData ourEnd,
    MCAPIParser::EndPointData theirEnd, QStringList& endpointDefs)
{
    QString remoteName = ourEnd.remoteName;

    // Direction and transfer type must convert to correct format.
    QString ourTransferString = transferTypeToEString(ourEnd.transferType);
    QString ourDirectionString = transferDirectionToEString(ourEnd.direction);
    QString theirTransferString = transferTypeToEString(theirEnd.transferType);
    QString theirDirectionString = transferDirectionToEString(theirEnd.direction);

    // Read the sizes of scalars.
    QString ourScalarSize = ourEnd.scalarSize;
    QString theirScalarSize = theirEnd.scalarSize;

    // Generate the endpoint identifier for the remote endpoint.
    writer.writeLine(" //" + remoteName + " endpoint identifier:");
    writer.writeLine("#define " + remoteName.toUpper() + "_DOMAIN "+ theirEnd.domainID);
    writer.writeLine("#define " + remoteName.toUpper() + "_NODE "+ theirEnd.nodeID);
    writer.writeLine("#define " + remoteName.toUpper() + "_PORT "+ theirEnd.portID);

    writer.writeEmptyLine();

    // Generate identifier strings corresponding the endpoint identifiers.
    QString theirIdString = makeIdString(theirEnd.domainID, theirEnd.nodeID, theirEnd.portID);
    QString ourIdString = makeIdString(ourEnd.domainID, ourEnd.nodeID, ourEnd.portID);

    // Create endpoint definitions.
    QString ourEndpointDef = ourEnd.name.toUpper() + "_DEF";
    QString theirEndpointDef = remoteName.toUpper() + "_DEF";

    writer.writeLine("#define " + ourEndpointDef + " {" + ourIdString + ", " + ourTransferString  +
        ", " + ourDirectionString + ", " + theirIdString + ", " + ourScalarSize + "}" );
    writer.writeLine("#define " + theirEndpointDef + " {" + theirIdString + ", " + ourTransferString +
        ", " + theirDirectionString + ", " + ourIdString + ", " + theirScalarSize + "}" );

    writer.writeEmptyLine();

    // Append the definitions to list, to be placed to an array.
    endpointDefs.append(ourEndpointDef);
    endpointDefs.append(theirEndpointDef);
}

//-----------------------------------------------------------------------------
// Function: MCAPIHeaderGenerator::writeEndpointDefList()
//-----------------------------------------------------------------------------
void MCAPIHeaderGenerator::writeEndpointDefList(CSourceWriter& writer, QStringList& endpointDefs)
{
    writer.writeLine("//List of endpoint definitions used by the implementation." );
    writer.writeLine("#define DEF_LIST { \\" );
    QString line;

    for ( int i = 0; i < endpointDefs.size(); ++i )
    {
        QString edpDef = endpointDefs[i];

        line.append(edpDef + ", ");

        if ( (i+1)%5 == 0 )
        {
            writer.writeLine(line + " \\");
            line.clear();
        }
    }

    if ( !line.isEmpty() )
    {
        writer.writeLine(line + " \\");
    }

    writer.writeLine("}" );
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPIHeaderGenerator::transferDirectionToEString()
//-----------------------------------------------------------------------------
QString MCAPIHeaderGenerator::transferDirectionToEString(General::Direction direction)
{
    if ( direction == General::IN )
    {
        return "CHAN_DIR_RECV";
    }
    else if ( direction == General::OUT )
    {
        return "CHAN_DIR_SEND";
    }
    else if ( direction == General::INOUT )
    {
        return "CHAN_NO_DIR";
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: MCAPIHeaderGenerator::transferTypeToEString()
//-----------------------------------------------------------------------------
QString MCAPIHeaderGenerator::transferTypeToEString(QString transferType)
{
    if ( transferType == "message" )
    {
        return "MCAPI_NO_CHAN";
    }
    else if ( transferType == "scalar" )
    {
        return"MCAPI_SCL_CHAN";
    }
    else if ( transferType == "packet" )
    {
        return "MCAPI_PKT_CHAN";
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: MCAPIHeaderGenerator::makeIdString()
//-----------------------------------------------------------------------------
QString MCAPIHeaderGenerator::makeIdString(QString domainID, QString nodeID, QString portID)
{
    return "{" + domainID + ", " + nodeID + ", " + portID + "}";
}