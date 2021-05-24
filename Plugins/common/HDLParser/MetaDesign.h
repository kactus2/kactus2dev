//-----------------------------------------------------------------------------
// File: MetaDesign.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 11.01.2017
//
// Description:
// An instantiated design within a hierarchy with all its parameters, instances, and interconnections parsed.
//-----------------------------------------------------------------------------

#ifndef METADESIGN_H
#define METADESIGN_H

#include "MetaInstance.h"

class Design;
class DesignConfiguration;
class AdHocConnection;
class MessageMediator;
class PartSelect;
class PortAbstraction;

//-----------------------------------------------------------------------------
// An instantiated design with all its parameters, instances, and interconnections parsed.
//-----------------------------------------------------------------------------
class MetaDesign
{
public:
    /*!
     *  The constructor.
     *
     *      @param [in] library             The library which is expected to contain the parsed documents.
     *      @param [in] messages		    For message output.
     *      @param [in] design              The design to parse.
     *      @param [in] designConf          The design configuration to parse.
     *      @param [in] topInstance         The parsed meta instance of the top component.
     */
    MetaDesign(LibraryInterface* library,
        MessageMediator* messages,
        QSharedPointer<Design const> design,
        QSharedPointer<DesignInstantiation const> designInstantiation,
        QSharedPointer<DesignConfiguration const> designConf,
        QSharedPointer<MetaInstance> topInstance);

    //! The destructor.
    ~MetaDesign() = default;

    // Disable copying.
    MetaDesign(MetaDesign const& rhs) = delete;
    MetaDesign& operator=(MetaDesign const& rhs) = delete;

    /*!
     *  Parses a hierarchy of designs and return the list of them.
     *
     *      @param [in] library                 The library which is expected to contain the parsed documents.
     *      @param [in] input		            The starting point for hierarchy parsing.
     *      @param [in] topComponentView        The active view of the component in input.
     */
    static QList<QSharedPointer<MetaDesign> > parseHierarchy(LibraryInterface* library,
        GenerationTuple input,
        QSharedPointer<View> topComponentView);
    
    /*!
     *  Returns the meta instance of the top component. Must exists even if is the topmost in hierarchy.
     */
    QSharedPointer<MetaInstance> getTopInstance() const {return topInstance_;}
    
    /*!
     *  Returns map of the parsed meta instances within the design. Keyed with instance names.
     */
    QSharedPointer<QMap<QString,QSharedPointer<MetaInstance> > > getInstances() const {return instances_;}
    
    /*!
     *  Returns list of the parsed meta interconnections within the design.
     */
    QSharedPointer<QList<QSharedPointer<MetaInterconnection> > > getInterconnections() const {return interconnections_;}
    
    /*!
    *  Returns list of the parsed ad-hoc wires within the design.
     */
    QSharedPointer<QList<QSharedPointer<MetaWire> > > getAdHocWires() const {return adHocWires_;}

private:

    /*!
     *  Finds the component instances within the design, also determines if they are hierarchical.
     */
    void findInstances();

    /*!
     *  Parses the design_: The instances, the interconnections, the ad-hocs.
     */
    void parseDesign();

    /*!
     *  Parses the design parameters. Also applies them to the meta parameters.
     */
    void parseDesignParamaters();
    
    /*!
     *  Parses parameters within a list.
     *
     *      @param [inout] subList              The list which parameters are parsed.
     *      @param [in] input		            The list of parameters which may affect the parsing result. May be null.
     *      @param [in] topComponentView        The list of overrides for the parsed parameters. May be null.
     */
    static void parseParameters(
        QSharedPointer<QList<QSharedPointer<Parameter> > > subList,
        QSharedPointer<QList<QSharedPointer<Parameter> > > topList,
        QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > cevs);

    /*!
     *  Parses instances in the design_.
     */
    void parseInstances();

    /*!
     *  Parses interconnections in the design_.
     */
    void parseInterconnections();
    
    /*!
     *  Assigns ports of mInterface with the wires of mInterconnect.
     *
     *      @param [in] mInterface              The interface which will have the ports.
     *      @param [in] mIterconnect		    The interconnect which will have the wires.
     *      @param [in] isHierarchical          True, if mInterface is in topInstance, else false.
     */
    void wireInterfacePorts(QSharedPointer<MetaInterface> mInterface,
        QSharedPointer<MetaInterconnection> mIterconnect, bool isHierarchical);
    
    /*!
     *  Associate the selected meta port with wire.
     *
     *      @param [in] metaInterconnect    Meta interconnection containing the wire.
     *      @param [in] portAbstraction     Connected logical port.
     *      @param [in] assignment          Meta assignment for the port.
     *      @param [in] connectedWires      List of already connected meta wire.
     *      @param [in] isHierarchical      Flag for hierarchical ports.
     *      @param [in] metaPort            The selected meta port.
     */
    void associateWithWire(QSharedPointer<MetaInterconnection> metaInterconnect,
        QSharedPointer<PortAbstraction> portAbstraction, QSharedPointer<MetaPortAssignment> assignment,
        QList<QSharedPointer<MetaWire> >& connectedWires, bool isHierarchical, QSharedPointer<MetaPort> metaPort);

    /*!
     *  Associate the selected meta port with transactional.
     *
     *      @param [in] metaInterconnect            Meta interconnection containing the wire.
     *      @param [in] portAbstraction             Connected logical port.
     *      @param [in] assignment                  Meta assignment for the port.
     *      @param [in] connectedTransactionals     List of already connected meta transactionals.
     *      @param [in] isHierarchical              Flag for hierarchical ports.
     *      @param [in] metaPort                    The selected meta port.
     */
    void associateWithTransactional(QSharedPointer<MetaInterconnection> metaInterconnect,
        QSharedPointer<PortAbstraction> portAbstraction, QSharedPointer<MetaPortAssignment> assignment,
        QList<QSharedPointer<MetaTransactional> >& connectedTransactionals, bool isHierarchical,
        QSharedPointer<MetaPort> metaPort);

