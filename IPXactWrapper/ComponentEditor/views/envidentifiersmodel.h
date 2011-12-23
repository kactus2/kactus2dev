/* 
 *
 *  Created on: 18.4.2011
 *      Author: Antti Kamppi
 * 		filename: envidentifiersmodel.h
 */

#ifndef ENVIDENTIFIERSMODEL_H
#define ENVIDENTIFIERSMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class View;

/*! \brief Model that contains the environment identifiers to be displayed to user.
 *
 */
class EnvIdentifiersModel : public QAbstractTableModel {
	Q_OBJECT

public:
	
	/*! \brief The constructor
	 *
	 * \param view Pointer to the view being edited.
	 * \param parent Pointer to the owner of this model
	 *
	*/
	EnvIdentifiersModel(View* view,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~EnvIdentifiersModel();

	/*! \brief Get the number of rows in the model.
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Number of rows currently in the model.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the number of columns in the model
	 *
	 * \param parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Always returns 3
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the data for the specified item for specified role.
	 *
	 * \param index Identifies the item that's data is wanted.
	 * \param role Specifies what kind of data is wanted
	 *
	 * \return QVariant containing the data
	*/
	virtual QVariant data(const QModelIndex& index, 
		int role = Qt::DisplayRole ) const;

	/*! \brief Get the data for the headers
	 *
	 * \param section The column that's header is wanted
	 * \param orientation Only Qt::Horizontal is supported
	 * \param role Specified the type of data that is wanted.
	 *
	 * \return QVariant containing the data to be displayed
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole ) const;

	/*! \brief Set the data for specified item.
	 *
	 * \param index Specifies the item that's data is modified
	 * \param value The value to be set.
	 * \param role The role that is trying to modify the data. Only Qt::EditRole
	 * is supported.
	 *
	 * \return True if data was successfully set.
	*/
	virtual bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole );

	/*! \brief Get information on how specified item can be handled.
	 *
	 * \param index Specifies the item that's flags are wanted.
	 *
	 * \return Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Check if the model is in valid state or not.
	 *
	 * \return True if all items in model are valid.
	*/
	bool isValid() const;

	/*! \brief Write the changes to the original model.
	 *
	*/
	void apply();

	/*! \brief Restore the settings from the original model.
	 *
	*/
	void restore();

public slots:

	/*! \brief Remove a row from the model
	 *
	 * \param row Specifies the row to remove
	*/
	void onRemoveRow(int row);

	/*! \brief Add a new empty row to the model
	 *
	*/
	void onAddRow();

	/*! \brief A new item should be added to given index.
	 *
	 * \param index The position where new item should be added at.
	 *
	*/
	void onAddItem(const QModelIndex& index);

	/*! \brief An item should be removed from the model.
	 * 
	 * \param index Identifies the item that should be removed.
	 *
	*/
	void onRemoveItem(const QModelIndex& index);

signals:

	//! \brief Emitted when contents of the model change
	void contentChanged();

	//! \brief Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! \brief Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:
	
	//! \brief No copying
	EnvIdentifiersModel(const EnvIdentifiersModel& other);

	//! No assignment
	EnvIdentifiersModel& operator=(const EnvIdentifiersModel& other);

	//! \brief Pointer to the view being edited.
	View* view_;

	//! \brief Contains the environment identifiers being edited.
	QStringList table_;
};

#endif // ENVIDENTIFIERSMODEL_H
