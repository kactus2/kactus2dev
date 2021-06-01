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
#include <QSharedPointer>

class PortsInterface;

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
      *      @param [in] portsInterface     Interface for accessing the component ports.
      *      @param [in] parent             The parent object.
      */
    PortsFilter(QSharedPointer<PortsInterface> portInterface, QObject* parent = 0);

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

    /*!
     *  Get the interface for accessing the component ports.
     *
     *      @return Interface for accessing the component ports.
     */
    QSharedPointer<PortsInterface> getInterface() const;

private:

    /*!
     *  Get the name column.
     *
     *      @return Index of the name column.
     */
    virtual int nameColumn() const = 0;

    /*!
     *  Check if a port is filtered.
     *
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the port is accepted, false otherwise.
     */
    virtual bool portIsAccepted(QString const& portName) const = 0;

    //! Interface for accessing the component ports.
    QSharedPointer<PortsInterface> portInterface_;
};

#endif // PORTSFILTER_H