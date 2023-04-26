//-----------------------------------------------------------------------------
// File: Ribbon.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 06.02.2013
//
// Description:
// Ribbon toolbar.
//-----------------------------------------------------------------------------

#ifndef RIBBON_H
#define RIBBON_H

#include <QWidget>
#include <QColor>
#include <QHBoxLayout>
#include <QScrollArea>

#include <QPushButton>
#include <QToolBar>

class RibbonGroup;

//-----------------------------------------------------------------------------
//! Ribbon toolbar.
//-----------------------------------------------------------------------------
class Ribbon : public QToolBar
{
	Q_OBJECT

public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent widget.
     */
    Ribbon(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    virtual ~Ribbon() = default;

    // Disable copying.
    Ribbon(Ribbon const& rhs) = delete;
    Ribbon& operator=(Ribbon const& rhs) = delete;

    /*!
     *  Adds a group to the ribbon.
     *
     *      @param [in] title The title for the group.
     */
    QAction* addGroup(RibbonGroup* group);

protected:
    //! Draws the ribbon background.
    virtual void paintEvent(QPaintEvent* event);

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------


	//! Spacing amount for the container.
	static const int CONTAINER_SPACING = 1;

	//! The height of the ribbon.
    static const int RIBBONHEIGHT = 64;
};

//-----------------------------------------------------------------------------

#endif // RIBBON_H
