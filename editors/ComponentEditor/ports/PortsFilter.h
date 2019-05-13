//-----------------------------------------------------------------------------
// File: PortsFilter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.05.2019
//
// Description:
// Sorting proxy model for ports.
//-----------------------------------------------------------------------------

#ifndef PORTSFILTER_H
#define PORTSFILTER_H

#include <QSortFilterProxyModel>

class Port;

//-----------------------------------------------------------------------------
//! Sorting proxy model for ports.
//-----------------------------------------------------------------------------
class PortsFilter : public QSortFilterProxyModel
{
    Q_OBJECT

public:

	 /*!
      *  Constructor.
      *
      *      @param [in] parent     The parent object.
      */
    PortsFilter(QObject* parent = 0);

	 /*!
      *  Destructor.
      */
    virtual ~PortsFilter() = default;

    // No copying. No assignments.
    PortsFilter(PortsFilter const& rhs) = delete;
    PortsFilter& operator=(PortsFilter const& rhs) = delete;

public slots:

    /*!
     *  Handle the item change for the extensions editor.
     *
     *      @param [in] index   Index of the selected item.
     */
    void onHandleExtensionsEditorItemChange(QModelIndex const& index);

signals:

    /*!
     *  Change the item in the extensions editor.
     *
     *      @param [in] index   Index of the selected item.
     */
    void changeExtensionsEditorItem(QModelIndex const& index);

protected:

    /*!
	 *  Implementation of the row filtering.
	 *
	 *      @param [in] source_row      The row to check for filtering.
     *      @param [in] source_parent   The parent index of the row.
     *
     *      @return True, if the row passes the filters, otherwise false.
	 */
    virtual bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const override;

private:

    /*!
     *  Check if a port is filtered.
     *
     *      @param [in] port    The selected port.
     *
     *      @return True, if the port is accepted, false otherwise.
     */
    virtual bool portIsAccepted(QSharedPointer<Port> port) const = 0;
};

#endif // PORTSFILTER_H