//-----------------------------------------------------------------------------
// File: RemapStateReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Reader class for IP-XACT remap state element.
//-----------------------------------------------------------------------------

#ifndef REMAPSTATEREADER_H
#define REMAPSTATEREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QSharedPointer>
#include <QDomNode>

class RemapState;
class RemapPort;

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT remap state element.
//-----------------------------------------------------------------------------
namespace RemapStateReader
{
    /*!
     *  Creates a new remap state from a given view node.
     *
     *    @param [in] remapStateNode  XML description of the remap state.
     *
     *    @return The created remap state.
     */
    IPXACTMODELS_EXPORT QSharedPointer<RemapState> createRemapStateFrom(QDomNode const& remapStateNode);

    namespace Details
    {
        /*!
         *  Read the remap state name group.
         *
         *    @param [in] remapStateNode  XML description of the remap state.
         *    @param [in] newRemapState   The selected remap state item.
         */
        void parseNameGroup(QDomNode const& remapStateNode, QSharedPointer<RemapState> newRemapState);

        /*!
         *  Read the remap ports.
         *
         *    @param [in] remapStateNode  XML description of the remap state.
         *    @param [in] newRemapState   The selected remap state item.
         */
        void parseRemapPorts(QDomNode const& remapStateNode, QSharedPointer<RemapState> newRemapState);

        /*!
         *  Read the remap port index.
         *
         *    @param [in] remapPortElement    XML description of the remap port.
         *    @param [in] newRemapPort        The selected remap port item.
         */
        void parseRemapPortIndex(QDomElement const& remapPortElement, QSharedPointer<RemapPort> newRemapPort);

        /*!
         *  Read the remap port value.
         *
         *    @param [in] remapPortElement    XML description of the remap port.
         *    @param [in] newRemapPort        The selected remap port item.
         */
        void parseRemapPortValue(QDomElement const& remapPortElement, QSharedPointer<RemapPort> newRemapPort);
    };
};

#endif // VIEWREADER_H