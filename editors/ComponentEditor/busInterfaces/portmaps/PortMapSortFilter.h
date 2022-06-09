//-----------------------------------------------------------------------------
// File: PortMapSortFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.06.2022
//
// Description:
// Sort filter proxy model for port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPSORTFILTER_H
#define PORTMAPSORTFILTER_H

#include <QSortFilterProxyModel>
#include <QColor>

class PortMapInterface;

//-----------------------------------------------------------------------------
//! Sort filter proxy model for port maps.
//-----------------------------------------------------------------------------
class PortMapSortFilter : public QSortFilterProxyModel
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] portMapInterface    Interface for accessing port maps.
     *      @param [in] parent              Pointer to the owner of this model.
     */
    PortMapSortFilter(PortMapInterface* portMapInterface, QObject *parent);

    /*!
     *  The destructor.
     */
    virtual ~PortMapSortFilter() = default;

    /*!
     *  Get the data for the specified item.
     *
     *      @param [in] index   Specifies the item that's data is requested.
     *      @param [in] role    The role that defines what kind of data is requested.
     *
     *      @return QVariant contains the data for the item.
     */
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

protected:

    /*!
     *  Get the background color for the selected index.
     *
     *      @param [in] index   Index of the selected item.
     *
     *      @return Background color for the selected index.
     */
    virtual QColor getBackgroundColorForIndex(QModelIndex const& index) const;

private:
    //! No copying. No assignment.
    PortMapSortFilter(const PortMapSortFilter& other);
    PortMapSortFilter& operator=(const PortMapSortFilter& other);

    /*!
     *  Get the background colour on the previous row of the selected index.
     *
     *      @param [in] previousRow     The previous row.
     *      @param [in] index           The selected index.
     *
     *      @return Background colour of the previous row.
     */
    QColor getPreviousColor(int const& previousRow, QModelIndex const& index) const;

    //! Interface for accessing port maps.
    PortMapInterface* portMapInterface_;

};

#endif // PORTMAPSORTFILTER_H
