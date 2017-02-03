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

#include "HDLParserCommon.h"
#include "MetaInstance.h"

class Design;
class DesignConfiguration;
class LibraryInterface;
class ListParameterFinder;

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

    QSharedPointer<QMap<QString,QSharedPointer<MetaInstance> > > getInstances() const { return instances_; }
    QSharedPointer<MetaInstance> getTopInstance() const { return topInstance_; }
    QSharedPointer<QList<QSharedPointer<MetaInterconnection> > > getInterconnections() const { return interconnections_; }
    QSharedPointer<QList<QSharedPointer<MetaWire> > > getAdHocWires() const { return adHocWires_; }

private:

    // Disable copying.
    MetaDesign(MetaDesign const& rhs);
    MetaDesign& operator=(MetaDesign const& rhs);

    /*!
     *  Parses the design_: The instances, the interconnections, the ad-hocs.
     */
    void parseDesign();
    
    /*!
     *  Parses instance in the design_.
     */
    void parseInstances();
    
    /*!
     *  Parses insterconnections in the design_.
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
    
    /*!
     *  Compares new bounds to existing bounds of the wire and then assigns the largest combination of two
     *  If even one of the existing bounds is missing, candidates will replace them both.
     *
     *      @param [in] wire            The wire which bounds are possibly enlarged.
     *      @param [in] boundCand		The candidate for new bounds.
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

    //! The design configuration to parse.
    QSharedPointer<DesignConfiguration const> designConf_;
     
    //! The list of all parsed designs that are below the current top are in this list.
    QList<QSharedPointer<MetaDesign> > subDesigns_;

    //! The finder for top level parameters.
    QSharedPointer<ListParameterFinder> topFinder_;

    //! The parsed meta instance of the top component.
    QSharedPointer<MetaInstance> topInstance_;
    //! The parsed instances in the design_, keyed with their names.
    QSharedPointer<QMap<QString,QSharedPointer<MetaInstance> > > instances_;
    //! The parsed interconnections of the design_.
    QSharedPointer<QList<QSharedPointer<MetaInterconnection> > > interconnections_;
    //! The parsed ad-hoc connections of the design_.
    QSharedPointer<QList<QSharedPointer<MetaWire> > > adHocWires_;
};

#endif // METADESIGN_H
