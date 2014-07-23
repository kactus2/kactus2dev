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

#include "ipxactmodels_global.h"
#include <designEditors/common/ColumnTypes.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! ColumnDesc structure which describes the kactus2:column element
//! in an IP-XACT document.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ColumnDesc
{
public:
    /*!
     *  Default constructor.
     */
    ColumnDesc();

    /*!
     *  Constructor which parses the column description from an XML node.
     *
     *      @param [in] node A QDomNode from where the information is parsed.
     */
    ColumnDesc(const QDomNode& node);

    /*!
     *  Constructor which initializes the column description fully.
     */
    ColumnDesc(QString const& name, ColumnContentType contentType,
               unsigned int allowedItems = CIT_NONE, unsigned int minWidth = 259);

    /*!
     *  Writes the column description to an XML stream.
     */
    void write(QXmlStreamWriter& writer) const;

    /*!
     *  Sets the name of the column.
     *
     *      @param [in] name The name of the column.
     */
    void setName(QString const& name);

    /*!
     *  Sets the content type.
     *
     *      @param [in] contentType The content type.
     */
    void setContentType(ColumnContentType contentType);

    /*!
     *  Sets the allowed items.
     *
     *      @param [in] allowedItems The allowed items as bit flags.
     */
    void setAllowedItems(unsigned int allowedItems);

    /*!
     *  Sets the width of the column.
     *
     *      @param [in] width The width of the column.
     */
    void setWidth(unsigned int width);

    /*!
     *  Returns the name of the column.
     */
    QString const& getName() const;

    /*!
     *  Returns the content type.
     */
    ColumnContentType getContentType() const;

    /*!
     *  Returns the allowed items.
     */
    unsigned int getAllowedItems() const;

    /*!
     *  Returns the width of the column.
     */
    unsigned int getWidth() const;

    //! \brief Copy constructor
    ColumnDesc(const ColumnDesc& other);

    //! \brief Assignment operator
    ColumnDesc& operator=(const ColumnDesc& other);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the column.
    QString name_;

    //! The content type for the column.
    ColumnContentType contentType_;

    //! The allowed items for the column.
    unsigned int allowedItems_;

    //! The width of the column.
    unsigned int width_;

    //! The minimum width for the column.
    unsigned int minWidth_;
};

//-----------------------------------------------------------------------------

#endif // COLUMNDESC_H
