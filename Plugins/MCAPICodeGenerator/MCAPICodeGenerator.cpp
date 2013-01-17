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

#include <common/CSourceWriter.h>
#include <MCAPI/CSourceTextEdit.h>

#include <models/component.h>
#include <models/ComInterface.h>
#include <models/fileset.h>

#include <LibraryManager/libraryinterface.h>

#include <PluginSystem/IPluginUtility.h>

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
    static QString version("1.0");
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
// Function: MCAPICodeGenerator::checkGeneratorSupport()
//-----------------------------------------------------------------------------
bool MCAPICodeGenerator::checkGeneratorSupport(QSharedPointer<LibraryComponent> libComp) const
{
    QSharedPointer<Component> comp = libComp.dynamicCast<Component>();
    return (comp != 0 && comp->getComponentImplementation() == KactusAttribute::KTS_SW);
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::runGenerator()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::runGenerator(IPluginUtility* utility,
                                      QSharedPointer<LibraryComponent> libComp)
{
    utility_ = utility;

    QString dir = QFileInfo(utility->getLibraryInterface()->getPath(*libComp->getVlnv())).absolutePath();
    QSharedPointer<Component> component = libComp.dynamicCast<Component>();

    // Validate MCAPI COM interfaces.
    QStringList errorList;

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" ||
            comIf->getComType().getVersion() != "1.063")
        {
            continue;
        }

        if (comIf->getPropertyValues().value("handle_name").isEmpty())
        {
            errorList.append(tr("Property handle_name of COM interface '%1' is not set").arg(comIf->getName()));
        }

        if (comIf->getPropertyValues().value("remote_endpoint_name").isEmpty())
        {
            errorList.append(tr("Property remote_endpoint_name of COM interface '%1' is not set").arg(comIf->getName()));
        }

        if (comIf->getPropertyValues().value("port_id").isEmpty())
        {
            errorList.append(tr("Property port_id of COM interface '%1' is not set").arg(comIf->getName()));
        }
    }

    if (!errorList.isEmpty())
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
        return;
    }

    // Create the template only if it does not exist.
    if (!QFileInfo(dir + "/ktsmcapicode.h").exists())
    {
        generateMainTemplate(dir + "/main.c", component);
    }

    // Update the ktsmcapicode module.
    generateHeader(dir + "/ktsmcapicode.h", component);
    generateSource(dir + "/ktsmcapicode.c", component);

    // Add the files to the component metadata.
    FileSet* fileSet = component->getFileSet("cSources");

    if (fileSet == 0)
    {
        fileSet = new FileSet("cSources", "sourceFiles");
        component->addFileSet(fileSet);
    }

    if (!fileSet->contains("ktsmcapicode.h"))
    {
        fileSet->addFile("ktsmcapicode.h");
    }

    if (!fileSet->contains("ktsmcapicode.c"))
    {
        fileSet->addFile("ktsmcapicode.c");
    }

    if (!fileSet->contains("main.c"))
    {
        fileSet->addFile("main.c");
    }

    utility_->getLibraryInterface()->writeModelToFile(component);
}

