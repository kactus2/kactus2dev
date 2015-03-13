//-----------------------------------------------------------------------------
// File: ArrayView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.03.2015
//
// Description:
// Editor for changing values in arrays.
//-----------------------------------------------------------------------------

#ifndef ARRAYVIEW_H
#define ARRAYVIEW_H

#include <QTableView>

//-----------------------------------------------------------------------------
//! Editor for changing values in arrays.
//-----------------------------------------------------------------------------
class ArrayView : public QTableView
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The parent of this view.
     */
    ArrayView(QWidget* parent);

    /*!
     *  The destructor.
     */
    virtual ~ArrayView();

    /*!
     *  Reimplemented to do nothing when keyboard search is initiated.
     *
     *      @param [in] search  The found item should be the best match for this string.
     */
    virtual void keyboardSearch(const QString &search);

    /*!
     *  The event filter for this view.
     *
     *      @param [in] filterObject    The object to be filtered.
     *      @param [in] filterEvent     The event to be filtered.
     */
    virtual bool eventFilter(QObject *filterObject, QEvent *filterEvent);

private:
    //! Disable copying and assignment.
    ArrayView(const ArrayView& other);
    ArrayView& operator=(const ArrayView& other);
};

#endif // ARRAYVIEW_H