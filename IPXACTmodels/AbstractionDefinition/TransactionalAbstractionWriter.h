//-----------------------------------------------------------------------------
// File: TransactionalAbstractionWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2015
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef TRANSACTIONALABSTRACTIONWRITER_H
#define TRANSACTIONALABSTRACTIONWRITER_H

#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

class TransactionalAbstraction;
class TransactionalPort;
class Protocol;

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
class TransactionalAbstractionWriter : public QObject
{
    Q_OBJECT

public:

	//! The constructor.
	TransactionalAbstractionWriter(QObject* parent = 0);

	//! The destructor.
	~TransactionalAbstractionWriter();

    void writeTransactional(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional) const;

private:

	// Disable copying.
	TransactionalAbstractionWriter(TransactionalAbstractionWriter const& rhs);
	TransactionalAbstractionWriter& operator=(TransactionalAbstractionWriter const& rhs);

    void writeQualifier(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional) const; 
    
    void writeSystem(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional) const;
    
    void writeTransactionalPort(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;
    
    void writePresence(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;
    
    void writeInitiative(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;
    
    void writeKind(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;

    void writeBusWidth(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;

    void writeProtocol(QXmlStreamWriter& writer, QSharedPointer<TransactionalPort> port) const;

    void writeProtocolType(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol) const;

    void writePayload(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol) const;

    void writeVendorExtensions(QXmlStreamWriter& writer, QSharedPointer<Protocol> portProtocol) const;

    void writeMaster(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional) const;

    void writeSlave(QXmlStreamWriter& writer, QSharedPointer<TransactionalAbstraction> transactional) const;

};

#endif // TRANSACTIONALABSTRACTIONWRITER_H