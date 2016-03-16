//-----------------------------------------------------------------------------
// File: envidentifiereditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 18.04.2011
//
// Description:
// Model that contains the environment identifiers to be displayed to user.
//-----------------------------------------------------------------------------

#ifndef ENVIDENTIFIERSMODEL_H
#define ENVIDENTIFIERSMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QSharedPointer>

class View;

//-----------------------------------------------------------------------------
//! Model that contains the environment identifiers to be displayed to user.
//-----------------------------------------------------------------------------
class EnvIdentifiersModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	
	/*!
     *  The constructor.
	 *
	 *      @param [in] view    The view being edited.
	 *      @param [in] parent  The owner of this model
	 */
	EnvIdentifiersModel(QSharedPointer<View> view, QObject *parent);
	
	//! The destructor.
	virtual ~EnvIdentifiersModel();

	/*!
     *  Get the number of rows in the model.
	 *
	 *      @param [in] parent  Model index of the parent of the item.
     *
	 *      @return Number of rows currently in the model.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the number of columns in the model.
	 *
	 *      @param [in] parent  Model index of the parent of the item.
	 *
	 *      @return Always returns 3.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in] index   Identifies the item that's data is wanted.
	 *      @param [in] role    Specifies what kind of data is wanted
	 *
	 *      @return QVariant containing the data
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;

	/*!
     *  Get the data for the headers.
	 *
	 *      @param [in] section         The column that's header is wanted
	 *      @param [in] orientation     Only Qt::Horizontal is supported
	 *      @param [in] role            Specified the type of data that is wanted.
	 *
	 *      @return QVariant containing the data to be displayed
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is modified
	 *      @param [in] value   The value to be set.
	 *      @param [in] role    The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole );

	/*!
     *  Get information on how specified item can be handled.
	 *
	 *      @param [in] index   Specifies the item that's flags are wanted.
	 *
	 *      @return Qt::ItemFlags that define how object can be handled.
	 */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
     *  Check if the model is in valid state or not.
	 *
	 *      @return True if all items in model are valid.
	 */
	bool isValid() const;

public slots:

	/*!
     *  A new item should be added to given index.
	 *
	 *      @param [in] index   The position where new item should be added at.
	 */
	void onAddItem(const QModelIndex& index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *      @param [in] index   Identifies the item that should be removed.
	 */
	void onRemoveItem(const QModelIndex& index);

signals:

	//! Emitted when contents of the model change.
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:
	
	//! No copying.
	EnvIdentifiersModel(const EnvIdentifiersModel& other);

	//! No assignment.
	EnvIdentifiersModel& operator=(const EnvIdentifiersModel& other);

	//! The view being edited.
	QSharedPointer<View> view_;

	//! Contains the environment identifiers being edited.
    QStringList table_;
};

#endif // ENVIDENTIFIERSMODEL_H
