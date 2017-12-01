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
class PartSelect;

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
        MessagePasser* messages,
        QSharedPointer<Design const> design,
        QSharedPointer<DesignInstantiation const> designInstantiation,
        QSharedPointer<DesignConfiguration const> designConf,
        QSharedPointer<MetaInstance> topInstance);

    //! The destructor.
    ~MetaDesign();
    
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

    // Disable copying.
    MetaDesign(MetaDesign const& rhs);
    MetaDesign& operator=(MetaDesign const& rhs);


    void cullInstances();

    /*!
     *  Parses the design_: The instances, the interconnections, the ad-hocs.
     */
    void parseDesign();

    void parseDesignParamaters();



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
     *  Parses ad-hocs in the design_.
     */
    void parseAdHocs();

    void parseAdHocAssignmentForPort(QSharedPointer<MetaPort> mPort, QSharedPointer<AdHocConnection> connection, QSharedPointer<MetaWire> mWire,  bool isHierarchical, QString wireName, QSharedPointer<PartSelect> matchingPartSelect);

    void findHierarchicalPortsInAdHoc(QSharedPointer<AdHocConnection> connection,
        QList<QSharedPointer<MetaPort> > &foundPorts,
        QList<QSharedPointer<MetaPort> > &foundHierPorts,
        QList<QSharedPointer<PartSelect> > &matchingPartSelects);

    void findPortsInAdHoc(QSharedPointer<AdHocConnection> connection,
        QList<QSharedPointer<MetaPort> > &foundPorts,
        QList<QSharedPointer<PartSelect> > &matchingPartSelects);


    /*!
     *  Removes assignments without wire or default value from down ports of the topInstance_ and
     *  up ports of others. Also culls cases where the wire has only one attached port assignment.
     *  
     *      @param [in] allowDefault		    If true, assignments containing default values are not culled.
     *  
     */
    void removeUnconnectedInterfaceAssignments();
    void removeUnconnectedAdHocAssignments();
    
    /*!
     *  Tries to find hierarchy from mInstance for the active view and then create a sub design.
     *  If sub design is created, it shall be appended to the list of the sub designs.
     *
     *      @param [in] mInstance		        The instance, which component is expected to have needed instantiations.
     *                                          Consequently, will be the top instance of the created meta design.
     */
    void findHierarchy(QSharedPointer<MetaInstance> mInstance);

    QSharedPointer<Design> findDesignFromInstantiation(QSharedPointer<DesignInstantiation> dis);

    QSharedPointer<DesignConfiguration> findDesignConfigurationFromInsantiation(
        QSharedPointer<DesignConfigurationInstantiation> disg,
        QSharedPointer<Design> &subDesign);


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
    MessagePasser* messages_;

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
