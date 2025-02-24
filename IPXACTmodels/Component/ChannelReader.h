//-----------------------------------------------------------------------------
// File: ChannelReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Reader class for IP-XACT Channel element.
//-----------------------------------------------------------------------------

#ifndef ChannelReader_H
#define ChannelReader_H

#include "Channel.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QDomNode>

#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT Channel element.
//-----------------------------------------------------------------------------
namespace  ChannelReader 
{
    /*!
     *  Creates a new channel from a given channel node.
     *
     *    @param [in] channelNode    XML description of the channel.
     *    @param [in] docRevision    The applied IP-XACT standard revision.
     */
    IPXACTMODELS_EXPORT QSharedPointer<Channel> createChannelFrom(QDomNode const& channelNode,
        Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Parses the local interface references for the channel. 
         *
         *    @param [in] channelNode     XML description of the channel.
         *    @param [in] newChannel      The channel to add the references to.
         *    @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void parseInterfaceReferences(QDomNode const& channelNode, QSharedPointer<Channel> newChannel,
            Document::Revision docRevision);

    }
};

#endif // ChannelReader_H
