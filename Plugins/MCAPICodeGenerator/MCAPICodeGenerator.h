//-----------------------------------------------------------------------------
// File: MCAPICodeGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// MCAPI code generator.
//-----------------------------------------------------------------------------

#ifndef MCAPICODEGENERATOR_H
#define MCAPICODEGENERATOR_H

#include <IPXACTmodels/ComDefinition.h>
#include <IPXACTmodels/ComInterface.h>
#include <Plugins/PluginSystem/IPluginUtility.h>

#include <MCAPIParser.h>

class Component;
class CSourceWriter;

//-----------------------------------------------------------------------------
//! MCAPI code generator.
//-----------------------------------------------------------------------------
class MCAPICodeGenerator
{
public:
    MCAPICodeGenerator( MCAPIParser& parser, IPluginUtility* utility );
    ~MCAPICodeGenerator();

    /*!
     *  Generates MCAPI code for the given component.
     *
     *      @param [in] QString dir   The directory for the generated code.
     *      @param [in] QSharedPointer<Component> component   The component which MCAPI code will be generated.
     */
    void generateMCAPIForComponent(QString dir, QSharedPointer<Component> component);

private:

    /*!
     *  Creates the indent string based on the application settings.
     */
    QString createIndentString();

    /*!
     *  Checks if MCAPI code may be generated for the given component.
     *
     *      @param [in] component   The component, which is checked.
     *      @return   True, if MCAPI code can be generated for the component.
     */
    bool canGenerateMCAPIComponent(QSharedPointer<Component> component);

    /*!
     *  Checks if all properties required in given ComDefintion are set in the given ComInterface.
     *
     *      @param [in] comDef   The ComDefinition under inspection.
     *      @param [in] comIf   The ComInterface under inspection.
     *      @param [in] errorList   Missing properties will be reported in this list.
     */
     void checkRequiredPropertiesSet(QString componentVLNV, QSharedPointer<ComDefinition> comDef,
         QSharedPointer<ComInterface> comIf,  QStringList &errorList);

    /*!
     *  Generates the MCAPI code header file.
     *
     *      @param [in] filename   The name of the header file to write.
     */
     void generateHeader(QString const& filename);

    /*!
     *  Writes function declarations for the generated code.
     *
     *      @param [in] writer   The source writer.
     */
     void writeFunctionDeclarations(CSourceWriter &writer);

    /*!
     *  Generates the MCAPI code source file.
     *
     *      @param [in] filename   The name of the source file to write.
     */
     void generateSource(QString const& filename);

    /*!
     *   Generates port identifiers for the local endpoints.
     *
     *      @param [in] writer   The source writer.
     */
     void writeLocalPorts(CSourceWriter &writer);

    /*!
     *  Writes declarations for remote endpoint variables and handles.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] isExtern   True, if extern is added to variable declaration.
     */
     void writeRemoteEndpoints(CSourceWriter &writer, bool isExtern);

    /*!
     *  Writes declarations for local endpoint variables.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] isExtern   True, if extern is added to variable declaration.
     */
     void writeLocalEndpoints(CSourceWriter &writer, bool isExtern);

    /*!
     *  Generates the template source containing the program main().
     *
     *      @param [in] filename   The name of the file to write.
     */
     void generateMainTemplate(QString const& filename);

    /*!
     *  Generates the initializeMCAPI() function based on the component.
     *
     *      @param [in] writer     The source writer.
     */
     void generateInitializeMCAPIFunc(CSourceWriter& writer);

    /*!
     *  Generates the node initialization code.
     *
     *      @param [in] writer   The source writer.
     */
     void createInitialization(CSourceWriter &writer);

    /*!
     *   Generates code to get remote endpoints for the given component.
     *
     *      @param [in] writer   The source writer.
     */
     void getRemoteEndpoints(CSourceWriter &writer);

    /*!
     *  Generates code to create local endpoints for the given component.
     *
     *      @param [in] writer   The source writer.
     */
     void createLocalEndpoints(CSourceWriter &writer);

    /*!
     *  Generates the connectChannels() function based on the component.
     *
     *      @param [in] writer     The source writer.
     */
     void generateConnectChannelsFunc(CSourceWriter& writer);

    /*!
     *  Generates the openConnections() function based on the component.
     *
     *      @param [in] writer     The source writer.
     */
     void generateOpenConnectionsFunc(CSourceWriter& writer);

    /*!
     *  Generates the closeConnections() function based on the component.
     *
     *      @param [in] writer     The source writer.
     */
     void generateCloseConnectionsFunc(CSourceWriter& writer);

    /*!
     *  Generates variables used in connection, open, or close function.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] interfaceCount   Number of interfaces in the component.
     *      @param [in] name   Name of the operation, ideally 'connect', 'open', or 'close'.
     */
     void writeConnectionVariables(CSourceWriter &writer, QString interfaceCount, QString name);

    /*!
     *  Writes connection call for the given interface.
     *
     *      @param [in] comIf   Interface, which connection call is written.
     *      @param [in] writer   The source writer.
     *      @param [in] name   Name of transfer type, ideally 'pkt' or 'scl'.
     */
     void writeCon(MCAPIParser::EndPointData epd, CSourceWriter &writer, QString name);

    /*!
     *  Writes open call for the given interface.
     *
     *      @param [in] comIf   Interface, which open call is written.
     *      @param [in] writer   The source writer.
     *      @param [in] name   Name of transfer type, ideally 'pkt' or 'scl'.
     */
     void writeOpen(MCAPIParser::EndPointData epd, CSourceWriter &writer, QString name);

    /*!
     *  Writes close call for the given interface.
     *
     *      @param [in] comIf   Interface, which close call is written.
     *      @param [in] writer   The source writer.
     *      @param [in] name   Name of transfer type, ideally 'pkt' or 'scl'.
     */
     void writeClose(MCAPIParser::EndPointData epd, CSourceWriter &writer, QString name);

    /*!
     *  Writes loop which tries to connect, open or close for preset times.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] name   Name of the operation, ideally 'connect', 'open', or 'close'.
     */
     void writeRetryLoop(CSourceWriter &writer, QString name);

    /*!
     *  Call which waits for request for givent time.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] name   Name of the operation, ideally 'connect', 'open', or 'close'.
     */
     void writeIterationWait(CSourceWriter &writer, QString name);

    /*!
     *  Checks if all connect, open or close calls are complete.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] name   Name of the operation, ideally 'connect', 'open', or 'close'.
     */
     void writeCompleteCheck(CSourceWriter &writer, QString name);

    /*!
     *  Writes an MCAPI status check, which waits in case of pending.
     *
     *      @param [in] writer         The source writer.
     *      @param [in] name Name of the operation, ideally 'connect', 'open', or 'close'.
     */
    void writePendStatusCheck(CSourceWriter &writer, QString name);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Endpoints parsed from component
    QList<MCAPIParser::EndPointData> componentEndpoints_;
    //! Nodes parsed from design.
    QList<MCAPIParser::NodeData> designNodes_;
    //! The plugin utility.
    IPluginUtility* utility_;
};

#endif // MCAPICODEGENERATOR_H
