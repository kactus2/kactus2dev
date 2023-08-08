//-----------------------------------------------------------------------------
// File: ModeReader.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 03.08.2023
//
// Description:
// Reader class for IP-XACT mode element.
//-----------------------------------------------------------------------------

#ifndef ModeREADER_H
#define ModeREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

class Mode;
class RemapPort;

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT mode element.
//-----------------------------------------------------------------------------
namespace ModeReader
{

    /*!
     *  Creates a new mode from a given view node.
     *
     *      @param [in] modeNode  XML description of the mode.
     *
     *      @return The created mode.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Mode> createModeFrom(QDomNode const& modeNode);

    namespace Details
    {

        /*!
         *  Read the remap ports.
         *
         *      @param [in] ModeNode  XML description of the mode.
         *      @param [in] newMode   The selected mode item.
         */
        void parseRemapPorts(QDomNode const& ModeNode, QSharedPointer<Mode> newMode);
    }


};

#endif // VIEWREADER_H