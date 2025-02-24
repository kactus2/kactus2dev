//-----------------------------------------------------------------------------
// File: RemapStateWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Writer class for IP-XACT remapState element.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATEWRITER_H
#define REMAPSTATEWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

class RemapState;
class RemapPort;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT remapState element.
//-----------------------------------------------------------------------------
namespace  RemapStateWriter 
{

    /*!
     *  Write a remap state to an XML file.
     *
     *    @param [in] writer  The used xml writer.
     *    @param [in] view    The remap state to be written.
     */
    IPXACTMODELS_EXPORT void writeRemapState(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState);

    namespace Details
    {
        /*!
         *  Write the name group.
         *
         *    @param [in] writer      Used XML writer.
         *    @param [in] remapState  The selected remap state.
         */
        void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState);

        /*!
         *  Write the remap ports.
         *
         *    @param [in] writer      Used XML writer.
         *    @param [in] remapState  The selected remap state.
         */
        void writeRemapPorts(QXmlStreamWriter& writer, QSharedPointer<RemapState> remapState);

        /*!
         *  Write a single remap port.
         *
         *    @param [in] writer      Used XML writer.
         *    @param [in] remapPort   The selected remap port.
         */
        void writeSingleRemapPort(QXmlStreamWriter& writer, QSharedPointer<RemapPort> remapPort);
    };
};

#endif // REMAPSTATEWRITER_H