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

// This class is based on Q4puGenericSignalSpy a class written by me.
// It is modified for use in GCF. You can download the original class
// from http://www.vcreatelogic.com/oss/signalspy/index.html

#ifndef GENERIC_SIGNAL_SPY
#define GENERIC_SIGNAL_SPY

#include "Common.h"
#include <QObject>
#include <QVariant>

namespace GCF
{
    struct GenericSignalSpyData;
    class GCF_EXPORT_CLASS GenericSignalSpy : public QObject
    {
        Q_OBJECT

    public:
        GenericSignalSpy(QObject* parent, QObject* sender, const char* signal);
        ~GenericSignalSpy();

        bool isValid() const;
        QObject* sender() const;
        QByteArray signal() const;
        int signalMethodIndex() const;

    signals:
        void caughtSignal(const QVariantList& args);

    protected:
        virtual void handleSignalEmission(const QVariantList& args);

    protected slots:
        void senderDestroyed(QObject* object);

    public:
        // public, but strictly internal.
        void methodInvoked(int method_index, void **argv);

    private:
        GenericSignalSpyData* d;
    };

}

#endif
