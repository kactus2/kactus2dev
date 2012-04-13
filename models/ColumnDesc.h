//-----------------------------------------------------------------------------
// File: ColumnDesc.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.4.2012
//
// Description:
// Column description structure for designs.
//-----------------------------------------------------------------------------

#ifndef COLUMNDESC_H
#define COLUMNDESC_H

#include <common/ColumnTypes.h>

#include <QString>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! ColumnDesc structure which describes the kactus2:column element
//! in an IP-XACT document.
//-----------------------------------------------------------------------------
struct ColumnDesc
{
    QString name;                       //!< The name of the column.
    ColumnContentType contentType;      //!< The content type for the column.
    unsigned int allowedItems;          //!< The allowed items for the column.

    /*!
     *  Constructor which parses the column description from an XML node.
     *
     *      @param [in] node A QDomNode from where the information is parsed.
     */
    ColumnDesc(const QDomNode& node);

    /*!
     *  Convenience constructor.
     */
    ColumnDesc(QString const& name, ColumnContentType contentType,
               unsigned int allowedItems);

    //! \brief Copy constructor
    ColumnDesc(const ColumnDesc& other);

    //! \brief Assignment operator
    ColumnDesc& operator=(const ColumnDesc& other);
};

//-----------------------------------------------------------------------------

#endif // COLUMNDESC_H
