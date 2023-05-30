//-----------------------------------------------------------------------------
// File: WireAbstractionReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.08.2015
//
// Description:
// Reader for ipxact:wire within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef WIREABSTRACTIONREADER_H
#define WIREABSTRACTIONREADER_H

#include <QDomNode>
#include <QObject>
#include <QSharedPointer>

#include <IPXACTmodels/common/Document.h>

class WireAbstraction;
class WirePort;
class TimingConstraint;
class CellSpecification;

//-----------------------------------------------------------------------------
//! Reader for ipxact:wire within abstraction definition.
//-----------------------------------------------------------------------------
namespace WireAbstractionReader
{
    /*!
     *  Creates a wire for abstraction definition from XML description.
     *
     *      @param [in] wireNode   The XML document to create the wire definition from.
     *      @param [in] revision   The standard revision of the XML document being read.
     *
     *      @return The created wire definition.
     */
    QSharedPointer<WireAbstraction> createWireAbstractionFrom(QDomNode const& wireNode, Document::Revision revision);

    namespace Details
    {
        /*!
         *  Reads the qualifier from XML to a wire.
         *
         *      @param [in]     wireNode    The XML description of the wire.
         *      @param [in/out] wire        The wire definition to insert the qualifier into.
         */
        void parseQualifier(QDomNode const& wireNode, QSharedPointer<WireAbstraction> wire);
                                   
        /*!
         *  Reads the system ports from XML to a wire.
         *
         *      @param [in]     wireNode    The XML description of the wire.
         *      @param [in/out] wire        The wire definition to insert the system ports into.
         */
        void parseSystems(QDomNode const& wireNode, QSharedPointer<WireAbstraction> wire);
                                   
        /*!
         *  Creates a wirePort for a wire from XML description.
         *
         *      @param [in]     wirePortNode    The XML description of the wirePort.
         *
         *      @return The created wirePort.
         */
        QSharedPointer<WirePort> parseWirePort(QDomNode const& wirePortNode);

        /*!
         *  Reads the constraints for a wirePort from XML to a wire.
         *
         *      @param [in]     wirePortNode    The XML description of the wirePort.
         *      @param [in/out] wirePort        The wirePort to insert the constraints into.
         */
        void parseConstraints(QDomNode const& wirePortNode, QSharedPointer<WirePort> wirePort);

        /*!
         *  Creates a timing constraint for a wirePort from a XML description.
         *
         *      @param [in]     timingNode    The XML description of the timing constraint.
         *
         *      @return The created timing constraint.
         */
        QSharedPointer<TimingConstraint> parseTimingConstraint(QDomElement const& timingNode);

        /*!
         *  Creates a cell specification for load or drive constraint in a wirePort from a XML description.
         *
         *      @param [in]     cellNode    The XML description of the cell specification.
         *
         *      @return The created cell specification.
         */
        QSharedPointer<CellSpecification> parseCellSpecification(QDomElement const& cellNode);
       
        /*!
         *  Reads the constraints for mirrored mode for a wirePort from XML to a wire.
         *
         *      @param [in]     wirePortNode    The XML description of the wirePort.
         *      @param [in/out] wirePort        The wirePort to insert the mirrored constraints into.
         */
        void parseMirroredConstraints(QDomNode const& wirePortNode, QSharedPointer<WirePort> wirePort);

        /*!
         *  Reads the initiator port from XML to a wire.
         *
         *      @param [in]     wireNode    The XML description of the wire.
         *      @param [in/out] wire        The wire definition to insert the initiator port into.
         *      @param [in]     revision    The standard revision of the XML document being read.
         */
        void parseInitiator(QDomNode const& wireNode, QSharedPointer<WireAbstraction> wire, Document::Revision revision);
                                 
        /*!
         *  Reads the target port from XML to a wire.
         *
         *      @param [in]     wireNode    The XML description of the wire.
         *      @param [in/out] wire        The wire definition to insert the target port into.
         *      @param [in]     revision    The standard revision of the XML document being read.
         */ 
        void parseTarget(QDomNode const& wireNode, QSharedPointer<WireAbstraction> wire, Document::Revision revision);
    }
}

#endif // WIREABSTRACTIONREADER_H
