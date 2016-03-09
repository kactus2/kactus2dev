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

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>
#include <IPXACTmodels/common/CommonItemsWriter.h>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT Channel element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ChannelWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this writer.
     */
    ChannelWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ChannelWriter();

    /*!
     *  Write a channel to an XML file.
     *
     *      @param [in] writer      The used xml writer.
     *      @param [in] channel		The channel to be written.
     */
    void writeChannel(QXmlStreamWriter& writer, QSharedPointer<Channel> channel) const;

private:

    //! No copying allowed.
    ChannelWriter(ChannelWriter const& rhs);
    ChannelWriter& operator=(ChannelWriter const& rhs);

    /*!
     *  Writes the name group to an XML file.
     *
     *      @param [in] writer      The used xml writer.
     *      @param [in] channel		The channel to be written.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Channel> channel) const;

    /*!
     *  Writes the presence to an XML file.
     *
     *      @param [in] writer      The used xml writer.
     *      @param [in] channel		The channel to be written.
     */
    void writeIsPresent(QXmlStreamWriter& writer, QSharedPointer<Channel> channel) const;
};

#endif // ChannelWriter_H
