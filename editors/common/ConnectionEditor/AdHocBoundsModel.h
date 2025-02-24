//-----------------------------------------------------------------------------
// File: AdHocBoundsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Joni-Matti Maatta
// Date: 20.2.2012
//
// Description:
// Table model for visualizing ad-hoc bounds for component ports.
//-----------------------------------------------------------------------------

#ifndef ADHOCBOUNDSMODEL_H
#define ADHOCBOUNDSMODEL_H

#include <QAbstractTableModel>
#include <QSharedPointer>

class PortReference;

class AdHocConnection;
class IEditProvider;

//-----------------------------------------------------------------------------
//! Table model for visualizing ad-hoc visibility for component ports.
//-----------------------------------------------------------------------------
class AdHocBoundsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  Constructor.
	 *
     *    @param [in] editProvider  The edit provider managing the undo/redo stack.
	 *    @param [in] parent        The owner of this model.
	 */
	AdHocBoundsModel(QObject *parent);
	
	//! The destructor.
	virtual ~AdHocBoundsModel();

    /*!
     *  Sets the ad-hoc connection being edited.
     *
     *    @param [in] connection      The connection.
     *    @param [in] editProvider    The edit provider for undo/redo.
     */
    void setConnection(QSharedPointer<AdHocConnection> connection, QSharedPointer<IEditProvider> editProvider);

	/*!
     *  Returns the number of rows in the model.
	 *
	 *    @param [in] parent Model index of the parent of the item.
	 *
	 *    @return Number of rows currently in the model.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Returns the number of columns in the model.
	 *
	 *    @param [in] parent Model index of the parent of the item.
	 *
	 *    @return Number of columns currently in the model.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Returns the data for the specified item for specified role.
	 *
	 *    @param [in] index  Identifies the item that's data is wanted.
	 *    @param [in] role   Specifies what kind of data is wanted
	 *
	 *    @return The data in the requested index.
	 */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Returns the data for the headers.
	 *
	 *    @param [in] section      The column that's header is wanted
	 *    @param [in] orientation  Only Qt::Horizontal is supported
	 *    @param [in] role         Specified the type of data that is wanted.
	 *
	 *    @return The data to be displayed.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Set the data for specified item.
	 *
	 *    @param [in] index  Specifies the item that's data is modified
	 *    @param [in] value  The value to be set.
	 *    @param [in] role   The role that is trying to modify the data.
	 *
	 *    @return True if data was successfully set, otherwise false.
	 */
	virtual bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);

	/*!
     *  Returns information on how specified item can be handled.
	 *
	 *    @param [in] index Specifies the item that's flags are wanted.
	 *
	 *    @return Qt::ItemFlags that define how object can be handled.
	 */
	virtual Qt::ItemFlags flags(QModelIndex const& index) const;

    
signals:
	//! Emitted when contents of the model change
	void contentChanged();

private:
    // Disable copying.
    AdHocBoundsModel(AdHocBoundsModel const& rhs);
    AdHocBoundsModel& operator=(AdHocBoundsModel const& rhs);

    /*!
     *  Finds the number of endpoint in the ad-hoc connection.
     *
     *    @return The number of endpoints defined in the connection.
     */
    int getEndpointCount() const;

    /*!
     *  Gets the port reference on the given row.
     *
     *    @param [in] row   The row to find.
     *
     *    @return The port reference on the given row.
     */
    QSharedPointer<PortReference> getEndpoint(int row) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The edit provider for undo/redo to use.
    QSharedPointer<IEditProvider> editProvider_;

    //! The ad-hoc connection currently being edited.
    QSharedPointer<AdHocConnection> connection_;
};

//-----------------------------------------------------------------------------

#endif // ADHOCBOUNDSMODEL_H