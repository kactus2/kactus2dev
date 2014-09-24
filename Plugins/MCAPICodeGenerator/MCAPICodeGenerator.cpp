//-----------------------------------------------------------------------------
// File: McapiCodeGenerator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// MCAPI code generator plugin.
//-----------------------------------------------------------------------------

#include "MCAPICodeGenerator.h"

#include <QtPlugin>
#include <QMessageBox>
#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>
#include <QDir>

#include <CSourceWriter.h>
#include <editors/CSourceEditor/CSourceTextEdit.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/ComInterface.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/file.h>

#include <library/LibraryManager/libraryinterface.h>

#include <Plugins/PluginSystem/IPluginUtility.h>
#include "IPXACTmodels/SWView.h"
#include "common/dialogs/comboSelector/comboselector.h"
#include "IPXACTmodels/SystemView.h"

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::MCAPICodeGenerator()
//-----------------------------------------------------------------------------
MCAPICodeGenerator::MCAPICodeGenerator() : utility_(0)
{
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::~MCAPICodeGenerator()
//-----------------------------------------------------------------------------
MCAPICodeGenerator::~MCAPICodeGenerator()
{
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getName()
//----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getName() const
{
    static QString name("MCAPI Code Generator");
    return name;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getVersion()
//-----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getVersion() const
{
    static QString version("1.1");
    return version;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getDescription()
//-----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getDescription() const
{
    static QString desc("Generates MCAPI code templates based on the metadata.");
    return desc;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getVendor()
//-----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getVendor() const {
    static QString vendor(tr("TUT"));
    return vendor;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getLicence()
//-----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getLicence() const {
    static QString licence(tr("GPL2"));
    return licence;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getLicenceHolder()
//-----------------------------------------------------------------------------
QString const& MCAPICodeGenerator::getLicenceHolder() const {
    static QString holder(tr("Public"));
    return holder;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getSettingsWidget()
//-----------------------------------------------------------------------------
PluginSettingsWidget* MCAPICodeGenerator::getSettingsWidget()
{
    return new PluginSettingsWidget();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getIcon()
//-----------------------------------------------------------------------------
QIcon MCAPICodeGenerator::getIcon() const
{
    return QIcon(":icons/mcapi-generator.png");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool MCAPICodeGenerator::checkGeneratorSupport( QSharedPointer<LibraryComponent const> libComp,
    QSharedPointer<LibraryComponent const> libDesConf,
    QSharedPointer<LibraryComponent const> libDes ) const {

    // MCAPI code generator is only run for SW component editor
    /*if (libDesConf) {
        return false;
    }*/
    
    QSharedPointer<Component const> comp = libComp.dynamicCast<Component const>();
    QSharedPointer<DesignConfiguration const> desgConf = libDesConf.dynamicCast<DesignConfiguration const>();

    return (comp != 0 && comp->getComponentImplementation() == KactusAttribute::KTS_SW) ||
        ( libDes != 0 && desgConf != 0 && desgConf->getDesignConfigImplementation() == KactusAttribute::KTS_SYS );
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::runGenerator()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::runGenerator( IPluginUtility* utility, 
    QSharedPointer<LibraryComponent> libComp,
    QSharedPointer<LibraryComponent> libDesConf,
    QSharedPointer<LibraryComponent> libDes)
{
    utility_ = utility;

    QSharedPointer<const Design> design = libDes.dynamicCast<Design const>();
    QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
    QSharedPointer<DesignConfiguration const> desgConf = libDesConf.dynamicCast<DesignConfiguration const>();

    if ( comp != 0 && comp->getComponentImplementation() == KactusAttribute::KTS_SW  )
    {
        QString dir = QFileInfo(utility->getLibraryInterface()->getPath(*libComp->getVlnv())).absolutePath(); 
        generateMCAPIForComponent(dir, comp);
    }
    else if ( libDes != 0 && desgConf != 0 &&
        desgConf->getDesignConfigImplementation() == KactusAttribute::KTS_SYS )
    {
        generateTopLevel(design, comp, desgConf);
    }

    utility_->getLibraryInterface()->writeModelToFile(libComp);

    utility_->printInfo( "MCAPI generation complete.");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getProgramRequirements()
//-----------------------------------------------------------------------------
QList<IPlugin::ExternalProgramRequirement> MCAPICodeGenerator::getProgramRequirements() {
    return QList<IPlugin::ExternalProgramRequirement>();
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
// Function: MCAPICodeGenerator::canGenerateMCAPIComponent()
//-----------------------------------------------------------------------------
bool MCAPICodeGenerator::canGenerateMCAPIComponent(QSharedPointer<Component> component)
{
    // Will not generate a null component.
    if ( component == 0 )
    {
        return false;
    }

    // Errors found in interfaces.
    QStringList errorList;
    // Must have at least one MCAPI interface to generate.
    bool hasMcapi = false;

    // Go over each com interface and check for that each required property is set.
    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        // Check API support for generated interface
        if (comIf->getComType().getName().toLower() == "mcapi" )
        {
            hasMcapi = true;
        }

        QSharedPointer<LibraryComponent> libCom = utility_->getLibraryInterface()->getModel(comIf->getComType());
        QSharedPointer<ComDefinition> comDef = libCom.dynamicCast<ComDefinition>();

        checkRequiredPropertiesSet(component->getVlnv()->toString(), comDef, comIf, errorList);
    }

    // If errors exist, print about it and return false.
    if ( hasMcapi && !errorList.isEmpty())
    {
        foreach (QString const& msg, errorList)
        {
            utility_->printError(msg);
        }

        QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
            tr("The component contained %1 error(s). MCAPI code was not generated.").arg(errorList.size()),
            QMessageBox::Ok, utility_->getParentWidget());
        msgBox.setDetailedText(tr("The following error(s) were found: \n* ") + errorList.join("\n* "));

        msgBox.exec();
    }

    // If no errors, we can generate the component.
    return errorList.isEmpty() && hasMcapi;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::checkRequiredPropertiesSet()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::checkRequiredPropertiesSet(QString componentVLNV, QSharedPointer<ComDefinition> comDef,
    QSharedPointer<ComInterface> comIf, QStringList &errorList)
{
    foreach ( QSharedPointer<ComProperty> property, comDef->getProperties() )
    {
        if ( property->isRequired() && comIf->getPropertyValues().value(property->getName()).isEmpty() )
        {
            errorList.append(tr("Property %1 of COM interface '%2' is not set in component '%3'").
                arg(property->getName(),
                comIf->getName(),componentVLNV));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateMCAPIForComponent()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateMCAPIForComponent(QString dir, QSharedPointer<Component> component)
{
    // Update the ktsmcapicode module.
    generateMainTemplate(dir + "/main.c", component);
    generateHeader(dir + "/ktsmcapicode.h", component);
    generateSource(dir + "/ktsmcapicode.c", component);

    // Add the files to the component metadata.
    QSharedPointer<FileSet> fileSet = component->getFileSet("generatedMCAPI");
    fileSet->setGroups("sourceFiles");

    QList<QSharedPointer<FileBuilder>> fblist = fileSet->getDefaultFileBuilders();

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
        cs.setLabelText(  tr("Select a software view which shall include reference to the generated MCAPI") );

        QString viewName = cs.execDialog();

        //QString viewName = ComboSelector::selectView(component, utility_->getParentWidget(), QString(),
        //    tr("Select a software view which shall include reference to the generated MCAPI"));

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
// Function: MCAPICodeGenerator::generateHeader()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateHeader(QString const& filename, QSharedPointer<Component> component)
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

    writeLocalEndpoints(writer, component, true);
    writeRemoteEndpoints(writer, component, true);

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
void MCAPICodeGenerator::generateSource(QString const& filename, QSharedPointer<Component> component)
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
    writeLocalPorts(writer, component);

    writer.writeEmptyLine();

    writeLocalEndpoints(writer, component, false);
    writeRemoteEndpoints(writer, component, false);

    writer.writeEmptyLine();

    // Write the other variables.
    writer.writeLine("// Other variables.");
    writer.writeLine("mcapi_status_t status;");
    writer.writeLine("char status_msg[MCAPI_MAX_STATUS_MSG_LEN];");
    writer.writeEmptyLine();

    // Generate functions.
    generateInitializeMCAPIFunc(writer, component);
    generateConnectChannelsFunc(writer, component);
    generateOpenConnectionsFunc(writer, component);
    generateCloseConnectionsFunc(writer, component);

    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeLocalPorts()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeLocalPorts(CSourceWriter &writer, QSharedPointer<Component> component)
{
    writer.writeLine("// Local port IDs.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        writer.writeLine("const mcapi_port_t " + comIf->getName().toUpper() + "_PORT = " +
            comIf->getPropertyValues().value("port_id") + ";");
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeLocalEndpoints()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeLocalEndpoints(CSourceWriter &writer, QSharedPointer<Component> component,
    bool isExtern)
{
    writer.writeLine("// Local endpoints.");

    QString externString;

    if ( isExtern )
    {
        externString = "extern ";
    }

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        writer.writeLine(externString + "mcapi_endpoint_t " + comIf->getName() + ";");

        QString handleName = comIf->getPropertyValues().value("handle_name");

        if (comIf->getTransferType() == "packet")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine(externString + "mcapi_pktchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine(externString + "mcapi_pktchan_recv_hndl_t " + handleName + ";");
            }
        }
        else if (comIf->getTransferType() == "scalar")
        {
            if (comIf->getDirection() == General::OUT)
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
void MCAPICodeGenerator::writeRemoteEndpoints(CSourceWriter &writer, QSharedPointer<Component> component,
    bool isExtern)
{
    writer.writeLine("// Remote endpoints.");

    QString externString;

    if ( isExtern )
    {
        externString = "extern ";
    }

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        QString remoteEndpointName = comIf->getPropertyValues().value("remote_endpoint_name", "");
        writer.writeLine(externString + "mcapi_endpoint_t " + remoteEndpointName + ";");
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateMainTemplate()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateMainTemplate(QString const& filename, QSharedPointer<Component> /*component*/)
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
void MCAPICodeGenerator::generateInitializeMCAPIFunc(CSourceWriter& writer, QSharedPointer<Component> component)
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

    createLocalEndpoints(writer, component);

    getRemoteEndpoints(writer, component);

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
void MCAPICodeGenerator::createLocalEndpoints(CSourceWriter &writer, QSharedPointer<Component> component)
{
    writer.writeLine("// Create local endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        writer.writeLine(comIf->getName() + " = mcapi_endpoint_create(" +
            comIf->getName().toUpper() + "_PORT, &status);");
        writer.writeEmptyLine();

        writer.writeLine("if (status != MCAPI_SUCCESS)");
        writer.beginBlock();

        writer.writeLine("mcapi_display_status( status, status_msg, MCAPI_MAX_STATUS_MSG_LEN );");
        writer.writeLine("fprintf(stderr, \"ERROR: %s Failed to create endpoint " + comIf->getName() + "\"" );
        writer.writeLine( "\" at line %u.\\n\", status_msg, __LINE__ );");
        writer.writeLine("return -1;");
        writer.endBlock();
        writer.writeEmptyLine();
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::getRemoteEndpoints()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::getRemoteEndpoints(CSourceWriter &writer, QSharedPointer<Component> component)
{
    writer.writeLine("// Retrieve remote endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        writer.writeLine(comIf->getPropertyValues().value("remote_endpoint_name") + " = mcapi_endpoint_get(" +
            comIf->getPropertyValues().value("remote_endpoint_name").toUpper() + "_DOMAIN" +
            ", " + comIf->getPropertyValues().value("remote_endpoint_name").toUpper() + "_NODE," );
        writer.writeLine( comIf->getPropertyValues().value("remote_endpoint_name").toUpper() +
            "_PORT, TIMEOUT, &status);");
        writer.writeEmptyLine();

        writer.writeLine("if (status != MCAPI_SUCCESS)");
        writer.beginBlock();
        writer.writeLine("mcapi_display_status( status, status_msg, MCAPI_MAX_STATUS_MSG_LEN );");
        writer.writeLine("fprintf(stderr, \"ERROR: %s Failed to get remote endpoint " + comIf->getName() + "\"" );
        writer.writeLine("\" at line %u.\\n\", status_msg, __LINE__ );");
        writer.writeLine("return -1;");
        writer.endBlock();
        writer.writeEmptyLine();
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateConnectChannelsFunc()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateConnectChannelsFunc(CSourceWriter& writer, QSharedPointer<Component> component)
{
    // Write the connectChannels() function.
    writer.writeHeaderComment("Function: connectChannels()");
    writer.writeLine("int connectChannels()");

    // Must have request for each interface, and a value used to check if it opened already.
    QString interfaceCount = QString::number( component->getComInterfaces().size());

    writer.beginBlock();
    writeConnectionVariables(writer, interfaceCount, "connect");
    writer.writeEmptyLine();

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        // Sender is responsible of calling the connect function.
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }
        
        if ( comIf->getDirection() == General::OUT )
        {
            if (comIf->getTransferType() == "packet")
            {
                writeCon(comIf, writer, "pkt");
            }
            else if (comIf->getTransferType() == "scalar")
            {
                writeCon(comIf, writer, "scl");
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
void MCAPICodeGenerator::generateOpenConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component)
{
    // Write the openConnections() function.
    writer.writeHeaderComment("Function: openConnections()");
    writer.writeLine("int openConnections()");

    // Must have request for each interface, and a value used to check if it opened already.
    QString interfaceCount = QString::number( component->getComInterfaces().size()) ;

    writer.beginBlock();
    writeConnectionVariables(writer, interfaceCount, "open");
    writer.writeEmptyLine();

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        if (comIf->getTransferType() == "packet")
        {
            writeOpen(comIf, writer, "pkt");
        }
        else if (comIf->getTransferType() == "scalar")
        {
            writeOpen(comIf, writer, "scl");
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
void MCAPICodeGenerator::generateCloseConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component)
{
    // Write the closeConnections() function.
    writer.writeHeaderComment("Function: closeConnections()");
    writer.writeLine("int closeConnections()");

    // Must have request for each interface, and a value used to check if it opened already.
    QString interfaceCount = QString::number( component->getComInterfaces().size()) ;

    writer.beginBlock();
    writeConnectionVariables(writer, interfaceCount, "close");

    writer.writeEmptyLine();

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" )
        {
            continue;
        }

        if (comIf->getTransferType() == "packet")
        {
            writeClose(comIf, writer, "pkt");
        }
        else if (comIf->getTransferType() == "scalar")
        {
            writeClose(comIf, writer, "scl");
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
void MCAPICodeGenerator::writeCon(QSharedPointer<ComInterface> comIf, CSourceWriter &writer, QString name)
{
    writer.writeLine("mcapi_" + name + "chan_connect_i(" + comIf->getName() + ", " +
        comIf->getPropertyValues().value("remote_endpoint_name") + ", &request, &status);");

    writer.writeEmptyLine();
    writePendStatusCheck(writer, "connect");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeConOpen()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeOpen(QSharedPointer<ComInterface> comIf, CSourceWriter &writer, QString name)
{
    if (comIf->getDirection() == General::OUT)
    {
        writer.writeLine("mcapi_" + name + "chan_send_open_i(&" + comIf->getPropertyValues().value("handle_name") +
            ", " + comIf->getName() + ", &request, &status);");
    }
    else if (comIf->getDirection() == General::IN)
    {
        writer.writeLine("mcapi_" + name + "chan_recv_open_i(&" + comIf->getPropertyValues().value("handle_name") +
            ", " + comIf->getName() + ", &request, &status);");
    }

    writer.writeEmptyLine();
    writePendStatusCheck(writer, "open");
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeClose()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeClose(QSharedPointer<ComInterface> comIf, CSourceWriter &writer, QString name)
{
    if (comIf->getDirection() == General::OUT)
    {
        writer.writeLine("mcapi_" + name + "chan_send_close_i(" + comIf->getPropertyValues().value("handle_name") +
            ", &request, &status);");
    }
    else if (comIf->getDirection() == General::IN)
    {
        writer.writeLine("mcapi_" + name + "chan_recv_close_i(" + comIf->getPropertyValues().value("handle_name") +
            ", &request, &status);");
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

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateTopLevel()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateTopLevel(QSharedPointer<const Design> design, QSharedPointer<Component> topComponent,
    QSharedPointer<DesignConfiguration const> desgConf)
{
    foreach ( SWInstance instance, design->getSWInstances() )
    {
        VLNV instanceVLNV = instance.getComponentRef();
        VLNV* topVLNV = topComponent->getVlnv();
        VLNV* designVLNV = design->getVlnv();

        QSharedPointer<LibraryComponent> instanceLibComp = utility_->getLibraryInterface()->getModel(instanceVLNV);
        QSharedPointer<Component> instanceComp = instanceLibComp.dynamicCast<Component>();

        // Check if can generate the component, return if cannot
        if ( canGenerateMCAPIComponent(instanceComp) )
        {
            QString subDir = "/sw/" + instance.getInstanceName() + "/";
            QString dir = QFileInfo(utility_->getLibraryInterface()->getPath(*designVLNV)).absolutePath() + subDir;
            QString topDir = QFileInfo(utility_->getLibraryInterface()->getPath(*topVLNV)).absolutePath();

            generateInstanceHeader(dir, instance, instanceComp, design);

            addGeneratedMCAPIToFileset(General::getRelativePath(topDir,dir), topComponent, instance.getInstanceName(), desgConf);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: generateInstanceHeader()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateInstanceHeader(QString& directory, SWInstance& ourInstance,
    QSharedPointer<Component> component, QSharedPointer<const Design> design)
{
    // Create folder for the instance.
    QDir path;
    path.mkpath( directory );

    // Open the file for writing.
    QString const filename = directory + "instanceheader.h";
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

    writeLocalNodeAndDomain(writer, ourInstance);

    writeEndpointDefinitions(design, ourInstance, component, writer, endpointDefs);

    writeEndpointDefList(writer, endpointDefs);

    writer.writeLine("#endif");

    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::addGeneratedMCAPIToFileset()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::addGeneratedMCAPIToFileset(QString directory, QSharedPointer<Component> topComponent,
    QString instanceName, QSharedPointer<DesignConfiguration const> desgConf)
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

    QString fileSetName = sysViewName + "_" + instanceName + "_headers";

    // Add the files to the component metadata.
    QSharedPointer<FileSet> fileSet = topComponent->getFileSet(fileSetName);
    fileSet->setGroups("sourceFiles");

    QSettings settings;

    QSharedPointer<File> file;
    QStringList types;
    types.append("cSource");
    QString filePath = directory + "/instanceheader.h";

    if (!fileSet->contains(filePath))
    {
        file = fileSet->addFile(filePath, settings);
        file->setAllFileTypes( types );
        file->setIncludeFile( true );
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeLocalNodeAndDomain()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeLocalNodeAndDomain(CSourceWriter& writer, SWInstance& ourInstance)
{
    QString ourNodeID = ourInstance.getPropertyValues().value("Node ID");
    QString ourDomainID = ourInstance.getPropertyValues().value("Domain ID");

    writer.writeLine("//Local instance specific data:");
    writer.writeLine("#define LOCAL_NODE_ID " + ourNodeID);
    writer.writeLine("#define LOCAL_DOMAIN_ID " + ourDomainID);

    writer.writeEmptyLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::findConnectedComInterfaces()
//-----------------------------------------------------------------------------
QList<QPair<QSharedPointer<ComInterface>, ComInterfaceRef> > MCAPICodeGenerator::findConnectedComInterfaces(
    QSharedPointer<const Design> design, SWInstance &ourInstance, QSharedPointer<Component> component )
{
    QList<QPair<QSharedPointer<ComInterface>, ComInterfaceRef> > connectedInterfaces;

    // Go through the list of connections in the design to retrieve remote endpoint identifiers.
    foreach ( ComConnection connection, design->getComConnections() )
    {
        // See which end on the connection is NOT ours.
        if ( ourInstance.getInstanceName() == connection.getInterface1().componentRef )
        {
            QSharedPointer<ComInterface> ourInterface = component->
                getComInterface( connection.getInterface1().comRef );

            // If our interface was not found we should not generate remote endpoint identifier for it.
            if ( !ourInterface.isNull() )
            {
                QPair<QSharedPointer<ComInterface>, ComInterfaceRef> pair( ourInterface,
                    connection.getInterface2() );
                connectedInterfaces.append(pair);
            }
        }
        
        if ( ourInstance.getInstanceName() == connection.getInterface2().componentRef )
        {
            QSharedPointer<ComInterface> ourInterface = component->
                getComInterface( connection.getInterface2().comRef );

            // If our interface was not found we should not generate remote endpoint identifier for it.
            if ( !ourInterface.isNull() )
            {
                QPair<QSharedPointer<ComInterface>, ComInterfaceRef> pair( ourInterface,
                    connection.getInterface1() );
                connectedInterfaces.append(pair);
            }
        }
    }

    return connectedInterfaces;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeEndpointDefinitions()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeEndpointDefinitions(QSharedPointer<const Design> design, SWInstance &ourInstance,
    QSharedPointer<Component> component, CSourceWriter& writer, QStringList& endpointDefs)
{
    QPair<QSharedPointer<ComInterface>, ComInterfaceRef> conPair;
    foreach( conPair, findConnectedComInterfaces(design, ourInstance, component) )
    {
        // Search for software instance corresponding theirs.
        SWInstance targetInstance = searchInstance(design, conPair.second.componentRef);

        // Obtain the component object corresponding the software instance.
        VLNV instanceVLNV = targetInstance.getComponentRef();
        QSharedPointer<LibraryComponent> instanceLibComp = utility_->getLibraryInterface()->getModel(instanceVLNV);
        QSharedPointer<Component> instanceComp = instanceLibComp.dynamicCast<Component>();

        if ( instanceComp == 0 )
        {
            continue;
        }

        // Get the interfaces.
        QSharedPointer<ComInterface> targetInterface = instanceComp->getComInterface(conPair.second.comRef);
        QSharedPointer<ComInterface> ourInterface = conPair.first;

        // Must find it to generate.
        if ( targetInterface != 0 )
        {
            checkEndpointIdentifier(targetInterface, targetInstance);
            checkTransferType(ourInterface, targetInterface, ourInstance, targetInstance);
            checkScalarSize(ourInterface, targetInterface, ourInstance, targetInstance);

            writeRemoteEndpoint(writer, ourInstance, targetInstance, ourInterface, targetInterface, endpointDefs);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeRemoteEndpoint()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeRemoteEndpoint(CSourceWriter& writer, SWInstance& ourInstance,
    SWInstance& targetInstance, QSharedPointer<ComInterface> ourInterface, QSharedPointer<ComInterface>
    targetInterface, QStringList& endpointDefs)
{
    QString ourPortID = ourInterface->getPropertyValues().value("port_id").toUpper();
    QString ourDomainID = ourInstance.getPropertyValues().value("Domain ID");
    QString ourNodeID = ourInstance.getPropertyValues().value("Node ID");
    QString remoteName = ourInterface->getPropertyValues().value("remote_endpoint_name").toUpper();

    QString theirPortID = targetInterface->getPropertyValues().value("port_id").toUpper();
    QString theirDomainID = targetInstance.getPropertyValues().value("Domain ID");
    QString theirNodeID = targetInstance.getPropertyValues().value("Node ID");

    // Direction and transfer type must convert to correct format.
    QString ourTransferString = transferTypeToEString(ourInterface->getTransferType());
    QString ourDirectionString = transferDirectionToEString(ourInterface->getDirection());
    QString theirTransferString = transferTypeToEString(targetInterface->getTransferType());
    QString theirDirectionString = transferDirectionToEString(targetInterface->getDirection());

    // Read the sizes of scalars.
    QString ourScalarSize = ourInterface->getPropertyValues().value("scalar_size");
    QString theirScalarSize = targetInterface->getPropertyValues().value("scalar_size");

    // Generate the endpoint identifier for the remote endpoint.
    writer.writeLine(" //" + remoteName + " endpoint identifier:");
    writer.writeLine("#define " + remoteName + "_DOMAIN "+ theirDomainID);
    writer.writeLine("#define " + remoteName + "_NODE "+ theirNodeID);
    writer.writeLine("#define " + remoteName + "_PORT "+ theirPortID);

    writer.writeEmptyLine();

    // Generate identifier strings corresponding the endpoint identifiers.
    QString theirIdString = makeIdString(theirDomainID, theirNodeID, theirPortID);
    QString ourIdString = makeIdString(ourDomainID, ourNodeID, ourPortID);

    // Create endpoint definitions.
    QString ourEndpointDef = ourInterface->getName().toUpper() + "_DEF";
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
// Function: MCAPICodeGenerator::writeEndpointDefList()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeEndpointDefList(CSourceWriter& writer, QStringList& endpointDefs)
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
// Function: MCAPICodeGenerator::searchInstance()
//-----------------------------------------------------------------------------
SWInstance MCAPICodeGenerator::searchInstance(QSharedPointer<const Design> design, QString instanceName)
{
    SWInstance targetInstance;

    foreach ( SWInstance instance, design->getSWInstances() )
    {
        // If the found instance name is same as target, will generate for it.
        if ( instance.getInstanceName() == instanceName )
        {
            return instance;
        }
    }

    return targetInstance;
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::transferDirectionToEString()
//-----------------------------------------------------------------------------
QString MCAPICodeGenerator::transferDirectionToEString(General::Direction direction)
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
// Function: MCAPICodeGenerator::transferTypeToEString()
//-----------------------------------------------------------------------------
QString MCAPICodeGenerator::transferTypeToEString(QString transferType)
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
// Function: MCAPICodeGenerator::makeIdString()
//-----------------------------------------------------------------------------
QString MCAPICodeGenerator::makeIdString(QString domainID, QString nodeID, QString portID)
{
    return "{" + domainID + ", " + nodeID + ", " + portID + "}";
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::checkEndpointIdentifier()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::checkEndpointIdentifier(QSharedPointer<ComInterface> targetInterface,
    SWInstance &targetInstance)
{
    QString theirPortID = targetInterface->getPropertyValues().value("port_id").toUpper();
    QString theirDomainID = targetInstance.getPropertyValues().value("Domain ID");
    QString theirNodeID = targetInstance.getPropertyValues().value("Node ID");

    if ( theirPortID.isEmpty() || theirDomainID.isEmpty() || theirNodeID.isEmpty() )
    {
        utility_->printError("Could not find whole endpoint identifier for instance "
            + targetInstance.getInstanceName() + " interface " + targetInterface->getName()
            + " Found domain: " + theirDomainID + " node: " + theirNodeID + " port: " + theirPortID);
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::checkTransferType()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::checkTransferType(QSharedPointer<ComInterface> ourInterface, QSharedPointer<ComInterface>
    targetInterface, SWInstance &ourInstance, SWInstance &targetInstance)
{
    if ( ourInterface->getTransferType() != targetInterface->getTransferType() )
    {
        utility_->printError("Transfer types of connected endpoints did not match! "
            "First instance: " + ourInstance.getInstanceName() + " first interface: "
            + ourInterface->getName() + " Second instance: " + targetInstance.getInstanceName() + 
            " second interface " + targetInterface->getName());
    }
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::checkScalarSize()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::checkScalarSize(QSharedPointer<ComInterface> ourInterface, QSharedPointer<ComInterface>
    targetInterface, SWInstance &ourInstance, SWInstance &targetInstance)
{
    QString ourScalarSize = ourInterface->getPropertyValues().value("scalar_size");
    QString theirScalarSize = targetInterface->getPropertyValues().value("scalar_size");

    if ( ourScalarSize != theirScalarSize )
    {
        utility_->printError("Scalar sizes of connected endpoints did not match! "
            "First instance: " + ourInstance.getInstanceName() + " first interface: "
            + ourInterface->getName() + " Second instance: " + targetInstance.getInstanceName() + 
            " second interface " + targetInterface->getName());
    }
}