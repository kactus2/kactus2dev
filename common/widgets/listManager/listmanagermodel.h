/* 
 *
 *  Created on: 28.1.2011
 *      Author: Antti Kamppi
 * 		filename: listmanagermodel.h
 */

#ifndef LISTMANAGERMODEL_H
#define LISTMANAGERMODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVariant>
#include <QStringList>
#include <QModelIndex>

/*! \brief ListManagerModel is a model class to manage a list of strings.
 *
 * This class can be used as a model class for a list view to implement a 
 * widget to manage a list of strings.
 */
class ListManagerModel : public QAbstractListModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	*
	* \param parent Pointer to the owner of this model.
	* \param items QStringList that contains the items to add.
	*/
	ListManagerModel(QObject *parent = 0, 
		const QStringList& items = QStringList());

	//! \brief The destructor
	virtual ~ListManagerModel();

	/*! \brief Get the number of rows in this model.
	*
	* \param parent ModelIndex of the item that's rowCount is requested.
	*/
	int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the data stored for the specified item.
	*
	* \param index ModelIndex of the wanted item.
	* \param role Specifies what kind of data is requested.
	*/
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Get the data for the header of the list.
	*
	* \param section Specifies the column for which the header is requested.
	* \param orientation The orientation of the header, only Qt::Horizontal is
	* supported.
	* \param role Specifies what kind of header data is requested.
	*/
	QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

	/*! \brief Save the data to the model for specified item
	 *
	 * \param index The model index of the item that's data is to be saved.
	 * \param value The data that is to be saved.
	 * \param role The role specifies what kind of data should be saved.
	 *
	 * \return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole);

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Append a new item to the end of the list.
	 *
	 * \param item The string to be appended to the end of the list.
	 */
	void appendItem(const QString item);

	/*! \brief Get the items currently stored in the model.
	 *
	 * \return QStringList containing the items.
	 */
	const QStringList& items() const;

	/*! \brief Set the items to the model.
	 *
	 * \param items QStringList containing the items to be contained in the
	 * model.
	 */
	void setItems(const QStringList& items);

	/*! \brief Append a QStringList to the current items in the list
	 *
	 * \param items QStringList containing the items to append
	 *
	*/
	void appendItems(const QStringList& items);

	/*! \brief Replace an item text in the list
	 * 
	 * \param index ModelIndex of the item that is to be replaced.
	 * \param newText The new text for the given item.
	 */
	void replace(QModelIndex& index, const QString newText);

public slots:
	 
	/*! \brief Removes the specified item from the model.
	*
	* \param index The model index of the item to remove.
	*/
	void remove(const QModelIndex& index);

	/*! \brief A new item should be added to given index.
	 *
	 * \param index The position where new item should be added at.
	 *
	*/
	void addItem(const QModelIndex& index);

	/*! \brief Move item to another position.
	 *
	 * \param originalPos Identifies the item that should be moved.
	 * \param newPos The new position the item should be moved to.
	 *
	*/
	void moveItem(const QModelIndex& originalPos, const QModelIndex& newPos);

signals: 

	//! \brief Emitted when contents of the model change.
	void contentChanged();

private:

	//! No copying
	ListManagerModel(const ListManagerModel& other);

	//! No assignment
	ListManagerModel& operator=(const ListManagerModel& other);

	//! \brief Contains the items to be displayed in a list.
	QStringList items_;
};

#endif // LISTMANAGERMODEL_H
