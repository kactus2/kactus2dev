//-----------------------------------------------------------------------------
// File: MCAPICodeGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// MCAPI code generator.
//-----------------------------------------------------------------------------

#include "MCAPICodeGenerator.h"

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
// Function: MCAPICodeGenerator::MCAPICodeGenerator()
//-----------------------------------------------------------------------------
MCAPICodeGenerator::MCAPICodeGenerator( MCAPIParser& parser, IPluginUtility* utility ) : 
componentEndpoints_( parser.getComponentEndpoints() ), designNodes_( parser.getDesignNodes() ), utility_( utility )
{
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::~MCAPICodeGenerator()
//-----------------------------------------------------------------------------
MCAPICodeGenerator::~MCAPICodeGenerator()
{
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateMCAPIForComponent()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateMCAPIForComponent(QString dir, QSharedPointer<Component> component)
{
    QStringList replacedFiles;

    QString mainDir = dir + "/main.c";
    QString sourceDir = dir + "/ktsmcapicode.c";
    QString headerDir = dir + "/ktsmcapicode.h";

    // We also need to know if the files exists
    QFile file1(mainDir);
    QFile file2(sourceDir);
    QFile file3(headerDir);

    // If does, put in the list.
    if ( file1.exists() )
    {
        replacedFiles.append(mainDir);
    }

    if ( file2.exists() )
    {
        replacedFiles.append(sourceDir);
    }

    if ( file2.exists() )
    {
        replacedFiles.append(headerDir);
    }

    // Ask verification from the user, if any file is being replaced,
    if ( replacedFiles.size() > 0 )
    {
        // Details will be the list of files being replaced.
        QString detailMsg;

        foreach ( QString file, replacedFiles )
        {
            detailMsg += file + "\n";
        }

        QMessageBox msgBox( QMessageBox::Warning, QCoreApplication::applicationName(),
            "Some files will be WRITTEN OVER in the generation. Proceed?",
            QMessageBox::Yes | QMessageBox::No, utility_->getParentWidget());
        msgBox.setDetailedText(detailMsg);

        // If user did not want to replace the files.
        if (msgBox.exec() == QMessageBox::No) {
            return;
        }
    }

    // Update the ktsmcapicode module.
    generateMainTemplate(mainDir);
    generateHeader(headerDir);
    generateSource(sourceDir);

    // Add the files to the component metadata.
    QSharedPointer<FileSet> fileSet = component->getFileSet("generatedMCAPI");
    fileSet->setGroups("sourceFiles");

    QList<QSharedPointer<FileBuilder> > fblist = fileSet->getDefaultFileBuilders();

    bool hasCBuild = false;

    // Check if there is already a builder for c sources specified.
    foreach ( QSharedPointer<FileBuilder> builder, fblist )
    {
        if ( builder->getFileTypes().contains("cSource") )
        {
            hasCBuild = true;
            break;
        }
    }

    // If there is no pre-existing builder for c sources, make a new one.
    if ( !hasCBuild )
    {
        QSharedPointer<FileBuilder> newBuilder = QSharedPointer<FileBuilder>(new FileBuilder);
        newBuilder->setCommand( "gcc -c -o" );
        newBuilder->setFileType("cSource");
        newBuilder->setReplaceDefaultFlags( true);
        fblist.append(newBuilder);
        fileSet->setDefaultFileBuilders(fblist);
    }

    QSettings settings;

    QSharedPointer<File> file;
    QStringList types;
    types.append("cSource");

    if (!fileSet->contains("ktsmcapicode.h"))
    {
        file = fileSet->addFile("ktsmcapicode.h", settings);
        file->setAllFileTypes( types );
        file->setIncludeFile( true );
    }
    if (!fileSet->contains("ktsmcapicode.c"))
    {
        file = fileSet->addFile("ktsmcapicode.c", settings);
        file->setAllFileTypes( types );
    }
    if (!fileSet->contains("main.c"))
    {
        file = fileSet->addFile("main.c", settings);
        file->setAllFileTypes( types );
    }

    int viewCount = component->getSWViews().size();

    if ( viewCount > 1 )
    {
        // Add fileSet to selected software view
        ComboSelector cs(utility_->getParentWidget());
        cs.setComboBoxItems( component->getSWViewNames() );
        cs.setLabelText( QObject::tr("Select a software view which shall include reference to the generated MCAPI") );

        QString viewName = cs.execDialog();

        if ( !viewName.isEmpty() )
        {
            QSharedPointer<SWView> view = component->getSWView( viewName );
            view->addFileSetRef( fileSet->getName() );
        }
    }
    else if ( viewCount > 0 )
    {
        // Add fileSet to the existing software view
        component->getSWViews().first()->addFileSetRef( fileSet->getName() );
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::createIndentString()
//-----------------------------------------------------------------------------
QString MCAPICodeGenerator::createIndentString()
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
// Function: MCAPICodeGenerator::generateHeader()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateHeader(QString const& filename)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    writer.writeFileHeader();

    writer.writeLine("// DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeEmptyLine();

    writer.writeLine("#ifndef KTSMCAPICODE_H");
    writer.writeLine("#define KTSMCAPICODE_H");
    writer.writeEmptyLine();

    // Write the Kactus 2 generated code.

    writer.writeInclude("mcapi.h");
    writer.writeEmptyLine();

    writer.writeHeaderComment("Node data.");
    writer.writeEmptyLine();

    writeLocalEndpoints(writer, true);
    writeRemoteEndpoints(writer, true);

    writer.writeEmptyLine();

    // Write the other variables.
    writer.writeLine("// Other variables.");
    writer.writeLine("extern mcapi_status_t status;");
    writer.writeEmptyLine();

    writeFunctionDeclarations(writer);

    writer.writeLine("#endif // KTSMCAPICODE_H");
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeFunctionDeclarations(()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeFunctionDeclarations(CSourceWriter &writer)
{
    writer.writeHeaderComment("Functions.");
    writer.writeEmptyLine();


    writer.writeLine("/*");
    writer.writeLine(" *  Main program implemented by the application." );
    writer.writeLine(" *  Has the same parameters as the main.");
    writer.writeLine(" *");
    writer.writeLine(" *        @param [in] argc    The number of arguments in the array of arguments.");
    writer.writeLine(" *        @param [in] char* argv[]    The array of arguments to main.");
    writer.writeLine(" */");
    writer.writeLine("int appmain(int argc, char* argv[]);");
    writer.writeEmptyLine();

    writer.writeLine("/*");
    writer.writeLine(" *  Initializes the MCAPI system and endpoints.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int initializeMCAPI();");
    writer.writeEmptyLine();

    writer.writeLine("/*");
    writer.writeLine(" *  Connects all channels.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int connectChannels();");
    writer.writeEmptyLine();

    writer.writeLine("/*");
    writer.writeLine(" *  Opens all connections.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int openConnections();");
    writer.writeEmptyLine();

    writer.writeLine("/*");
    writer.writeLine(" *  Closes all connections.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int closeConnections();");
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateSource()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateSource(QString const& filename)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    writer.writeFileHeader();

    // Write the Kactus 2 generated code.
    writer.writeLine("// DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeEmptyLine();

    writer.writeInclude("ktsmcapicode.h", true);
    writer.writeInclude("instanceheader.h", true);
    writer.writeInclude("stdio.h");
    writer.writeInclude("errno.h");
    writer.writeEmptyLine();
    writer.writeHeaderComment("Constants.");
    writer.writeEmptyLine();
    writer.writeLine("const mcapi_timeout_t TIMEOUT = 1000;");
    writer.writeEmptyLine();

    // Write local port IDs.
    writeLocalPorts(writer);

    writer.writeEmptyLine();

    writeLocalEndpoints(writer, false);
    writeRemoteEndpoints(writer, false);

    writer.writeEmptyLine();

    // Write the other variables.
    writer.writeLine("// Other variables.");
    writer.writeLine("mcapi_status_t status;");
    writer.writeLine("char status_msg[MCAPI_MAX_STATUS_MSG_LEN];");
    writer.writeEmptyLine();

    // Generate functions.
    generateInitializeMCAPIFunc(writer);
    generateConnectChannelsFunc(writer);
    generateOpenConnectionsFunc(writer);
    generateCloseConnectionsFunc(writer);

    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeLocalPorts()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeLocalPorts(CSourceWriter &writer)
{
    writer.writeLine("// Local port IDs.");

    foreach (MCAPIParser::EndPointData epd, componentEndpoints_)
    {
        writer.writeLine("const mcapi_port_t " + epd.name.toUpper() + "_PORT = " + epd.portID + ";");
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeLocalEndpoints()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeLocalEndpoints(CSourceWriter &writer, bool isExtern)
{
    writer.writeLine("// Local endpoints.");

    QString externString;

    if ( isExtern )
    {
        externString = "extern ";
    }

    foreach (MCAPIParser::EndPointData epd, componentEndpoints_)
    {
        writer.writeLine(externString + "mcapi_endpoint_t " + epd.name + ";");

        QString handleName = epd.handleName;

        if (epd.transferType == "packet")
        {
            if (epd.direction == General::OUT)
            {
                writer.writeLine(externString + "mcapi_pktchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine(externString + "mcapi_pktchan_recv_hndl_t " + handleName + ";");
            }
        }
        else if (epd.transferType == "scalar")
        {
            if (epd.direction == General::OUT)
            {
                writer.writeLine(externString + "mcapi_sclchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine(externString + "mcapi_sclchan_recv_hndl_t " + handleName + ";");
            }
        }

        writer.writeEmptyLine();
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeRemoteEndpoints()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeRemoteEndpoints(CSourceWriter &writer, bool isExtern)
{
    writer.writeLine("// Remote endpoints.");

    QString externString;

    if ( isExtern )
    {
        externString = "extern ";
    }

    foreach (MCAPIParser::EndPointData epd, componentEndpoints_)
    {
        QString remoteEndpointName = epd.remoteName;
        writer.writeLine(externString + "mcapi_endpoint_t " + remoteEndpointName + ";");
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateMainTemplate()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateMainTemplate(QString const& filename)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    writer.writeFileHeader();

    writer.writeLine("// DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeEmptyLine();

    // Write includes.
    writer.writeInclude("stdlib.h");
    writer.writeInclude("stdio.h");
    writer.writeInclude("errno.h");
    writer.writeEmptyLine();

    writer.writeLine("// This header includes the Kactus2 generated MCAPI code.");
    writer.writeInclude("ktsmcapicode.h");
    writer.writeEmptyLine();

    writer.writeLine("int main(int argc, char* argv[])");
    writer.beginBlock();
    writer.writeLine("// Return value from application main.");
    writer.writeLine("int appReturn = 0;");
    writer.writeLine("// Initialize MCAPI.");

    writer.writeLine("if (initializeMCAPI() != 0)");
    writer.beginBlock();
    writer.writeLine("fprintf(stderr, \"MCAPI INITIALIZATION FAILED!!!\\n\" );");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeEmptyLine();

    writer.writeLine("if (connectChannels() != 0)");
    writer.beginBlock();
    writer.writeLine("fprintf(stderr, \"CONNECTING CHANNELS FAILED!!!\\n\" );");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeEmptyLine();

    writer.writeLine("if (openConnections() != 0)");
    writer.beginBlock();
    writer.writeLine("fprintf(stderr, \"OPENING CHANNELS FAILED!!!\\n\" );");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeEmptyLine();

    writer.writeLine("// Branching to the application execution.");
    writer.writeLine("// NOTICE: This function must be implemented by application!");
    writer.writeLine("appReturn = appmain(argc, argv);");
    writer.writeEmptyLine();
    writer.writeLine("// Close connections and finalize MCAPI before exiting.");
    writer.writeLine("if (closeConnections() != 0)");
    writer.beginBlock();
    writer.writeLine("fprintf(stderr, \"CLOSING CHANNELS FAILED!!!\\n\" );");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeEmptyLine();
    writer.writeLine("mcapi_finalize(&status);");
    writer.writeEmptyLine();
    writer.writeLine("return appReturn;");

    writer.endBlock();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateInitializeMCAPIFunc()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateInitializeMCAPIFunc(CSourceWriter& writer)
{
    // Write the initialization function.
    writer.writeHeaderComment("Function: initializeMCAPI()");
    writer.writeLine("int initializeMCAPI()");

    writer.beginBlock();
    writer.writeLine("mcapi_status_t status;");
    writer.writeLine("mcapi_info_t info;");
    writer.writeEmptyLine();

    createInitialization(writer);
    writer.writeEmptyLine();

    createLocalEndpoints(writer);

    getRemoteEndpoints(writer);

    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::createInitialization()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::createInitialization(CSourceWriter &writer)
{
    writer.writeLine("// Initialize MCAPI implementation.");
    writer.writeLine("mcapi_initialize(LOCAL_DOMAIN_ID, LOCAL_NODE_ID, 0, 0, &info, &status);");
    writer.writeEmptyLine();

    writer.writeLine("if (status != MCAPI_SUCCESS)");
    writer.beginBlock();
    writer.writeLine("mcapi_display_status( status, status_msg, MCAPI_MAX_STATUS_MSG_LEN );");
    writer.writeLine("fprintf(stderr, \"ERROR: %s Failed to initialized node %u at domain %u\"" );
    writer.writeLine( "\" at line %u.\\n\", status_msg, LOCAL_NODE_ID, LOCAL_DOMAIN_ID, __LINE__ );");
    writer.writeLine("return -1;");
    writer.endBlock();
}


//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::createLocalEndpoints()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::createLocalEndpoints(CSourceWriter &writer)
{
    writer.writeLine("// Create local endpoints.");

    foreach (MCAPIParser::EndPointData epd, componentEndpoints_)
    {
        writer.writeLine(epd.name + " = mcapi_endpoint_create(" + epd.name.toUpper() + "_PORT, &status);");
        writer.writeEmptyLine();

        writer.writeLine("if (status != MCAPI_SUCCESS)");
        writer.beginBlock();

        writer.writeLine("mcapi_display_status( status, status_msg, MCAPI_MAX_STATUS_MSG_LEN );");
        writer.writeLine("fprintf(stderr, \"ERROR: %s Failed to create endpoint " + epd.name + "\"" );
        writer.writeLine( "\" at line %u.\\n\", status_msg, __LINE__ );");
        writer.writeLine("return -1;");
        writer.endBlock();
        writer.writeEmptyLine();
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getRemoteEndpoints()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::getRemoteEndpoints(CSourceWriter &writer)
{
    writer.writeLine("// Retrieve remote endpoints.");

    foreach (MCAPIParser::EndPointData epd, componentEndpoints_)
    {
        writer.writeLine(epd.remoteName + " = mcapi_endpoint_get(" + epd.remoteName.toUpper() +
            "_DOMAIN" + ", " +  epd.remoteName.toUpper() + "_NODE," );
        writer.writeLine( epd.remoteName.toUpper() + "_PORT, TIMEOUT, &status);");
        writer.writeEmptyLine();

        writer.writeLine("if (status != MCAPI_SUCCESS)");
        writer.beginBlock();
        writer.writeLine("mcapi_display_status( status, status_msg, MCAPI_MAX_STATUS_MSG_LEN );");
        writer.writeLine("fprintf(stderr, \"ERROR: %s Failed to get remote endpoint " + epd.name + "\"" );
        writer.writeLine("\" at line %u.\\n\", status_msg, __LINE__ );");
        writer.writeLine("return -1;");
        writer.endBlock();
        writer.writeEmptyLine();
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateConnectChannelsFunc()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateConnectChannelsFunc(CSourceWriter& writer)
{
    // Write the connectChannels() function.
    writer.writeHeaderComment("Function: connectChannels()");
    writer.writeLine("int connectChannels()");

    // Must have request for each interface, and a value used to check if it opened already.
    QString interfaceCount = QString::number( componentEndpoints_.size() ) ;

    writer.beginBlock();
    writeConnectionVariables(writer, interfaceCount, "connect");
    writer.writeEmptyLine();

    foreach (MCAPIParser::EndPointData epd, componentEndpoints_ )
    {
        // Sender is responsible of calling the connect function.
        if ( epd.direction == General::OUT )
        {
            if (epd.transferType == "packet")
            {
                writeCon(epd, writer, "pkt");
            }
            else if (epd.transferType == "scalar")
            {
                writeCon(epd, writer, "scl");
            }
        }
        else
        {
            writer.writeLine("connect_complete[ifIter] = 1;");
        }

        writer.writeLine("++ifIter;");
        writer.writeEmptyLine();
    }
    
    writeRetryLoop(writer, "connect");

    writer.writeEmptyLine();
    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateOpenConnectionsFunc()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateOpenConnectionsFunc(CSourceWriter& writer)
{
    // Write the openConnections() function.
    writer.writeHeaderComment("Function: openConnections()");
    writer.writeLine("int openConnections()");

    // Must have request for each interface, and a value used to check if it opened already.
    QString interfaceCount = QString::number( componentEndpoints_.size() ) ;

    writer.beginBlock();
    writeConnectionVariables(writer, interfaceCount, "open");
    writer.writeEmptyLine();

    foreach (MCAPIParser::EndPointData epd, componentEndpoints_)
    {
        if (epd.transferType == "packet")
        {
            writeOpen(epd, writer, "pkt");
        }
        else if (epd.transferType == "scalar")
        {
            writeOpen(epd, writer, "scl");
        }

        writer.writeLine("++ifIter;");
        writer.writeEmptyLine();
    }

    writeRetryLoop(writer, "open");

    writer.writeEmptyLine();
    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateCloseConnectionsFunc()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateCloseConnectionsFunc(CSourceWriter& writer)
{
    // Write the closeConnections() function.
    writer.writeHeaderComment("Function: closeConnections()");
    writer.writeLine("int closeConnections()");

    // Must have request for each interface, and a value used to check if it opened already.
    QString interfaceCount = QString::number( componentEndpoints_.size() ) ;

    writer.beginBlock();
    writeConnectionVariables(writer, interfaceCount, "close");

    writer.writeEmptyLine();

    foreach (MCAPIParser::EndPointData epd, componentEndpoints_)
    {
        if (epd.transferType == "packet")
        {
            writeClose(epd, writer, "pkt");
        }
        else if (epd.transferType == "scalar")
        {
            writeClose(epd, writer, "scl");
        }

        writer.writeLine("++ifIter;");
        writer.writeEmptyLine();
    }

    writeRetryLoop(writer, "close");
    writer.writeEmptyLine();

    writer.writeEmptyLine();
    writer.writeLine("return 0;");
    writer.endBlock();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeConnectionParameters()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeConnectionVariables(CSourceWriter &writer, QString interfaceCount, QString name)
{
    writer.writeLine("size_t interfaceCount = " + interfaceCount + ";");
    writer.writeLine("int tryMaximum = 5;");
    writer.writeLine("mcapi_request_t request;");
    writer.writeLine("mcapi_request_t " + name + "_request[interfaceCount];");
    writer.writeLine("char " + name + "_complete[interfaceCount];");
    writer.writeLine("size_t size = 0;");
    writer.writeLine("int ifIter = 0;");
    writer.writeLine("int retry = 0;");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeConOpen()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeCon(MCAPIParser::EndPointData epd, CSourceWriter &writer, QString name)
{
    writer.writeLine("mcapi_" + name + "chan_connect_i(" + epd.name + ", " +
        epd.remoteName + ", &request, &status);");

    writer.writeEmptyLine();
    writePendStatusCheck(writer, "connect");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeConOpen()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeOpen(MCAPIParser::EndPointData epd, CSourceWriter &writer, QString name)
{
    if (epd.direction == General::OUT)
    {
        writer.writeLine("mcapi_" + name + "chan_send_open_i(&" + epd.handleName +
            ", " + epd.name + ", &request, &status);");
    }
    else if (epd.direction == General::IN)
    {
        writer.writeLine("mcapi_" + name + "chan_recv_open_i(&" + epd.handleName +
            ", " + epd.name + ", &request, &status);");
    }

    writer.writeEmptyLine();
    writePendStatusCheck(writer, "open");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeClose()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeClose(MCAPIParser::EndPointData epd, CSourceWriter &writer, QString name)
{
    if (epd.direction == General::OUT)
    {
        writer.writeLine("mcapi_" + name + "chan_send_close_i(" + epd.handleName + ", &request, &status);");
    }
    else if (epd.direction == General::IN)
    {
        writer.writeLine("mcapi_" + name + "chan_recv_close_i(" + epd.handleName + ", &request, &status);");
    }

    writer.writeEmptyLine();
    writePendStatusCheck(writer, "close");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeRetryLoop()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeRetryLoop(CSourceWriter &writer, QString name)
{
    writer.writeLine("for ( retry = 0; retry < tryMaximum; ++retry )");
    writer.beginBlock();
    writer.writeLine("for ( ifIter = 0; ifIter < interfaceCount; ++ifIter )");
    writer.beginBlock();

    writeIterationWait(writer, name);

    writer.endBlock();
    writer.endBlock();

    writer.writeEmptyLine();
    writeCompleteCheck(writer, name);
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeIterationWait()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeIterationWait(CSourceWriter &writer, QString name)
{
    writer.writeLine("if ( " + name + "_complete[ifIter] != 1 )");
    writer.beginBlock();
    writer.writeLine("if (mcapi_wait(&" + name + "_request[ifIter], &size, TIMEOUT, &status)");
    writer.writeLine("== MCAPI_TRUE && status == MCAPI_SUCCESS)");
    writer.beginBlock();
    writer.writeLine(name + "_complete[ifIter] = 1;");
    writer.endBlock();
    writer.endBlock();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeCompleteCheck()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeCompleteCheck(CSourceWriter &writer, QString name)
{
    writer.writeLine("for ( ifIter = 0; ifIter < interfaceCount; ++ifIter )");
    writer.beginBlock();
    writer.writeLine("if ( " + name + "_complete[ifIter] != 1 )");
    writer.beginBlock();
    writer.writeLine("mcapi_display_status( status, status_msg, MCAPI_MAX_STATUS_MSG_LEN );");
    writer.writeLine("fprintf(stderr, \"ERROR: %s Failed to " + name + " channel \"" );
    writer.writeLine("\"at line %u.\\n\", status_msg, __LINE__ );");
    writer.writeLine("return -1;");
    writer.endBlock();
    writer.endBlock();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writePendStatusCheck()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writePendStatusCheck(CSourceWriter &writer, QString name)
{
    writer.writeLine("if (status == MCAPI_PENDING)");
    writer.beginBlock();
    writer.writeLine(name + "_request[ifIter] = request;");
    writer.writeLine(name + "_complete[ifIter] = 0;");
    writer.endBlock();

    writer.writeLine("else if (status != MCAPI_SUCCESS)");
    writer.beginBlock();
    writer.writeLine("mcapi_display_status( status, status_msg, MCAPI_MAX_STATUS_MSG_LEN );");
    writer.writeLine("fprintf(stderr, \"ERROR: %s Failed to " + name + " channel \"" );
    writer.writeLine("\"at line %u.\\n\", status_msg, __LINE__ );");
    writer.writeLine("return -1;");
    writer.endBlock();

    writer.writeLine("else");
    writer.beginBlock();
        writer.writeLine(name + "_complete[ifIter] = 1;");
    writer.endBlock();

    writer.writeEmptyLine();
}