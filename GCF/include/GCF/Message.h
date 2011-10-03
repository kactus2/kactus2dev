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

#ifndef MESSAGE_H
#define MESSAGE_H

#include "Common.h"
#include "DataStore.h"
#include <QString>
#include <QVariant>
#include <QByteArray>

class QDataStream;
class QTextStream;

/////////////////////////////////////////////////////////////////////////////

namespace GCF
{
    struct MessageData;
    class GCF_EXPORT_CLASS Message
    {
    public:
        // 0x00f0 messages are client to server messages
        // 0x0f00 messages are server to client messages
        // Use messageDirection() static method to figure out
        // message direction
        enum MessageType
        {
            InvalidMessage       = (0x0ff0+0),
            RequestAccess        = (0x00f0+1),
            RequestAccessRes     = (0x0f00+2),
            RequestObject        = (0x00f0+3),
            RequestObjectRes     = (0x0f00+4),
            GetPropertyNames     = (0x00f0+5),
            GetPropertyNamesRes  = (0x0f00+6),
            SetProperty          = (0x00f0+7),
            SetPropertyRes       = (0x0f00+8),
            GetProperty          = (0x00f0+9),
            GetPropertyRes       = (0x0f00+10),
            InvokeMethod         = (0x00f0+11),
            InvokeMethodRes      = (0x0f00+12),
            RequestConnection    = (0x00f0+13),
            RequestConnectionRes = (0x0f00+14),
            SignalDelivery       = (0x0ff0+15),
            ReleaseObject        = (0x0ff0+16),
            ReleaseObjectRes     = (0x0f00+17),
            ReleaseAccess        = (0x0ff0+18),
            ReleaseAccessRes     = (0x0f00+19),
            ReleaseConnection    = (0x0ff0+20),
            ReleaseConnectionRes = (0x0f00+21),
            UserSpecific         = (0x0ff0+22), // anything more than this
        };

        enum MessageDirection
        {
            ClientToServer       = 1,
            ServerToClient       = 2,
            Both                 = 3
        };

        static MessageDirection messageDirection(MessageType);
        static QString messageTypeString(MessageType);

        Message(qint32 id=-1);
        ~Message();

        bool isValid() const;
        bool isIncoming() const;
        qint32 id() const;
        MessageType messageType() const;
        MessageDirection messageDirection() const;
        virtual void setObjectId(qint32 id); // helper method to make things easy
        virtual qint32 objectId() const; // helper method to make things easy
        QString toString() const;

        DataStore customData() const;
        QVariant & operator [] (const QString& key);
        bool hasKey(const QString& key);
        void removeKey(const QString& key);
        QStringList keys() const;

        QByteArray toByteArray() const;
        static Message* fromByteArray(QByteArray& ba);

    private:
        void setIncoming(bool val);
        void setId(qint32 id);

    protected:
        void setMessageType(MessageType type);

    protected:
        // Methods that are called from toByteArray()
        void writeHeader(QDataStream& ds) const;
        virtual void writeBody(QDataStream& ds) const;
        void writeFooter(QDataStream& ds) const;

        // Methods that are called from fromByteArray()
        void initFromByteArray(const QByteArray& ba);

        // Methods called from initFromByteArray()
        void readHeader(QDataStream& ds);
        virtual void readBody(QDataStream& ds);
        void readFooter(QDataStream& ds);

        // Write message info into the string
        virtual void dumpMessageInfo(QTextStream& ts) const;

    public:
        static bool encodeVariant(QDataStream& ds, const QVariant& value);
        static bool extractVariant(QDataStream& ds, QVariant& value);
        static bool encodeVariantList(QDataStream& ds, const QVariantList& list);
        static bool extractVariantList(QDataStream& ds, QVariantList& list);
        static QString byteArrayToString(const QByteArray& ba);
        static QByteArray stringToByteArray(const QString& str);

    private:
        friend class RemoteApp;
        MessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct RequestAccessMessageData;
    class GCF_EXPORT_CLASS RequestAccessMessage : public Message
    {
    public:
        RequestAccessMessage(qint32 id=-1);
        ~RequestAccessMessage();

        void setApplicationName(const QString& appName);
        QString applicationName() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        RequestAccessMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct RequestAccessResMessageData;
    class GCF_EXPORT_CLASS RequestAccessResMessage : public Message
    {
    public:
        RequestAccessResMessage(qint32 id=-1);
        ~RequestAccessResMessage();

        void setAccessGranted(bool val);
        bool isAccessGranted() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        RequestAccessResMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct RequestObjectMessageData;
    class GCF_EXPORT_CLASS RequestObjectMessage : public Message
    {
    public:
        RequestObjectMessage(qint32 id=-1);
        ~RequestObjectMessage();

        void setObjectName(const QString& name);
        QString objectName() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        RequestObjectMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct RequestObjectResMessageData;
    class GCF_EXPORT_CLASS RequestObjectResMessage : public Message
    {
    public:
        RequestObjectResMessage(qint32 id=-1);
        ~RequestObjectResMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        bool isAccessGranted() const; /* granted if objectId() >= 0 */

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        RequestObjectResMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct GetPropertyNamesMessageData;
    class GCF_EXPORT_CLASS GetPropertyNamesMessage : public Message
    {
    public:
        GetPropertyNamesMessage(qint32 id=-1);
        ~GetPropertyNamesMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        GetPropertyNamesMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct GetPropertyNamesResMessageData;
    class GCF_EXPORT_CLASS GetPropertyNamesResMessage : public Message
    {
    public:
        GetPropertyNamesResMessage(qint32 id=-1);
        ~GetPropertyNamesResMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setPropertyNames(const QStringList& names);
        QStringList propertyNames() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        GetPropertyNamesResMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct SetPropertyMessageData;
    class GCF_EXPORT_CLASS SetPropertyMessage : public Message
    {
    public:
        SetPropertyMessage(qint32 id=-1);
        ~SetPropertyMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setPropertyName(const QString& name);
        QString propertyName() const;

