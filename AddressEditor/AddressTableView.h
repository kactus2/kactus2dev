//-----------------------------------------------------------------------------
// File: AddressTableView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 03.09.2012
//
// Description:
// Table view with reordering support.
//-----------------------------------------------------------------------------

#ifndef ADDRESSTABLEVIEW_H
#define ADDRESSTABLEVIEW_H

#include <QTableView>

//-----------------------------------------------------------------------------
//! Table view with reordering support.
//-----------------------------------------------------------------------------
class AddressTableView : public QTableView
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    AddressTableView(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~AddressTableView();

signals:
    /*! \brief Move item to another position.
	 *
	 * \param originalPos Identifies the item that should be moved.
	 * \param newPos The new position the item should be moved to.
	 *
	*/
	void moveItem(const QModelIndex& originalPos, const QModelIndex& newPos);

protected:
    //! \brief Handler for mouse move events
    virtual void mouseMoveEvent(QMouseEvent* e);

    //! \brief Handler for mouse press events
    virtual void mousePressEvent(QMouseEvent* event);

    //! \brief Handler for mouse release events
    virtual void mouseReleaseEvent(QMouseEvent* event);

private:
    // Disable copying.
    AddressTableView(AddressTableView const& rhs);
    AddressTableView& operator=(AddressTableView const& rhs);
    
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! \brief The point where mouse was clicked
    QPoint pressedPoint_;
};

//-----------------------------------------------------------------------------

#endif // ADDRESSTABLEVIEW_H
