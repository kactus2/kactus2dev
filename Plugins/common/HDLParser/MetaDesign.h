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
    //! The constructor.
    MetaDesign(LibraryInterface* library, QSharedPointer<Design const> design,
        QSharedPointer<DesignConfiguration const> designConf, QSharedPointer<MetaInstance> topInstance);

    //! The destructor.
    ~MetaDesign();

    //! The alternative constructor.
    MetaDesign();

    static QList<QSharedPointer<MetaDesign> > parseHierarchy(LibraryInterface* library,  GenerationTuple input,
        QSharedPointer<View> topComponentView);

    QMap<QString,QSharedPointer<MetaInstance> > instances_;

    QSharedPointer<MetaInstance> topInstance_;

    QList<QSharedPointer<MetaInterconnection> > interconnections_;

    QList<QSharedPointer<MetaWire> > adHocWires_;

private:

    // Disable copying.
    MetaDesign(MetaDesign const& rhs);
    MetaDesign& operator=(MetaDesign const& rhs);

    void parseDesign();

    void parseInstances();

    void parseInterconnections();

    void wireInterfacePorts(QSharedPointer<MetaInterface> mInterface,
        QSharedPointer<MetaInterconnection> mIterconnect, bool isHierarchical);

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

    //! The design to parse.
    QSharedPointer<Design const> design_;

    //! The design configuration to parse.
    QSharedPointer<DesignConfiguration const> designConf_;
     
    //! The list of all parsed designs that are below the current top are in this list.
    QList<QSharedPointer<MetaDesign> > subDesigns_;

    //! The finder for top level parameters.
    QSharedPointer<ListParameterFinder> topFinder_;
};

#endif // METADESIGN_H
