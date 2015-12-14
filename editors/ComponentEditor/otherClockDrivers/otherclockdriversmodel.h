//-----------------------------------------------------------------------------
// File: otherclockdriversmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 04.04.2011
//
// Description:
// Table model that contains the items to edit otherClockDrivers of a component.
//-----------------------------------------------------------------------------

#ifndef OTHERCLOCKDRIVERSMODEL_H
#define OTHERCLOCKDRIVERSMODEL_H

#include <QAbstractTableModel>

#include <QList>
#include <QSharedPointer>

class Component;
class OtherClockDriver;
class OtherClockDriverValidator;
//-----------------------------------------------------------------------------
//! Table model that contains the items to edit otherClockDrivers of a component.
//-----------------------------------------------------------------------------
class OtherClockDriversModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor
	 *
	 *      @param [in] component       The component being edited.
     *      @param [in] clockValidator  Validator used for other clock drivers.
	 *      @param [in] parent          The owner of this model
	 */
	OtherClockDriversModel(QSharedPointer<Component> component,
        QSharedPointer<OtherClockDriverValidator> clockValidator, QObject *parent);
	
	//! The destructor.
	~OtherClockDriversModel();

	/*!
     *  Get the number of rows in the model.
	 *
	 *      @param [in] parent  Model index of the parent of the item. Must be invalid because this is not 
     *                          hierarchical model.
	 *
	 *      @return Number of rows currently in the model.
	 */
	virtual int rowCount(QModelIndex const&  parent = QModelIndex()) const;

	/*!
     *  Get the number of columns in the model
	 *
	 *      @param [in] parent  Model index of the parent of the item. Must be invalid because this is not 
     *                          hierarchical model.
	 *
	 *      @return Always returns 9
	 */
	virtual int columnCount(QModelIndex const&  parent = QModelIndex()) const;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in] index Identifies the item that's data is wanted.
	 *      @param [in] role Specifies what kind of data is wanted
	 *
	 *      @return QVariant containing the data
	 */
	virtual QVariant data(QModelIndex const&  index, int role = Qt::DisplayRole) const;

	/*!
     *  Get the data for the headers
	 *
	 *      @param [in] section The column that's header is wanted
	 *      @param [in] orientation Only Qt::Horizontal is supported
	 *      @param [in] role Specified the type of data that is wanted.
	 *
	 *      @return QVariant containing the data to be displayed
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in] index Specifies the item that's data is modified
	 *      @param [in] value The value to be set.
	 *      @param [in] role The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	virtual bool setData(QModelIndex const&  index, const QVariant& value, int role = Qt::EditRole);

	/*!
     *  Get information on how specified item can be handled.
	 *
	 *      @param [in] index Specifies the item that's flags are wanted.
	 *
	 *      @return Qt::ItemFlags that define how object can be handled.
	 */
	virtual Qt::ItemFlags flags(QModelIndex const&  index) const;

public slots:

	/*!
     *  Remove a row from the model
	 *
	 *      @param [in] row Specifies the row to remove
	 */
	void onRemoveRow(int row);

	/*!
     *  Add a new empty row to the model
	 */
	void onAddRow();

	/*!
     *  A new item should be added to given index.
	 *
	 *      @param [in] index The position where new item should be added at.
	 */
	void onAddItem(QModelIndex const&  index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *      @param [in] index Identifies the item that should be removed.
	 */
	void onRemoveItem(QModelIndex const&  index);

signals:

	//! Emitted when contents of the model change.
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:

	//! No copying.
	OtherClockDriversModel(const OtherClockDriversModel& other);

	//! No assignment.
	OtherClockDriversModel& operator=(const OtherClockDriversModel& other);
	
    /*!
     *  Validate the data contained within a given index.
     *
     *      @param [in] index   The index of the data being validated.
     *
     *      @return True, if the data is valid, otherwise false.
     */
    bool validateIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The component being edited.
	QSharedPointer<Component> component_;

	//! Contains the current items in the editor.
	QSharedPointer<QList<QSharedPointer<OtherClockDriver> > > table_;

    //! The validator used for other clock drivers.
    QSharedPointer<OtherClockDriverValidator> clockValidator_;
};

#endif // OTHERCLOCKDRIVERSMODEL_H
