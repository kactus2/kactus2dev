//-----------------------------------------------------------------------------
// File: MCAPIParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 13.10.2014
//
// Description:
// MCAPI parser.
//-----------------------------------------------------------------------------

#ifndef MCAPIParser_H
#define MCAPIParser_H

#include <IPXACTmodels/ComDefinition.h>
#include <IPXACTmodels/ComInterface.h>
#include <IPXACTmodels/design.h>
#include <IPXACTmodels/designconfiguration.h>
#include <Plugins/PluginSystem/IPluginUtility.h>

class Component;
class CSourceWriter;

//-----------------------------------------------------------------------------
//! MCAPI code generator.
//-----------------------------------------------------------------------------
class MCAPIParser
{
public:
    struct EndPointData
    {
        QString name;
        QString remoteName;
        QString portID;
        QString nodeID;
        QString domainID;
        QString handleName;
        QString scalarSize;
        QString transferType;
        General::Direction direction;
    };

    struct NodeData
    {
        SWInstance instance;
        QString name;
        QString nodeID;
        QString domainID;
        QString directory;
        QList<QPair<EndPointData,EndPointData>> connections;
    };

    MCAPIParser( IPluginUtility* utility );
    ~MCAPIParser();

    /*!
     *  Returns list MCAPI endpoints within a single software component.
     */
     const QList<EndPointData>& getComponentEndpoints();

    /*!
     *  Returns list nodes within a single system design, along with their connections to other nodes.
     */
     const QList<NodeData>& getDesignNodes();

    /*!
     *  Returns list of files replaced on the instance header generation.
     */
     const QStringList& getReplacedFiles();

    /*!
     *  Generates MCAPI code for the given component.
     *
     *      @param [in] component   The component, which is being parsed.
     */
    void parseMCAPIForComponent(QSharedPointer<Component> component);

    /*!
     *  Generates source files associated with the top level component of the design.
     *
     *      @param [in] design   The design subject to generation.
     *      @param [in] topComponent   The top level component associated with the design.
     *      @param [in] desgConf   The design configuration associated with the design.
     */
     void parseTopLevel(QSharedPointer<Design> design, QSharedPointer<Component> topComponent,
         QSharedPointer<DesignConfiguration const> desgConf);

private:

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
      *      @param [in] componentVLNV   VLNV associated with component where the interface belongs to.
      *      @param [in] comDef    The ComDefinition under inspection.
      *      @param [in] comIf   The ComInterface under inspection.
      *      @param [in] errorList    Missing properties will be reported in this list.
      */
      void checkRequiredPropertiesSet(QString componentVLNV, QSharedPointer<ComDefinition> comDef,
         QSharedPointer<ComInterface> comIf,  QStringList &errorList);

    /*!
     *  Parses all endpoint definitions of given software instance.
     *
     *      @param [in] design   The design where the software instance belongs to.
     *      @param [in] ourInstance   The software instance, which endpoint definitions are written.
     *      @param [in] component   The software component of ourInstance.
     *      @param [in] nodeData    Node associated with the instance.
     */
     void findEndpointDefinitions(QSharedPointer<const Design> design, SWInstance &ourInstance,
         QSharedPointer<Component> component, NodeData& nodeData);

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
      *  Parses data associated with an endpoint from given ComInterface and assigns it to the given endpoint.
      *
      *      @param [in] epd   Struct that will contain the parsed data.
      *      @param [in] comIf   Source of the parsed data.
      */
      void parseEndpoint(EndPointData &epd, QSharedPointer<ComInterface> comIf);

    /*!
     *  Search for a software instance by name within a design.
     *
     *      @param [in] design   The design where the software instance belongs to.
     *      @param [in] instanceName   The name of the searched software instance.
     *      @return The found software instance.
     */
     SWInstance searchInstance(QSharedPointer<const Design> design, QString instanceName);

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

    //! Endpoints parsed from component.
    QList<EndPointData> componentEndpoints_;
    //! Nodes parsed from design.
    QList<NodeData> designNodes_;
    //! List of files that may be replaced
    QStringList replacedFiles_;
    //! The plugin utility.
    IPluginUtility* utility_;
};

#endif // MCAPIParser_H
