//-----------------------------------------------------------------------------
// File: McapiCodeGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// MCAPI code generator plugin.
//-----------------------------------------------------------------------------

#ifndef MCAPICODEGENERATOR_H
#define MCAPICODEGENERATOR_H

#include <Plugins/PluginSystem/IGeneratorPlugin.h>
#include <IPXACTmodels/ComDefinition.h>
#include <IPXACTmodels/ComInterface.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <QObject>

class Component;
class CSourceWriter;
class IPluginUtility;

//-----------------------------------------------------------------------------
//! MCAPI code generator.
//-----------------------------------------------------------------------------
class MCAPICodeGenerator : public QObject, public IGeneratorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.MCAPICodeGenerator" FILE "mcapi.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:
    MCAPICodeGenerator();
    ~MCAPICodeGenerator();

    /*!
     *  Returns the name of the plugin.
     */
    virtual QString const& getName() const;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString const& getVersion() const;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString const& getDescription() const;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString const& getVendor() const;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString const& getLicence() const;

    /*!
     *  Returns the holder of the licence of the plugin.
     */
    virtual QString const& getLicenceHolder() const;

    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const;

    /*!
     *  Checks whether the generator supports generation for the given library component.
     *
     *      @param [in] libComp The library component for which to check support.
     *      @param [in] libDesConf The optional design configuration object.
     *      @param [in] libDes The optional design object.
     *
     *      @return True, if the generator supports the given component. Otherwise false.
     */
     virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp, 
         QSharedPointer<LibraryComponent const> libDesConf = QSharedPointer<LibraryComponent const>(), 
         QSharedPointer<LibraryComponent const> libDes = QSharedPointer<LibraryComponent const>()) const;

    /*!
     *  Runs the generator.
     *
     *
     *      @param [in]     utility       The plugin utility interface.
     *      @param [in,out] libComp       The component for which the generator is run.
     *      @param [in,out] libDesConf        The optional design configuration object for the generator.
     *      @param [in,out] libDes            The optional design object.
     */
     virtual void runGenerator(IPluginUtility* utility,
         QSharedPointer<LibraryComponent> libComp, 
         QSharedPointer<LibraryComponent> libDesConf = QSharedPointer<LibraryComponent const>(),
         QSharedPointer<LibraryComponent> libDes = QSharedPointer<LibraryComponent>());

     //! \brief Returns the external program requirements of the plugin.
     virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

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
     *  Generates MCAPI code for the given component.
     *
     *      @param [in] QString dir   The directory for the generated code.
     *      @param [in] QSharedPointer<Component> component   The component which MCAPI code will be generated.
     *      @param [in] design   The design of the component instance.
     */
    void generateMCAPIForComponent(QString dir, QSharedPointer<Component> component);

    /*!
     *  Generates the MCAPI code header file.
     *
     *      @param [in] filename   The name of the header file to write.
     *      @param [in] component  The owner component.
     */
    void generateHeader(QString const& filename, QSharedPointer<Component> component);

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
     *      @param [in] component  The owner component.
     */
     void generateSource(QString const& filename, QSharedPointer<Component> component);

    /*!
     *   Generates port identifiers for the local endpoints.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] component   The component which MCAPI interfaces are used for generation.
     */
     void writeLocalPorts(CSourceWriter &writer, QSharedPointer<Component> component);

    /*!
     *  Writes declarations for remote endpoint variables and handles.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] component   The component which MCAPI interfaces are used for generation.
     *      @param [in] isExtern   True, if extern is added to variable declaration.
     */
     void writeRemoteEndpoints(CSourceWriter &writer, QSharedPointer<Component> component, bool isExtern);

    /*!
     *  Writes declarations for local endpoint variables.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] component   The component which MCAPI interfaces are used for generation.
     *      @param [in] isExtern   True, if extern is added to variable declaration.
     */
     void writeLocalEndpoints(CSourceWriter &writer, QSharedPointer<Component> component, bool isExtern);

    /*!
     *  Generates the template source containing the program main().
     *
     *      @param [in] filename   The name of the file to write.
     *      @param [in] component  The owner component.
     */
    void generateMainTemplate(QString const& filename, QSharedPointer<Component> component);

    /*!
     *  Generates the initializeMCAPI() function based on the component.
     *
     *      @param [in] writer     The source writer.
     *      @param [in] component  The owner component.
     */
    void generateInitializeMCAPIFunc(CSourceWriter& writer, QSharedPointer<Component> component);

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
     *      @param [in] component   The owner component.
     */
     void getRemoteEndpoints(CSourceWriter &writer, QSharedPointer<Component> component);

    /*!
     *  Generates code to create local endpoints for the given component.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] component   The owner component.
     */
     void createLocalEndpoints(CSourceWriter &writer, QSharedPointer<Component> component);

    /*!
     *  Generates the connectChannels() function based on the component.
     *
     *      @param [in] writer     The source writer.
     *      @param [in] component  The owner component.
     */
    void generateConnectChannelsFunc(CSourceWriter& writer, QSharedPointer<Component> component);

    /*!
     *  Generates the openConnections() function based on the component.
     *
     *      @param [in] writer     The source writer.
     *      @param [in] component  The owner component.
     */
    void generateOpenConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component);

    /*!
     *  Generates the closeConnections() function based on the component.
     *
     *      @param [in] writer     The source writer.
     *      @param [in] component  The owner component.
     */
    void generateCloseConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component);

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
     void writeCon(QSharedPointer<ComInterface> comIf, CSourceWriter &writer, QString name);

    /*!
     *  Writes open call for the given interface.
     *
     *      @param [in] comIf   Interface, which open call is written.
     *      @param [in] writer   The source writer.
     *      @param [in] name   Name of transfer type, ideally 'pkt' or 'scl'.
     */
     void writeOpen(QSharedPointer<ComInterface> comIf, CSourceWriter &writer, QString name);

    /*!
     *  Writes close call for the given interface.
     *
     *      @param [in] comIf   Interface, which close call is written.
     *      @param [in] writer   The source writer.
     *      @param [in] name   Name of transfer type, ideally 'pkt' or 'scl'.
     */
     void writeClose(QSharedPointer<ComInterface> comIf, CSourceWriter &writer, QString name);

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

    /*!
     *  Generates source files associated with the top level component of the design.
     *
     *      @param [in] design   The design subject to generation.
     *      @param [in] libComp   The top level component associated with the design.
     */
    void generateTopLevel(QSharedPointer<Design> design, QSharedPointer<Component> topComponent, QSharedPointer<DesignConfiguration const> desgConf);

    /*!
     *  Generates the MCAPI code source file.
     *
     *      @param [in] filename   The name of the source file to write.
     *      @param [in] component  The owner component.
     *      @param [in] design  The design of the component instance.
     */
    void generateInstanceHeader(QString& directory, SWInstance& ourInstance, QSharedPointer<Component> component,
        QSharedPointer<const Design> design);

    /*!
     *  Add generated MCAPI code files to the Fileset of the associated component.
     *
     *      @param [in] QSharedPointer<Component> component   The component, which Fileset will be expanded.
     */
    void addGeneratedMCAPIToFileset(QString directory, QSharedPointer<Component> topComponent, SWInstance& instance, QSharedPointer<DesignConfiguration const> desgConf);

    /*!
     *  Writes node ID and domain ID for the given software instance.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] ourInstance   The software instance which node and domain ID are written.
     */
     void writeLocalNodeAndDomain(CSourceWriter& writer, SWInstance& ourInstance);

    /*!
     *  Find connections of given software instance and returns a list of pairs, where the first is from the
     *  end of the given instance, and second the other end.
     *
     *      @param [in] design   The design where the software instance belongs to.
     *      @param [in] ourInstance   The software instance, which connections are listed.
     *      @param [in] component   The software component of ourInstance.
     *      @return List of "our" interfaces paired with their connected interfaces.
     */
     QList<QPair<QSharedPointer<ComInterface>, ComInterfaceRef> > findConnectedComInterfaces(
        QSharedPointer<const Design> design, SWInstance &ourInstance, QSharedPointer<Component> component );

    /*!
     *  Writes all endpoint identifiers and definitions of given software instance.
     *
     *      @param [in] design   The design where the software instance belongs to.
     *      @param [in] ourInstance   The software instance, which endpoint definitions are written.
     *      @param [in] component   The software component of ourInstance.
     *      @param [in] writer   The source writer.
     *      @param [in] endpointDefs   List of the names of the written endpoint definitions.
     */
    void writeEndpointDefinitions(QSharedPointer<const Design> design, SWInstance &ourInstance,
        QSharedPointer<Component> component, CSourceWriter& writer, QStringList& endpointDefs);

    /*!
     *  Writes remote endpoint of 'ourInterface' and endpoint defintions of both given interfaces.
     *  The definitions are appended to the list 'endpointDefs'.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] ourInstance   Instance of connection in "our" end.
     *      @param [in] targetInstance   Instance of connection in "their" end.
     *      @param [in] ourInterface   Interface of connection in "our" end.
     *      @param [in] targetInterface  Interface of connection in "their" end.
     *      @param [in] endpointDefs   The list of endpoint definition names.
     */
     void writeRemoteEndpoint(CSourceWriter &writer, SWInstance& ourInstance, SWInstance& targetInstance,
        QSharedPointer<ComInterface> ourInterface, QSharedPointer<ComInterface> targetInterface,
        QStringList& endpointDefs);

    /*!
     *  Writes the given endpoint definition names to array.
     *
     *      @param [in] writer   The source writer.
     *      @param [in] endpointDefs   The list of endpoint definition names.
     */
     void writeEndpointDefList(CSourceWriter &writer, QStringList &endpointDefs);

    /*!
     *  Search for a software instance by name within a design.
     *
     *      @param [in] design   The design where the software instance belongs to.
     *      @param [in] instanceName   The name of the searched software instance.
     *      @return The found software instance.
     */
     SWInstance searchInstance(QSharedPointer<const Design> design, QString instanceName);

    /*!
     *  Converts direction string of Kactus2 to enum used by PMQ-MCAPI.
     *
     *      @param [in] direction   Direction string to be converted.
     *      @return The created enum.
     */
     QString transferDirectionToEString(General::Direction direction);

    /*!
     *  Converts transfer type string of Kactus2 to enum used by PMQ-MCAPI.
     *
     *      @param [in] transferType   Transfer type string to be converted.
     *      @return The created enum.
     */
     QString transferTypeToEString(QString transferType);

    /*!
     *  Makes an endpoint identifier understood by PMQ-MCAPI.
     *
     *      @param [in] domainID   The domain of the endpoint.
     *      @param [in] nodeID   The node of the endpoint.
     *      @param [in] portID   The port of the endpoint.
     *      @return The created string.
     */
     QString makeIdString(QString domainID, QString nodeID, QString portID);

    /*!
     *  Warns if all fields of endpoint identifier may be not found in given interface and its instance.
     *
     *      @param [in] targetInterface   Interface, which port ID is to be checked.
     *      @param [in] targetInstance   Instance, which node ID and domain ID are to be checked.
     */
     void checkEndpointIdentifier(QSharedPointer<ComInterface> targetInterface, SWInstance &targetInstance);

    /*!
     *  Warns if the transfer types of given interfaces are not compatible.
     *
     *      @param [in] ourInterface   Interface of connection in "our" end.
     *      @param [in] targetInterface   Interface of connection in "their" end.
     *      @param [in] ourInstance  Instance of connection in "our" end.
     *      @param [in] targetInstance   Instance of connection in "their" end.
     */
     void checkTransferType(QSharedPointer<ComInterface> ourInterface, QSharedPointer<ComInterface>
         targetInterface, SWInstance &ourInstance, SWInstance &targetInstance);

    /*!
     *   Warns if the scalar sizes of given interfaces are not compatible.
     *
     *      @param [in] ourInterface   Interface of connection in "our" end.
     *      @param [in] targetInterface   Interface of connection in "their" end.
     *      @param [in] ourInstance   Instance of connection in "our" end.
     *      @param [in] targetInstance   Instance of connection in "their" end.
     */
     void checkScalarSize(QSharedPointer<ComInterface> ourInterface, QSharedPointer<ComInterface> targetInterface,
        SWInstance &ourInstance, SWInstance &targetInstance);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin utility.
    IPluginUtility* utility_;
};

#endif // MCAPICODEGENERATOR_H