//-----------------------------------------------------------------------------
// Function: generateHeader()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateHeader(QString const& filename, QSharedPointer<Component> component)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    // Write the Kactus 2 generated code.
    writer.writeLine("// DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeLine();

    writer.writeLine("#ifndef KTSMCAPICODE_H");
    writer.writeLine("#define KTSMCAPICODE_H");
    writer.writeLine();

    writer.writeInclude("mcapi.h");
    writer.writeLine();

    writer.writeHeaderComment("Node data.");
    writer.writeLine();

    // Write local endpoint variables.
    writer.writeLine("// Local endpoints.");
    
    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" ||
            comIf->getComType().getVersion() != "v1.063")
        {
            continue;
        }

        writer.writeLine("extern mcapi_endpoint_t " + comIf->getName() + ";");

        QString handleName = comIf->getPropertyValues().value("handle_name");

        if (comIf->getTransferType() == "packet")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("extern mcapi_pktchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine("extern mcapi_pktchan_recv_hndl_t " + handleName + ";");
            }
        }
        else if (comIf->getTransferType() == "scalar")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("extern mcapi_sclchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine("extern mcapi_sclchan_recv_hndl_t " + handleName + ";");
            }
        }

        writer.writeLine();
    }

    // Write remote endpoint variables.
    writer.writeLine("// Remote endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" ||
            comIf->getComType().getVersion() != "v1.063")
        {
            continue;
        }

        QString remoteEndpointName = comIf->getPropertyValues().value("remote_endpoint_name", "");
        writer.writeLine("extern mcapi_endpoint_t " + remoteEndpointName + ";");
    }

    writer.writeLine();

    // Write the other variables.
    writer.writeLine("// Other variables.");
    writer.writeLine("extern mcapi_status_t status;");
    writer.writeLine();

    writer.writeHeaderComment("Functions.");
    writer.writeLine();

    // Write the initialization function prototype.
    writer.writeLine("/*");
    writer.writeLine(" *  Initializes the MCAPI system and endpoints.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int initializeMCAPI();");
    writer.writeLine();

    writer.writeLine("/*");
    writer.writeLine(" *  Creates and opens all connections.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int createConnections();");
    writer.writeLine();

    writer.writeLine("/*");
    writer.writeLine(" *  Closes all connections.");
    writer.writeLine(" *");
    writer.writeLine(" *        @return 0 if successful. -1 in case of an error.");
    writer.writeLine(" */");
    writer.writeLine("int closeConnections();");
    writer.writeLine();

    writer.writeLine("#endif // KTSMCAPICODE_H");
    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: generateSource()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateSource(QString const& filename, QSharedPointer<Component> component)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    // Write the Kactus 2 generated code.
    writer.writeLine("// DO NOT MODIFY THIS FILE. ALL CHANGES WILL BE OVERWRITTEN BY KACTUS2.");
    writer.writeLine();

    writer.writeInclude("ktsmcapicode.h", true);
    writer.writeLine();
    writer.writeHeaderComment("Constants.");
    writer.writeLine();

    // Write local port IDs.
    writer.writeLine("// Local port IDs.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" ||
            comIf->getComType().getVersion() != "v1.063")
        {
            continue;
        }
    
        writer.writeLine("const mcapi_port_t " + comIf->getName().toUpper() + "_PORT = " +
                         comIf->getPropertyValues().value("port_id") + ";");
    }

    writer.writeLine();

    writer.writeHeaderComment("Global variables.");
    writer.writeLine();

    // Write local endpoint variables.
    writer.writeLine("// Local endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" ||
            comIf->getComType().getVersion() != "v1.063")
        {
            continue;
        }
    
        writer.writeLine("mcapi_endpoint_t " + comIf->getName() + ";");

        QString handleName = comIf->getPropertyValues().value("handle_name");

        if (comIf->getTransferType() == "packet")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("mcapi_pktchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine("mcapi_pktchan_recv_hndl_t " + handleName + ";");
            }
        }
        else if (comIf->getTransferType() == "scalar")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("mcapi_sclchan_send_hndl_t " + handleName + ";");
            }
            else
            {
                writer.writeLine("mcapi_sclchan_recv_hndl_t " + handleName + ";");
            }
        }

        writer.writeLine();
    }

    // Write remote endpoint variables.
    writer.writeLine("// Remote endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" ||
            comIf->getComType().getVersion() != "v1.063")
        {
            continue;
        }

        QString remoteEndpointName = comIf->getPropertyValues().value("remote_endpoint_name", "");
        writer.writeLine("mcapi_endpoint_t " + remoteEndpointName + ";");
    }

    writer.writeLine();

    // Write the other variables.
    writer.writeLine("// Other variables.");
    writer.writeLine("mcapi_status_t status;");
    writer.writeLine();

    generateInitializeMCAPIFunc(writer, component);
    generateCreateConnectionsFunc(writer, component);
    generateCloseConnectionsFunc(writer, component);

    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateMainTemplate()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateMainTemplate(QString const& filename, QSharedPointer<Component> component)
{
    CSourceWriter writer(filename, createIndentString());

    if (!writer.open())
    {
        return;
    }

    // Write includes.
    writer.writeInclude("stdlib.h");
    writer.writeInclude("stdio.h");
    writer.writeLine();

    writer.writeLine("// This header includes the Kactus2 generated MCAPI code.");
    writer.writeInclude("ktsmcapicode.h");
    writer.writeLine();

    writer.writeLine("int main(int argc, char* argv[])");
    writer.beginBlock();
    writer.writeLine("// Initialize MCAPI.");

    writer.writeLine("if (initializeMCAPI() != 0)");
    writer.beginBlock();
    writer.writeLine("// TODO: Write error handling code.");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeLine();

    writer.writeLine("if (createConnections() != 0)");
    writer.beginBlock();
    writer.writeLine("// TODO: Write error handling code.");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeLine();

    writer.writeLine("// TODO: Write your application code here.");
    writer.writeLine();
    writer.writeLine("// Close connections and finalize MCAPI before exiting.");
    writer.writeLine("if (closeConnections() != 0)");
    writer.beginBlock();
    writer.writeLine("// TODO: Write error handling code.");
    writer.writeLine("return EXIT_FAILURE;");
    writer.endBlock();
    writer.writeLine();
    writer.writeLine("mcapi_finalize(&status);");
    writer.writeLine("return EXIT_SUCCESS;");

    writer.endBlock();
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
// Function: generateCreateConnectionsFunc()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateCreateConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component)
{
    // Write the createConnections() function.
    writer.writeHeaderComment("Function: createConnections()");
    writer.writeLine("int createConnections()");

    writer.beginBlock();
    writer.writeLine("mcapi_request_t request;");
    writer.writeLine("size_t size = 0;");
    writer.writeLine();

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" ||
            comIf->getComType().getVersion() != "v1.063")
        {
            continue;
        }

        if (comIf->getTransferType() == "packet")
        {
            if (comIf->getDirection() == General::OUT)
            {
                // Sender is responsible of calling the connect function.
                writer.writeLine("mcapi_connect_pktchan_i(" + comIf->getName() + ", " +
                                 comIf->getPropertyValues().value("remote_endpoint_name") + ", &request, &status);");
                writer.writeLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");

                writer.writeLine("mcapi_open_pktchan_send_i(&" + comIf->getPropertyValues().value("handle_name") +
                                 ", " + comIf->getName() + ", &request, &status);");
                writer.writeLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");
            }
            else
            {
                writer.writeLine("mcapi_open_pktchan_recv_i(&" + comIf->getPropertyValues().value("handle_name") +
                                 ", " + comIf->getName() + ", &request, &status);");
                writer.writeLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");
            }
        }
        else if (comIf->getTransferType() == "scalar")
        {
            if (comIf->getDirection() == General::OUT)
            {
                // Sender is responsible of calling the connect function.
                writer.writeLine("mcapi_connect_sclchan_i(" + comIf->getName() + ", " +
                    comIf->getPropertyValues().value("remote_endpoint_name") + ", &request, &status);");
                writer.writeLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");

                writer.writeLine("mcapi_open_sclchan_send_i(&" + comIf->getPropertyValues().value("handle_name") +
                                 ", " + comIf->getName() + ", &request, &status);");
                writer.writeLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");
            }
            else
            {
                writer.writeLine("mcapi_open_sclchan_recv_i(&" + comIf->getPropertyValues().value("handle_name") +
                                 ", " + comIf->getName() + ", &request, &status);");
                writer.writeLine();

                writeStatusCheck(writer);
                writeWaitCall(writer, "request", "size");
            }
        }
    }

    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::generateCloseConnectionsFunc()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::generateCloseConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component)
{
    // Write the createConnections() function.
    writer.writeHeaderComment("Function: closeConnections()");
    writer.writeLine("int closeConnections()");

    writer.beginBlock();
    writer.writeLine("mcapi_request_t request;");
    writer.writeLine("size_t size = 0;");
    writer.writeLine();

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" ||
            comIf->getComType().getVersion() != "v1.063")
        {
            continue;
        }

        if (comIf->getTransferType() == "packet")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("mcapi_pktchan_send_close_i(" + comIf->getPropertyValues().value("handle_name") +
                                 ", &request, &status);");
                writer.writeLine();
            }
            else
            {
                writer.writeLine("mcapi_pktchan_recv_close_i(" + comIf->getPropertyValues().value("handle_name") +
                    ", &request, &status);");
                writer.writeLine();
            }
        }
        else if (comIf->getTransferType() == "scalar")
        {
            if (comIf->getDirection() == General::OUT)
            {
                writer.writeLine("mcapi_sclchan_send_close_i(" + comIf->getPropertyValues().value("handle_name") +
                                 ", &request, &status);");
                writer.writeLine();
            }
            else
            {
                writer.writeLine("mcapi_sclchan_recv_close_i(" + comIf->getPropertyValues().value("handle_name") +
                                 ", &request, &status);");
                writer.writeLine();
            }
        }
    }

    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeLine();
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
    writer.writeLine("mcapi_version_t version;");
    writer.writeLine();
    writer.writeLine("// Initialize MCAPI implementation.");
    writer.writeLine("mcapi_initialize(LOCAL_NODE_ID, &version, &status);");
    writer.writeLine();

    writer.writeLine("if (status != MCAPI_SUCCESS)");
    writer.beginBlock();
    writer.writeLine("return -1;");
    writer.endBlock();
    writer.writeLine();

    // Write function calls to create local endpoints.
    writer.writeLine("// Create local endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" ||
            comIf->getComType().getVersion() != "v1.063")
        {
            continue;
        }

        writer.writeLine(comIf->getName() + " = mcapi_create_endpoint(" +
                         comIf->getName().toUpper() + "_PORT, &status);");
        writer.writeLine();

        writer.writeLine("if (status != MCAPI_SUCCESS)");
        writer.beginBlock();
        writer.writeLine("return -1;");
        writer.endBlock();
        writer.writeLine();
    }

    // Write function calls to retrieve remote endpoints.
    writer.writeLine("// Retrieve remote endpoints.");

    foreach (QSharedPointer<ComInterface> comIf, component->getComInterfaces())
    {
        if (comIf->getComType().getName().toLower() != "mcapi" ||
            comIf->getComType().getVersion() != "v1.063")
        {
            continue;
        }

        writer.writeLine(comIf->getPropertyValues().value("remote_endpoint_name") +
                         " = mcapi_get_endpoint(" +
                         comIf->getPropertyValues().value("remote_endpoint_name").toUpper() + "_NODE" +
                         ", " +
                         comIf->getPropertyValues().value("remote_endpoint_name").toUpper() + "_PORT, &status);");
        writer.writeLine();

        writer.writeLine("if (status != MCAPI_SUCCESS)");
        writer.beginBlock();
        writer.writeLine("return -1;");
        writer.endBlock();
        writer.writeLine();
    }

    writer.writeLine("return 0;");
    writer.endBlock();
    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeWaitCall()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeWaitCall(CSourceWriter& writer, QString const& requestName,
                                       QString const& sizeName)
{
    writer.writeLine("if (mcapi_wait(&" + requestName + ", &" + sizeName + ", &status, MCAPI_INFINITE) == MCAPI_FALSE)");
    writer.beginBlock();
    writer.writeLine("return -1;");
    writer.endBlock();
    writer.writeLine();
}

//-----------------------------------------------------------------------------
// Function: MCAPICodeGenerator::writeStatusCheck()
//-----------------------------------------------------------------------------
void MCAPICodeGenerator::writeStatusCheck(CSourceWriter &writer)
{
    writer.writeLine("if (status != MCAPI_SUCCESS)");
    writer.beginBlock();
    writer.writeLine("return -1;");
    writer.endBlock();
    writer.writeLine();
}


Q_EXPORT_PLUGIN2(MCAPICodeGenerator, MCAPICodeGenerator)