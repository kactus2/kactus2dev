//-----------------------------------------------------------------------------
// File: PacketField.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 25.05.2023
//
// Description:
// Implementation for ipxact:packetField element within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef PACKETFIELD_H
#define PACKETFIELD_H

#include <IPXACTmodels/common/Extendable.h>
#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Qualifier.h>

#include <QString>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:packetField element within abstraction definition.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT PacketField : public NameGroup, public Extendable
{
public:

    PacketField(QString const& name = QString());

    ~PacketField() = default;
    
    /*!
     *  Get the packet field value.
     *
     *      @return The packet field value.
     */
    QString getValue() const;

    /*!
     *  Set the packet field value.
     *
     *      @param [in] value   The new packet field value.
     */
    void setValue(QString const& value);

    /*!
     *  Get the packet field endianness.
     *
     *      @return The packet field endianness.
     */
    QString getEndianness() const;
    
    /*!
     *  Set the packet field endianness.
     *
     *      @param [in] endianness  The new packet field endianness.
     */
    void setEndianness(QString const& endianness);
    
    /*!
     *  Get the packet field qualifier.
     *
     *      @return The packet field qualifier.
     */
    QSharedPointer<Qualifier> getQualifier() const;
    
    /*!
     *  Set the packet field qualifier.
     *
     *      @param [in] qualifier   The new packet field qualifier.
     */
    void setQualifier(QSharedPointer<Qualifier> const& qualifier);

private:

    //! The packet field value.
    QString value_;

    //! The packet field endianness.
    QString endianness_;

    //! The packet field qualifier.
    QSharedPointer<Qualifier> qualifier_;

};

#endif // PACKETFIELD_H

