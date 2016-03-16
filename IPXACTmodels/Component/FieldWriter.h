//-----------------------------------------------------------------------------
// File: FieldWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 24.09.2015
//
// Description:
// Writer class for IP-XACT field element.
//-----------------------------------------------------------------------------

#ifndef FIELDWRITER_H
#define FIELDWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

class Field;
class WriteValueConstraint;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT field element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FieldWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    FieldWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~FieldWriter();

    /*!
     *  Write a field to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] field   The field to be written.
     */
    void writeField(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;

private:

    //! No copying allowed.
    FieldWriter(FieldWriter const& rhs);
    FieldWriter& operator=(FieldWriter const& rhs);

    /*!
     *  Write the field id attribute.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] fieldID     The field id.
     */
    void writeID(QXmlStreamWriter& writer, QString const& fieldID) const;

    /*!
     *  Write the name group.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] field   The selected field item.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;

    /*!
     *  Write the reset.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] field   The selected field item.
     */
    void writeReset(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;

    /*!
     *  Write the type identifier.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] field   The selected field item.
     */
    void writeTypeIdentifier(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;

    /*!
     *  Write the volatile value.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] field   The selected field item.
     */
    void writeVolatile(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;

    /*!
     *  Write the access value.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] field   The selected field item.
     */
    void writeAccess(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;

    /*!
     *  Write the enumerated values.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] field   The selected field item.
     */
    void writeEnumerations(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;

    /*!
     *  Write the modified write value.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] field   The selected field item.
     */
    void writeModifiedWriteValue(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;

    /*!
     *  Write the write value constraint.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] writeConstaint  The field write value constraint.
     */
    void writeWriteValueConstraint(QXmlStreamWriter& writer, QSharedPointer<WriteValueConstraint> writeConstraint)
        const;

    /*!
     *  Write the read action.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] field   The selected field item.
     */
    void writeReadAction(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;

    /*!
     *  Write the testable element.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] field   The selected field item.
     */
    void writeTestable(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;

    /*!
     *  Write the reserved value.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] field   The selected field item.
     */
    void writeReserved(QXmlStreamWriter& writer, QSharedPointer<Field> field) const;
};

#endif // FIELDWRITER_H