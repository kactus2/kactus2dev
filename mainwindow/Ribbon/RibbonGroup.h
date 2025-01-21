//-----------------------------------------------------------------------------
// File: RibbonGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
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
     *    @param [in] title   The group title.
     *    @param [in] parent  The parent ribbon.
     */
    RibbonGroup(QString const& title, Ribbon* parent);

    /*!
     *  Destructor.
     */
    virtual ~RibbonGroup() = default;

    // Disable copying.
    RibbonGroup(RibbonGroup const& rhs) = delete;
    RibbonGroup& operator=(RibbonGroup const& rhs) = delete;

protected:
    //! Draws the group.
    virtual void paintEvent(QPaintEvent* event);

private:

    enum
    {
        TITLE_MARGIN = 10,
        TITLE_HEIGHT = 21,
        MIN_LARGE_BUTTON_WIDTH = 56,
        HMARGIN = 1
    };

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The title text.
    QString title_;

};

//-----------------------------------------------------------------------------

#endif // RIBBONGROUP_H
