//-----------------------------------------------------------------------------
// File: bridgesmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 21.06.2012
//
// Description:
// The model to manage the bridges of a slave interface.
//-----------------------------------------------------------------------------

#ifndef BRIDGESMODEL_H
#define BRIDGESMODEL_H

#include <IPXACTmodels/Component/TransparentBridge.h>
#include <IPXACTmodels/common/Document.h>

#include <QAbstractTableModel>
#include <QSharedPointer>

class TransparentBridgeInterface;

//-----------------------------------------------------------------------------
//! The model to manage the bridges of a slave interface.
//-----------------------------------------------------------------------------
class BridgesModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *    @param [in] bridgeInterface     Interface for accessing transparent bridges.
     *    @param [in] docRevision         The IP-XACT version to comply to.
	 *    @param [in] parent              The parent of this model.
     */
    BridgesModel(TransparentBridgeInterface* bridgeInterface, Document::Revision docRevision, QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~BridgesModel();

	/*!
     *  Get the number of rows an item contains.
	 *
	 *    @param [in] parent Identifies the parent that's row count is requested.
	 *
	 *    @return Number of rows the item has.
     */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *    @param [in] parent Identifies the parent that's column count is requested.
	 *
	 *    @return The number of columns to be displayed.
     */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the item flags that defines the possible operations for the item.
	 *
	 *    @param [in] index Model index that identifies the item.
	 *
	 *    @return Qt::ItemFlags specify the possible operations for the item.
     */
	Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Get the header data for specified header.
	 *
	 *    @param [in] section         The section specifies the row/column number for the header.
	 *    @param [in] orientation     Specified if horizontal or vertical header is wanted.
	 *    @param [in] role            Specifies the type of the requested data.
	 *
	 *    @return QVariant Contains the requested data.
     */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for specified item.
	 *
	 *    @param [in] index   Specifies the item that's data is requested.
	 *    @param [in] role    The role that defines what kind of data is requested.
	 *
	 *    @return QVariant Contains the data for the item.
     */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item
	 *
	 *    @param [in] index   The model index of the item that's data is to be saved.
	 *    @param [in] value   The data that is to be saved.
	 *    @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *    @return True if saving happened successfully.
     */
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);

	/*!
     *  Refresh the model to match the slave interface.
     */
    void refresh();

public slots:

	/*! Add a new item to the given index.
	 *
	 *    @param [in] index The index identifying the position for new item.
	 */
	virtual void onAddItem(QModelIndex const& index);

	/*! Remove the item in the given index.
	 *
	 *    @param [in] index The index identifying the item to remove.
	 */
	virtual void onRemoveItem(QModelIndex const& index);

signals:

	//! Emitted when contents of the model change.
	void contentChanged();

private:

	//! No copying
	BridgesModel(const BridgesModel& other);
	BridgesModel& operator=(const BridgesModel& other);

	//! Interface for accessing transparent bridges.
    TransparentBridgeInterface* bridgeInterface_;

    //! The IP-XACT version to comply to.
    Document::Revision documentRevision_;
};

#endif // BRIDGESMODEL_H
