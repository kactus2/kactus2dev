//-----------------------------------------------------------------------------
// File: Ribbon.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.02.2013
//
// Description:
// Ribbon toolbar.
//-----------------------------------------------------------------------------

#ifndef RIBBONGROUPCONTAINER_H
#define RIBBONGROUPCONTAINER_H

#include <QWidget>
#include <QColor>
#include <QHBoxLayout>

class RibbonGroup;

//-----------------------------------------------------------------------------
//! Ribbon toolbar.
//-----------------------------------------------------------------------------
class Ribbon : public QWidget
{
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
    ~Ribbon();

    /*!
     *  Adds a group to the ribbon.
     *
     *      @param [in] title The title for the group.
     */
    RibbonGroup* addGroup(QString const& title);

protected:
    //! Draws the ribbon background.
    virtual void paintEvent(QPaintEvent* event);

private:
    // Disable copying.
    Ribbon(Ribbon const& rhs);
    Ribbon& operator=(Ribbon const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The layout for the ribbon groups.
    QHBoxLayout* layout_;

	//! The height of the ribbon.
	const int RIBBONHEIGHT_;
};

//-----------------------------------------------------------------------------

#endif // RIBBONGROUPCONTAINER_H
