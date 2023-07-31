//-----------------------------------------------------------------------------
// File: WireAbstractionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.08.2015
//
// Description:
// Writer for IP-XACT Wire element within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef WIREABSTRACTIONWRITER_H
#define WIREABSTRACTIONWRITER_H

#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QXmlStreamWriter>

class CellSpecification;
class TimingConstraint;
class WireAbstraction;
class WirePort;

//-----------------------------------------------------------------------------
//! Writer for IP-XACT Wire element within abstraction definition.
//-----------------------------------------------------------------------------
namespace WireAbstractionWriter
{

    /*!
     *  Writes the given wire into XML.
     *
     *      @param [in] writer      The XML writer to use.
     *      @param [in] wire        The wire to write.
     *      @param [in] revision    The document standard revision to use.
     */
    IPXACTMODELS_EXPORT void writeWire(QXmlStreamWriter& writer, QSharedPointer<WireAbstraction> wire,
        Document::Revision revision);

    namespace Details
    {

        /*!
         *  Writes the given wire qualifier into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wire        The wire whose qualifier to write.
         */
        void writeQualifier(QXmlStreamWriter& writer, QSharedPointer<WireAbstraction> wire);

        /*!
         *  Writes the system port for the given wire into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wire        The wire whose system port to write.
         */
        void writeSystem(QXmlStreamWriter& writer, QSharedPointer<WireAbstraction> wire);

        /*!
         *  Writes the wire port on system/master/slave into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wirePort    The wirePort to write.
         */
        void writeWirePort(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort);

        /*!
         *  Writes the wire port presence on system/master/slave into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wirePort    The wirePort whose presence to write.
         */
        void writePresence(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort);

        /*!
         *  Writes the wire port width on system/master/slave into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wirePort    The wirePort whose width to write.
         */
        void writeWidth(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort);

        /*!
         *  Writes the wire port direction on system/master/slave into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wirePort    The wirePort whose direction to write.
         */
        void writeDirection(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort);

        /*!
         *  Writes the wire mode constraints into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wirePort    The wirePort whose mode constraints to write.
         */
        void writeModeConstraints(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort);

        /*!
         *  Writes a timing constraint into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] timing      The timing constraint to write.
         */
        void writeTimingConstraint(QXmlStreamWriter& writer, QSharedPointer<TimingConstraint> timing);

        /*!
         *  Writes a drive constraint into XML.
         *
         *      @param [in] writer              The XML writer to use.
         *      @param [in] driveConstraint     The drive constraint to write.
         */
        void writeDriveConstraint(QXmlStreamWriter& writer, QSharedPointer<CellSpecification> driveConstraint);

        /*!
         *  Writes a load constraint into XML.
         *
         *      @param [in] writer              The XML writer to use.
         *      @param [in] loadConstraint      The load constraint to write.
         */
        void writeLoadConstraint(QXmlStreamWriter& writer, QSharedPointer<CellSpecification> loadConstraint);

        /*!
         *  Writes a cell specification in load or drive constraint into XML.
         *
         *      @param [in] writer              The XML writer to use.
         *      @param [in] specification       The  cell specification to write.
         */
        void writeCellSpecification(QXmlStreamWriter& writer, QSharedPointer<CellSpecification> specification);

        /*!
         *  Writes the wire mode constraints in mirrored mode into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wirePort    The wirePort whose mirrored mode constraints to write.
         */
        void writeMirroredModeConstraints(QXmlStreamWriter& writer, QSharedPointer<WirePort> wirePort);

        /*!
         *  Writes the initiator port for the given wire qualifier into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wire        The wire whose initiator port to write.
         */
        void writeInitiator(QXmlStreamWriter& writer, QSharedPointer<WireAbstraction> wirePort,
            QString const& elementName);

        /*!
         *  Writes the target port for the given wire qualifier into XML.
         *
         *      @param [in] writer      The XML writer to use.
         *      @param [in] wire        The wire whose target port to write.
         */
        void writeTarget(QXmlStreamWriter& writer, QSharedPointer<WireAbstraction> wirePort,
            QString const& elementName);
    }
}

#endif // WIREABSTRACTIONWRITER_H
