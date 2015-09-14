//-----------------------------------------------------------------------------
// File: TLMWHeaderGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 13.7.2015
//
// Description:
// TLMW generator for instance specific headers.
//-----------------------------------------------------------------------------

#include "TLMWHeaderGenerator.h"

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
// Function: TLMWHeaderGenerator::TLMWHeaderGenerator()
//-----------------------------------------------------------------------------
TLMWHeaderGenerator::TLMWHeaderGenerator( TLMWParser& parser, IPluginUtility* utility ) : 
componentEndpoints_( parser.getComponentEndpoints() ), designNodes_( parser.getDesignNodes() ), utility_( utility )
{
}

//-----------------------------------------------------------------------------
// Function: TLMWHeaderGenerator::~TLMWHeaderGenerator()
//-----------------------------------------------------------------------------
TLMWHeaderGenerator::~TLMWHeaderGenerator()
{
}

//-----------------------------------------------------------------------------
// Function: TLMWHeaderGenerator::generateTopLevel()
//-----------------------------------------------------------------------------
void TLMWHeaderGenerator::generateTopLevel(QSharedPointer<Design> design, QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration const> desgConf, QString topDir)
{
    foreach ( TLMWParser::NodeData nd, designNodes_ )
    {
        generateInstanceHeader(nd.directory, nd);

        addGeneratedTLMWToFileset(General::getRelativePath(topDir,nd.directory), topComponent,
            nd.instance, desgConf);
    }
}

//-----------------------------------------------------------------------------
// Function: TLMWHeaderGenerator::createIndentString()
//-----------------------------------------------------------------------------
QString TLMWHeaderGenerator::createIndentString()
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
void TLMWHeaderGenerator::generateInstanceHeader(QString& directory, TLMWParser::NodeData& nodeData)
{
    // Create folder for the instance.
    QDir path;
    path.mkpath( directory );

    // Open the file for writing.
    QString const filename = directory + "/TLMWheader.h";
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    writer.writeFileHeader();

    writer.writeLine("//DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeEmptyLine();

    writer.writeLine("#ifndef TLMWHEADER_H");
	writer.writeLine("#define TLMWHEADER_H");
	writer.writeLine("#include <string>");
	writer.writeLine("#include \"TLMW_EDA_entry.hh\"");
	writer.writeEmptyLine();

	//Use the official TLMW namespace.
	writer.writeLine("namespace TLMW");
	writer.beginBlock();

    QPair<TLMWParser::EndPointData, QString> conPair;
    foreach( conPair, nodeData.connections )
	{
		writer.writeLine("const EDA_entry " + conPair.first.name + " = { \"" + conPair.second + "\", "
			+ conPair.first.transferSize + " };" );
    }

	writer.endBlock();
    writer.writeLine("#endif");

    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: TLMWHeaderGenerator::addGeneratedTLMWToFileset()
//-----------------------------------------------------------------------------
void TLMWHeaderGenerator::addGeneratedTLMWToFileset(QString directory, QSharedPointer<Component> topComponent,
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
    QString filePath = directory + "/TLMWheader.h";

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