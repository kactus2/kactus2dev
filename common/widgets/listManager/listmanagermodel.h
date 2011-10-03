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
	void remove(QModelIndex& index);

	/*! \brief Moves the specified item up one row in the model.
	*
	* \parma index The model index of the item to move up.
	*/
	void moveUp(QModelIndex& index);

	/*! \brief Moves the specified item down one row in the model.
	*
	* \param index The model index of the item to move down.
	*/
	void moveDown(QModelIndex& index);

private:

	//! No copying
	ListManagerModel(const ListManagerModel& other);

	//! No assignment
	ListManagerModel& operator=(const ListManagerModel& other);

	//! \brief Contains the items to be displayed in a list.
	QStringList items_;
};

#endif // LISTMANAGERMODEL_H
