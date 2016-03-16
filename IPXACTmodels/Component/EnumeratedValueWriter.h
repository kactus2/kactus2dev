//-----------------------------------------------------------------------------
// File: EnumeratedValueWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Writer class for IP-XACT enumerated value element.
//-----------------------------------------------------------------------------

#ifndef ENUMERATEDVALUEWRITER_H
#define ENUMERATEDVALUEWRITER_H

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

class EnumeratedValue;

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT enumerated value element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT EnumeratedValueWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    EnumeratedValueWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~EnumeratedValueWriter();

    /*!
     *  Write an enumerated value to an XML file.
     *
     *      @param [in] writer              The used xml writer.
     *      @param [in] enumeratedValue     The enumerated value to be written.
     */
    void writeEnumeratedValue(QXmlStreamWriter& writer, QSharedPointer<EnumeratedValue> enumeratedValue) const;

private:

    //! No copying allowed.
    EnumeratedValueWriter(EnumeratedValueWriter const& rhs);
    EnumeratedValueWriter& operator=(EnumeratedValueWriter const& rhs);

    /*!
     *  Write the enumeration usage.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] enumeratedValue     The selected enumerated value item.
     */
    void writeUsage(QXmlStreamWriter& writer, QSharedPointer<EnumeratedValue> enumeratedValue) const;

    /*!
     *  Write the name group.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] enumeratedValue     The selected enumerated value item.
     */
    void writeNameGroup(QXmlStreamWriter& writer, QSharedPointer<EnumeratedValue> enumeratedValue) const;

    /*!
     *  Write the value.
     *
     *      @param [in] writer              Used XML writer.
     *      @param [in] enumeratedValue     The selected enumerated value item.
     */
    void writeValue(QXmlStreamWriter& writer, QSharedPointer<EnumeratedValue> enumeratedValue) const;
};

#endif // ENUMERATEDVALUEWRITER_H