//-----------------------------------------------------------------------------
// File: ColumnDesc.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 12.4.2012
//
// Description:
// ColumnDesc structure describes the kactus2:column vendor extension element in an IP-XACT design.
//-----------------------------------------------------------------------------

#ifndef COLUMNDESC_H
#define COLUMNDESC_H

#include <IPXACTmodels/common/VendorExtension.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <designEditors/common/ColumnTypes.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! ColumnDesc structure describes the kactus2:column vendor extension element in an IP-XACT design.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT ColumnDesc : public VendorExtension
{
public:
    /*!
     *  Default constructor.
     */
    ColumnDesc();

    /*!
     *  Constructor which initializes the column description fully.
     */
    ColumnDesc(QString const& name, ColumnTypes::ColumnContentType contentType,
               unsigned int allowedItems = ColumnTypes::NONE, unsigned int minWidth = 259);

    /*!
     *  Clone constructor.
     */
    virtual ColumnDesc* clone() const;

    /*!
     *  Get the type of the extension.
     *
     *      @return The type of the extension.
     */
    virtual QString type() const;

    /*!
     *  Writes the column description to an XML stream.
     */
    virtual void write(QXmlStreamWriter& writer) const;

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
    void setContentType(ColumnTypes::ColumnContentType contentType);

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

        
    void setMinimumWidth(int minimum);

    /*!
     *  Returns the name of the column.
     */
    QString name() const;

    /*!
     *  Returns the content type.
     */
    ColumnTypes::ColumnContentType getContentType() const;

    /*!
     *  Returns the allowed items.
     */
    unsigned int getAllowedItems() const;

    /*!
     *  Returns the width of the column.
     */
    unsigned int getWidth() const;

    unsigned int getPosition() const;

    void setPosition(unsigned int x);

    //! Copy constructor
    ColumnDesc(const ColumnDesc& other);

    //! Assignment operator
    ColumnDesc& operator=(const ColumnDesc& other);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the column.
    QString name_;

    //! The content type for the column.
    ColumnTypes::ColumnContentType contentType_;

    //! The allowed items for the column.
    unsigned int allowedItems_;

    //! The width of the column.
    unsigned int width_;

    //! The minimum width for the column.
    unsigned int minWidth_;

    unsigned int xPosition_;
};

//-----------------------------------------------------------------------------

#endif // COLUMNDESC_H