        void setPropertyValue(const QVariant& value);
        QVariant propertyValue() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        SetPropertyMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct SetPropertyResMessageData;
    class GCF_EXPORT_CLASS SetPropertyResMessage : public Message
    {
    public:
        SetPropertyResMessage(qint32 id=-1);
        ~SetPropertyResMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setPropertyName(const QString& name);
        QString propertyName() const;

        void setPropertyValue(const QVariant& value);
        QVariant propertyValue() const;

        void setSuccess(bool val);
        bool isSuccess() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        SetPropertyResMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct GetPropertyMessageData;
    class GCF_EXPORT_CLASS GetPropertyMessage : public Message
    {
    public:
        GetPropertyMessage(qint32 id=-1);
        ~GetPropertyMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setPropertyName(const QString& name);
        QString propertyName() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        GetPropertyMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct GetPropertyResMessageData;
    class GCF_EXPORT_CLASS GetPropertyResMessage : public Message
    {
    public:
        GetPropertyResMessage(qint32 id=-1);
        ~GetPropertyResMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setPropertyName(const QString& name);
        QString propertyName() const;

        void setPropertyValue(const QVariant& value);
        QVariant propertyValue() const;

        void setSuccess(bool val);
        bool isSuccess() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        GetPropertyResMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct InvokeMethodMessageData;
    class GCF_EXPORT_CLASS InvokeMethodMessage : public Message
    {
    public:
        InvokeMethodMessage(qint32 id=-1);
        ~InvokeMethodMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setMethodName(const QString& name);
        QString methodName() const;

        void setArgList(const QVariantList& args);
        QVariantList argList() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        InvokeMethodMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct InvokeMethodResMessageData;
    class GCF_EXPORT_CLASS InvokeMethodResMessage : public Message
    {
    public:
        InvokeMethodResMessage(qint32 id=-1);
        ~InvokeMethodResMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setMethodName(const QString& name);
        QString methodName() const;

        void setRetValue(const QVariant& val);
        QVariant retValue() const;

        void setSuccess(bool val);
        bool isSuccess() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        InvokeMethodResMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct RequestConnectionMessageData;
    class GCF_EXPORT_CLASS RequestConnectionMessage : public Message
    {
    public:
        RequestConnectionMessage(qint32 id=-1);
        ~RequestConnectionMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setSignal(const QString& signal); // use the SIGNAL() macro
        QString signal() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        RequestConnectionMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct RequestConnectionResMessageData;
    class GCF_EXPORT_CLASS RequestConnectionResMessage : public Message
    {
    public:
        RequestConnectionResMessage(qint32 id=-1);
        ~RequestConnectionResMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setSignal(const QString& signal);
        QString signal() const;

        void setSuccess(bool val);
        bool isSuccess() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        RequestConnectionResMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct SignalDeliveryMessageData;
    class GCF_EXPORT_CLASS SignalDeliveryMessage : public Message
    {
    public:
        SignalDeliveryMessage(qint32 id=-1);
        ~SignalDeliveryMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setSignal(const QString& signal);
        QString signal() const;

        void setArgList(const QVariantList& args);
        QVariantList argList() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        SignalDeliveryMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct ReleaseObjectMessageData;
    class GCF_EXPORT_CLASS ReleaseObjectMessage : public Message
    {
    public:
        ReleaseObjectMessage(qint32 id=-1);
        ~ReleaseObjectMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        ReleaseObjectMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct ReleaseObjectResMessageData;
    class GCF_EXPORT_CLASS ReleaseObjectResMessage : public Message
    {
    public:
        ReleaseObjectResMessage(qint32 id=-1);
        ~ReleaseObjectResMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setSuccess(bool val);
        bool isSuccess() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        ReleaseObjectResMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct ReleaseAccessMessageData;
    class GCF_EXPORT_CLASS ReleaseAccessMessage : public Message
    {
    public:
        ReleaseAccessMessage(qint32 id=-1);
        ~ReleaseAccessMessage();

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        ReleaseAccessMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct ReleaseAccessResMessageData;
    class GCF_EXPORT_CLASS ReleaseAccessResMessage : public Message
    {
    public:
        ReleaseAccessResMessage(qint32 id=-1);
        ~ReleaseAccessResMessage();

        void setSuccess(bool val);
        bool isSuccess() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        ReleaseAccessResMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct ReleaseConnectionMessageData;
    class GCF_EXPORT_CLASS ReleaseConnectionMessage : public Message
    {
    public:
        ReleaseConnectionMessage(qint32 id=-1);
        ~ReleaseConnectionMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setSignal(const QString& signal); // use the SIGNAL() macro
        QString signal() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        ReleaseConnectionMessageData* d;
    };

    /////////////////////////////////////////////////////////////////////////////

    struct ReleaseConnectionMessageResData;
    class GCF_EXPORT_CLASS ReleaseConnectionResMessage : public Message
    {
    public:
        ReleaseConnectionResMessage(qint32 id=-1);
        ~ReleaseConnectionResMessage();

        void setObjectId(qint32 id);
        qint32 objectId() const;

        void setSignal(const QString& signal);
        QString signal() const;

        void setSuccess(bool val);
        bool isSuccess() const;

    protected:
        void writeBody(QDataStream& ds) const;
        void readBody(QDataStream& ds);
        void dumpMessageInfo(QTextStream& ts) const;

    private:
        ReleaseConnectionMessageResData* d;
    };

}

#endif
