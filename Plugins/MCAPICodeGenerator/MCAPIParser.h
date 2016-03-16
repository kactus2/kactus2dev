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

#include <IPXACTmodels/kactusExtensions/ComDefinition.h>
#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <Plugins/PluginSystem/IPluginUtility.h>

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
        DirectionTypes::Direction direction;
    };

    struct NodeData
    {
        QSharedPointer<SWInstance> instance;
        QString name;
        QString nodeID;
        QString domainID;
        QString directory;
        QList<QPair<EndPointData,EndPointData> > connections;
    };

    MCAPIParser( IPluginUtility* utility );
    ~MCAPIParser();

    /*!
     *  Returns list MCAPI endpoints within a single software component.
     */
     QList<EndPointData> getComponentEndpoints();

    /*!
     *  Returns list nodes within a single system design, along with their connections to other nodes.
     */
     QList<NodeData> getDesignNodes();

    /*!
     *  Returns list of files replaced on the instance header generation.
     */
     QStringList getReplacedFiles();

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
     void findEndpointDefinitions(QSharedPointer<const Design> design, QSharedPointer<SWInstance> ourInstance,
         QSharedPointer<Component> component, NodeData& nodeData);

    /*!
     *  Find connections of given software instance and returns a list of pairs, where the first is from the
     *  end of the given instance, and second the other end.
     *
     *      @param [in] design   The design where the software instance belongs to.
     *      @param [in] ourInstance   The software instance, which connections are listed.
     *      @param [in] component   The software component of ourInstance.
     *
     *      @return List of "our" interfaces paired with their connected interfaces.
     */
     QList<QPair<QSharedPointer<ComInterface>, PortReference> > findConnectedComInterfaces(
        QSharedPointer<const Design> design, QSharedPointer<SWInstance> ourInstance, 
        QSharedPointer<Component> component );

     /*!
      *  Parses data associated with an endpoint from given ComInterface and assigns it to the given endpoint.
      *
      *      @param [in] comIf   Source of the parsed data.
      *
      *      @return The parsed data.
      */
      EndPointData parseEndpoint(QSharedPointer<ComInterface> comIf);

    /*!
     *  Search for a software instance by name within a design.
     *
     *      @param [in] design   The design where the software instance belongs to.
     *      @param [in] instanceName   The name of the searched software instance.
     *      @return The found software instance.
     */
     QSharedPointer<SWInstance> searchInstance(QSharedPointer<const Design> design, QString const& instanceName);

    /*!
     *  Warns if all fields of endpoint identifier may be not found in given interface and its instance.
     *
     *      @param [in] targetInterface   Interface, which port ID is to be checked.
     *      @param [in] targetInstance   Instance, which node ID and domain ID are to be checked.
     */
     void checkEndpointIdentifier(QSharedPointer<ComInterface> targetInterface,
         QSharedPointer<SWInstance> targetInstance);

    /*!
     *  Warns if the transfer types of given interfaces are not compatible.
     *
     *      @param [in] ourInterface   Interface of connection in "our" end.
     *      @param [in] targetInterface   Interface of connection in "their" end.
     *      @param [in] ourInstance  Instance of connection in "our" end.
     *      @param [in] targetInstance   Instance of connection in "their" end.
     */
     void checkTransferType(QSharedPointer<SWInstance> ourInstance, QSharedPointer<ComInterface> ourInterface, 
         QSharedPointer<SWInstance> targetInstance, QSharedPointer<ComInterface> targetInterface);

    /*!
     *   Warns if the scalar sizes of given interfaces are not compatible.
     *
     *      @param [in] ourInterface   Interface of connection in "our" end.
     *      @param [in] targetInterface   Interface of connection in "their" end.
     *      @param [in] ourInstance   Instance of connection in "our" end.
     *      @param [in] targetInstance   Instance of connection in "their" end.
     */
     void checkScalarSize(QSharedPointer<SWInstance> ourInstance, QSharedPointer<ComInterface> ourInterface, 
         QSharedPointer<SWInstance> targetInstance, QSharedPointer<ComInterface> targetInterface);

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
