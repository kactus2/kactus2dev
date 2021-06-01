//-----------------------------------------------------------------------------
// File: StdOutChannel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 05.02.2021
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef CHANNELRELAY_H
#define CHANNELRELAY_H

#include <PythonAPI/WriteChannel.h>

#include <QObject>
#include <QString>

class ChannelRelay: public QObject, public WriteChannel
{
    Q_OBJECT
public:

    //! The constructor.
    explicit ChannelRelay(QObject* parent = nullptr);
    
    //! The destructor.
    virtual ~ChannelRelay() = default;

    virtual void write(QString const& text) override final;

signals:

    void data(QString const& text);
};


#endif // CHANNELRELAY_H
