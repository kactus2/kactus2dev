//-----------------------------------------------------------------------------
// File: Packet.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 25.05.2023
//
// Description:
// Implementation for ipxact:packet element within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef PACKET_H
#define PACKET_H

#include "PacketField.h"

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>

#include <QString>
#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:packet element within abstraction definition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT Packet : public NameGroup, public Extendable
{
public:

	Packet(QString const& name = QString());

	Packet(Packet const& other);
	Packet& operator=(Packet const& other);

	~Packet() = default;

	/*!
	 *  Get the packet endianness.
	 *
	 *      @return The packet endianness.
	 */
	QString getEndianness() const;

	/*!
	 *  Set the packet endianness.
	 *
	 *      @param [in] endianness	The new packet endianness.
	 */
	void setEndianness(QString const& endianness);

	/*!
	 *  Get the packet fields.
	 *
	 *      @return The packet fields.
	 */
	QSharedPointer<QList<QSharedPointer<PacketField> > > getPacketFields() const;

	/*!
	 *  Set the packet fields.
	 *
	 *      @param [in] packetFields	The new packet fields.
	 */
	void setPacketFields(QSharedPointer<QList<QSharedPointer<PacketField> > > packetFields);

private:

	//! The packet endianness.
	QString endianness_;

	//! The packet fields.
	QSharedPointer<QList<QSharedPointer<PacketField> > > packetFields_ =
		QSharedPointer<QList<QSharedPointer<PacketField> > >(new QList<QSharedPointer<PacketField> >());
};

#endif // PACKET_H

