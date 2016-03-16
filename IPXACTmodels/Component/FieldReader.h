//-----------------------------------------------------------------------------
// File: FieldReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 25.09.2015
//
// Description:
// Reader class for IP-XACT field element.
//-----------------------------------------------------------------------------

#ifndef FIELDREADER_H
#define FIELDREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

class Field;

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT field element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FieldReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    FieldReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~FieldReader();

    /*!
     *  Creates a new field from a given field node.
     *
     *      @param [in] fieldNode   XML description of the field.
     *
     *      @return The created field.
     */
    QSharedPointer<Field> createFieldFrom(QDomNode const& fieldNode) const;

private:

    //! No copying allowed.
    FieldReader(FieldReader const& rhs);
    FieldReader& operator=(FieldReader const& rhs);

    /*!
     *  Reads the field ID.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseID(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the name group.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseNameGroup(QDomNode const& fieldNode, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the is present value.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parsePresence(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the bit offset.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseBitOffset(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the reset.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseReset(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the reset type reference.
     *
     *      @param [in] resetElement    XML description of the reset.
     *      @param [in] newField        The new field item.
     */
    void parseResetTypeRef(QDomElement const& resetElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the reset value.
     *
     *      @param [in] resetElement    XML description of the reset.
     *      @param [in] newField        The new field item.
     */
    void parseResetValue(QDomElement const& resetElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the reset mask.
     *
     *      @param [in] resetElement    XML description of the reset.
     *      @param [in] newField        The new field item.
     */
    void parseResetMask(QDomElement const& resetElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the type identifier.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseTypeIdentifier(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the bit width.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseBitWidth(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the volatile value.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseVolatile(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the access value.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseAccess(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the enumerated values.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseEnumeratedValues(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the modified write value.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseModifiedWriteValue(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the write value constraint.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseWriteValueConstraint(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the read action value.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseReadAction(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the testable value.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseTestable(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the reserved value.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseReserved(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;

    /*!
     *  Reads the parameters.
     *
     *      @param [in] fieldElement    XML description of the field.
     *      @param [in] newField        The new field item.
     */
    void parseParameters(QDomElement const& fieldElement, QSharedPointer<Field> newField) const;
};

#endif // FIELDREADER_H