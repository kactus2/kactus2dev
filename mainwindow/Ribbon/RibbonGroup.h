//-----------------------------------------------------------------------------
// File: RibbonGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.02.2013
//
// Description:
// Ribbon group class.
//-----------------------------------------------------------------------------

#ifndef RIBBONGROUP_H
#define RIBBONGROUP_H

#include <QWidget>
#include <QMenu>
#include <QToolBar>

class Ribbon;

//-----------------------------------------------------------------------------
//! Ribbon group class.
//-----------------------------------------------------------------------------
class RibbonGroup : public QToolBar
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] title   The group title.
     *      @param [in] parent  The parent ribbon.
     */
    RibbonGroup(QString const& title, Ribbon* parent);

    /*!
     *  Destructor.
     */
    ~RibbonGroup();

protected:
    //! Draws the group.
    virtual void paintEvent(QPaintEvent* event);

private:
    // Disable copying.
    RibbonGroup(RibbonGroup const& rhs);
    RibbonGroup& operator=(RibbonGroup const& rhs);

    enum
    {
        TITLE_MARGIN = 20,
        TITLE_HEIGHT = 25,
        MIN_LARGE_BUTTON_WIDTH = 56,
        HMARGIN = 5
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The title text.
    QString title_;

};

//-----------------------------------------------------------------------------

#endif // RIBBONGROUP_H
