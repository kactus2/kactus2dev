//-----------------------------------------------------------------------------
// File: ChannelWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Writer class for IP-XACT Channel element.
//-----------------------------------------------------------------------------

#ifndef ChannelWriter_H
#define ChannelWriter_H

#include "Channel.h"

#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Channel element.
//-----------------------------------------------------------------------------
namespace ChannelWriter
{

    /*!
     *  Write a channel to an XML file.
     *
     *      @param [in] writer      The used xml writer.
     *      @param [in] channel		The channel to be written.
     *      @param [in] docRevision The applied IP-XACT standard revision.
     */
    IPXACTMODELS_EXPORT void writeChannel(QXmlStreamWriter& writer, QSharedPointer<Channel> channel,
        Document::Revision docRevision);

};

#endif // ChannelWriter_H
