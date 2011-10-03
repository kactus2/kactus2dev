/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/


#ifndef SIGNAL_MULTIPLEXER_H
#define SIGNAL_MULTIPLEXER_H

#include "Common.h"
#include <QObject>

namespace GCF
{
    struct SignalMultiplexerData;
    class GCF_EXPORT_CLASS SignalMultiplexer : public QObject
    {
    Q_OBJECT
    public:
        SignalMultiplexer(QObject* obj, QObject* parent=0);
        ~SignalMultiplexer();

        QObject* object() const;

        void makeConnection(const char* signal, QObject* receiver, const char* member);
        void breakConnection(QObject* receiver);

        void setActiveReceiver(QObject* receiver, bool active=true);
        bool isActiveReceiver(QObject* receiver);

        QList<QObject*> receivers() const;
        bool hasReceivers() const;

    private slots:
        void receiver_destroyed(QObject* obj);
        void sender_destroyed(QObject* obj);

    private:
        SignalMultiplexerData* d;
    };
}

#endif

