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
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT Channel element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ChannelReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    ChannelReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ChannelReader();

    /*!
     *  Creates a new channel from a given channel node.
     *
     *      @param [in] channelNode    XML description of the channel.
     */
    QSharedPointer<Channel> createChannelFrom(QDomNode const& channelNode) const;

private:

    //! No copying allowed.
    ChannelReader(ChannelReader const& rhs);
    ChannelReader& operator=(ChannelReader const& rhs);

    /*!
     *  Reads the presence.
     *
     *      @param [in] channelNode    XML description of the view.
     *      @param [in] newChannel     The new channel item.
     */
    void parseIsPresent(QDomNode const& channelNode, QSharedPointer<Channel> newChannel) const;

    /*!
     *  Reads the name group.
     *
     *      @param [in] channelNode    XML description of the view.
     *      @param [in] newChannel		The new channel item.
     */
    void parseNameGroup(QDomNode const& channelNode, QSharedPointer<Channel> newChannel) const;
};

#endif // ChannelReader_H