    /*!
     *  Parses ad-hocs in the design_.
     */
    void parseAdHocs();
    
    /*!
     *  Parses a port assignment for an ad-hoc port.
     *
     *      @param [in] mPort                   The port which get an assignment.
     *      @param [in] connection		        The interconnect which is associated with IP-XACT port corresponding mPrt.
     *      @param [in] mWire                   Wire which will be referred by the assignment.
     *      @param [in] isHierarchical          True, if this connection is hierarchical.
     *      @param [in] wireName                Name of mWire.
     *      @param [in] matchingPartSelect      The part select that affects the bounds of the assignment.
     */
    void parseAdHocAssignmentForPort(QSharedPointer<MetaPort> mPort,
        QSharedPointer<AdHocConnection> connection,
        QSharedPointer<MetaWire> mWire,
        bool isHierarchical,
        QString wireName,
        QSharedPointer<PartSelect> matchingPartSelect);
    
    /*!
     *  Finds hierarchical ports within an ad-hoc interconnection.
     *
     *      @param [in] connection		        The interconnect which ports are beign searched.
     *      @param [out] foundPorts             Every port found for the interconnection.
     *      @param [out] foundHierPorts         Every hierarchical port found for the interconnection.
     *      @param [out] matchingPartSelects    The part select that affects the bounds of the assignment.
     */
    void findHierarchicalPortsInAdHoc(QSharedPointer<AdHocConnection> connection,
        QList<QSharedPointer<MetaPort> > &foundPorts,
        QList<QSharedPointer<MetaPort> > &foundHierPorts,
        QList<QSharedPointer<PartSelect> > &matchingPartSelects);
    
    /*!
     *  Finds ports within an ad-hoc interconnection.
     *
     *      @param [in] connection		        The interconnect which ports are beign searched.
     *      @param [out] foundPorts             Every port found for the interconnection.
     *      @param [out] matchingPartSelects    The part select that affects the bounds of the assignment.
     */
    void findPortsInAdHoc(QSharedPointer<AdHocConnection> connection,
        QList<QSharedPointer<MetaPort> > &foundPorts,
        QList<QSharedPointer<PartSelect> > &matchingPartSelects);


    /*!
     *  Removes assignments without wire or default value from down ports of the topInstance_ and
     *  up ports of others. Also culls cases where the wire has only one attached port assignment.
     */
    void removeUnconnectedInterfaceAssignments();

    /*!
     *  Removes assignments without wire or default value from down ports of the topInstance_ and
     *  up ports of others. Also culls cases where the wire has only one attached port assignment
     */
    void removeUnconnectedAdHocAssignments();
    
    /*!
     *  Tries to find hierarchy from mInstance for the active view and then create a sub design.
     *  If sub design is created, it shall be appended to the list of the sub designs.
     *
     *      @param [in] mInstance		        The instance, which component is expected to have needed instantiations.
     *                                          Consequently, will be the top instance of the created meta design.
     */
    void findHierarchy(QSharedPointer<MetaInstance> mInstance);

    /*!
     *  Finds a design from a design instantiation.
     *
     *      @param [in] dis                     Where from the design is searched.
     *
     *      @return The found design, or null.
     */
    QSharedPointer<Design> findDesignFromInstantiation(QSharedPointer<DesignInstantiation> designInstantiation,
        QSharedPointer<DesignConfiguration> configurationInstantiation);

    /*!
     *  Finds a design and design configuration from a design instantiation.
     *
     *      @param [in] configurationInstantiation                    Where from the design configuration is searched.
     *
     *      @return The found design configuration, or null.
     */
    QSharedPointer<DesignConfiguration> findDesignConfigurationFromInsantiation(
        QSharedPointer<DesignConfigurationInstantiation> configurationInstantiation);


    /*!
     *  Compares new bounds to existing bounds of the wire and then assigns the largest combination of two
     *  If even one of the existing bounds is missing, candidates will replace them both.
     *
     *      @param [in] wire                    The wire which bounds are possibly enlarged.
     *      @param [in] boundCand		        The candidate for new bounds.
     */
    void assignLargerBounds(QSharedPointer<MetaWire> wire, QPair<QString,QString> const& boundCand);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component library.
    LibraryInterface* library_;

    //! The messages.
    MessageMediator* messages_;

    //! The design to parse.
    QSharedPointer<Design const> design_;

    //! The design instantiation that matches the design.
    QSharedPointer<DesignInstantiation const> designInstantiation_;

    //! The design configuration to parse.
    QSharedPointer<DesignConfiguration const> designConf_;

    //! The parsed meta instance of the top component.
    QSharedPointer<MetaInstance> topInstance_;

    //! The parsed design parameters.
    QSharedPointer<QList<QSharedPointer<Parameter> > > parameters_;

    //! The parsed instances in the design_, keyed with their names.
    QSharedPointer<QMap<QString,QSharedPointer<MetaInstance> > > instances_;
    //! The parsed interconnections of the design_.
    QSharedPointer<QList<QSharedPointer<MetaInterconnection> > > interconnections_;
    //! The parsed ad-hoc connections of the design_.
    QSharedPointer<QList<QSharedPointer<MetaWire> > > adHocWires_;

    //! The list of all parsed designs that are below the current top are in this list.
    QList<QSharedPointer<MetaDesign> > subDesigns_;
};

#endif // METADESIGN_H
