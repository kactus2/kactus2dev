//-----------------------------------------------------------------------------
// File: AppPlaceholderItem.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 4.3.2011
//
// Description:
// Application placeholder graphics item.
//-----------------------------------------------------------------------------

#ifndef APPPLACEHOLDER_H
#define APPPLACEHOLDER_H

#include <QObject>
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>

class ProgramEntityItem;

//-----------------------------------------------------------------------------
//! AppPlaceholderItem class.
//-----------------------------------------------------------------------------
class AppPlaceholderItem : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    
public:
    enum { Type = UserType + 9 };

    enum
    {
        WIDTH = 200,
        HEIGHT = 40
    };

    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent node item.
     */
    AppPlaceholderItem(ProgramEntityItem* parent);

    /*!
     *  Destructor.
     */
    ~AppPlaceholderItem();

    /*!
     *  Returns the graphics item type.
     */
    int type() const;

private:
    // Disable copying.
    AppPlaceholderItem(AppPlaceholderItem const& rhs);
    AppPlaceholderItem& operator=(AppPlaceholderItem const& rhs);

    /*!
     *  Updates the file label.
     */
    void updateFileLabel();
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    enum
    {
        THIN_LINE = 1
    };

    //! The parent node item.
    ProgramEntityItem* parent_;
};

//-----------------------------------------------------------------------------

#endif // APPPLACEHOLDER_H
