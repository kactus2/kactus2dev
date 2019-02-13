//-----------------------------------------------------------------------------
// File: AutoConnectorList.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 12.02.2019
//
// Description:
// Lists items for auto connector.
//-----------------------------------------------------------------------------

#ifndef AUTOCONNECTORLIST_H
#define AUTOCONNECTORLIST_H

#include <QListWidget>
#include <QSharedPointer>
#include <QObject>

//-----------------------------------------------------------------------------
//! Lists items for auto connector.
//-----------------------------------------------------------------------------
class AutoConnectorList : public QListWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent          The parent item.
     */
    AutoConnectorList(QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    ~AutoConnectorList();

    // No copying. No assignments.
    AutoConnectorList(AutoConnectorList const& rhs) = delete;
    AutoConnectorList& operator=(AutoConnectorList const& rhs) = delete;

protected:

    /*!
     *  Handles the mouse press events. Used for creating the drag mime data.
     *
     *      @param [in] event   The mouse event.
     */
    virtual void mousePressEvent(QMouseEvent *event);

};

#endif // AUTOCONNECTORLIST_H
