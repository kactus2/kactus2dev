//-----------------------------------------------------------------------------
// File: EnumeratedValueReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Reader class for IP-XACT enumerated value element.
//-----------------------------------------------------------------------------

#ifndef ENUMERATEDVALUEREADER_H
#define ENUMERATEDVALUEREADER_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

class EnumeratedValue;

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT enumerated value element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT EnumeratedValueReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    EnumeratedValueReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~EnumeratedValueReader();

    /*!
     *  Creates a new enumerated value from a given enumerated value node.
     *
     *      @param [in] enumeratedValueNode     XML description of the enumerated value.
     *
     *      @return The created enumerated value.
     */
    QSharedPointer<EnumeratedValue> createEnumeratedValueFrom(QDomNode const& enumeratedValueNode) const;

private:

    //! No copying allowed.
    EnumeratedValueReader(EnumeratedValueReader const& rhs);
    EnumeratedValueReader& operator=(EnumeratedValueReader const& rhs);

    /*!
     *  Reads the usage.
     *
     *      @param [in] enumerationNode     XML description of the enumerated value.
     *      @param [in] newEnumeration      The new enumerated item.
     */
    void parseUsage(QDomNode const& enumerationNode, QSharedPointer<EnumeratedValue> newEnumeration) const;

    /*!
     *  Reads the name group.
     *
     *      @param [in] enumerationNode     XML description of the enumerated value.
     *      @param [in] newEnumeration      The new enumerated item.
     */
    void parseNameGroup(QDomNode const& enumerationNode, QSharedPointer<EnumeratedValue> newEnumeration) const;

    /*!
     *  Reads the value.
     *
     *      @param [in] enumerationNode     XML description of the enumerated value.
     *      @param [in] newEnumeration      The new enumerated item.
     */
    void parseValue(QDomNode const& enumerationNode, QSharedPointer<EnumeratedValue> newEnumeration) const;
};

#endif // ENUMERATEDVALUEREADER_H