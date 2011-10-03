//-----------------------------------------------------------------------------
// File: HighlightStyleDesc.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 17.6.2011
//
// Description:
// Basic highlight style description.
//-----------------------------------------------------------------------------

#ifndef HIGHLIGHTSTYLEDESC_H
#define HIGHLIGHTSTYLEDESC_H

#include <QColor>
#include <QMetaType>
#include <QDataStream>

//-----------------------------------------------------------------------------
//! HighlightStyleDesc structure.
//-----------------------------------------------------------------------------
struct HighlightStyleDesc
{
    QColor color;       //!< The highlight color for the style.
    bool bold;          //!< True for bold; false for normal text.
    bool italic;        //!< True for italic; false for non-italic.

    /*!
     *  Default constructor.
     */
    HighlightStyleDesc() : color(Qt::black), bold(false), italic(false)
    {
    }

    /*!
     *  Constructor which takes the values for all fields.
     */
    HighlightStyleDesc(QColor color, bool bold, bool italic) : color(color), bold(bold), italic(italic)
    {
    }
};

inline QDataStream& operator<<(QDataStream& out, HighlightStyleDesc const& obj)
{
    out << obj.color << obj.bold << obj.italic;
    return out;
}

inline QDataStream& operator>>(QDataStream& in, HighlightStyleDesc& obj)
{
    in >> obj.color >> obj.bold >> obj.italic;
    return in;
}

Q_DECLARE_METATYPE(HighlightStyleDesc);

//-----------------------------------------------------------------------------

#endif // HIGHLIGHTSTYLEDESC_H
