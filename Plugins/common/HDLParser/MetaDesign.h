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
    MetaDesign(LibraryInterface* library, QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> designConf, QSharedPointer<MetaInstance> topInstance);

    //! The destructor.
    ~MetaDesign();

    static void parseHierarchy(LibraryInterface* library, QSharedPointer<Component> topComponent,
        QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConf,
        QSharedPointer<View> topComponentView);

private:

    // Disable copying.
    MetaDesign(MetaDesign const& rhs);
    MetaDesign& operator=(MetaDesign const& rhs);

    void parseDesign();

    void parseInstances();

    void parseInsterconnections();
    
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
	 QSharedPointer<Design> design_;

	 //! The design configuration to parse.
	 QSharedPointer<DesignConfiguration> designConf_;

     QSharedPointer<MetaInstance> topInstance_;

     QMap<QString,QSharedPointer<MetaInstance> > instances_;

     QList<QSharedPointer<MetaInterconnection> > interconnections_;
     
     //! The list of all parsed designs that are below the current top are in this list.
     QList<QSharedPointer<MetaDesign> > subDesigns_;

     //! The finder for top level parameters.
     QSharedPointer<ListParameterFinder> topFinder_;
};

#endif // METADESIGN_H
