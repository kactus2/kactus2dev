//-----------------------------------------------------------------------------
// File: WarningSymbol.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.11.2021
//
// Description:
// Warning symbol constructed from QPolygonf
//-----------------------------------------------------------------------------

#ifndef WARNINGSYMBOL_H
#define WARNINGSYMBOL_H

#include <QGraphicsPolygonItem>
#include <QVector2D>

//-----------------------------------------------------------------------------
//! Warning symbol constructed from QPolygonf
//-----------------------------------------------------------------------------
class WarningSymbol : public QGraphicsPolygonItem
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] busIf       The bus interface this item represents.
     *      @param [in] component   Component containing the bus interface.
     *      @param [in] library     Access to the library.
     *      @param [in] parent      The parent object.
     *      @param [in] dir         Direction for the end point graphics.
     */
    WarningSymbol(QGraphicsItem* parent);

	/*!
     *  The destructor.
     */
    virtual ~WarningSymbol() = default;

    /*!
     *  Move the warning symbol in the design according to direction.
     *
     *      @param [in] dir     The selected direction.
     */
    void moveSymbol(QVector2D const& dir);

    // Disable copying.
    WarningSymbol(WarningSymbol const& rhs) = delete;
    WarningSymbol& operator=(WarningSymbol const& rhs) = delete;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Line of the warning exclamation mark.
    QGraphicsPolygonItem* exclamationLine_;

    //! Dot of the warning exclamation mark.
    QGraphicsPolygonItem* exclamationDot_;
};

#endif // WARNINGSYMBOL_H
