/* 
 *  	Created on: 8.8.2011
 *      Author: Antti Kamppi
 * 		filename: objectremovemodel.h
 *		Project: Kactus 2
 */

#ifndef OBJECTREMOVEMODEL_H
#define OBJECTREMOVEMODEL_H

#include <LibraryManager/vlnv.h>

#include <QAbstractTableModel>
#include <QString>
#include <QList>

/*! \brief Model class to select/deselect objects to be removed from library and file system.
 *
 */
class ObjectRemoveModel : public QAbstractTableModel {
	Q_OBJECT

public:

	//! \brief Identifies the type of the item in the model.
	enum Type {
		VLNVOBJECT = 0,	// item is vlnv object
		FILE			// item is a file in file system
	};

	//! \brief A single object that can be removed
	struct Item {

		//! \brief Identifies the type of the object.
		Type type_;

		//! \brief vlnv of the object if type is VLNVOBJECT
		VLNV vlnv_;

		//! \brief Path to the file is type is FILE
		QString path_;

		//! \brief If true then this item is checked and should be removed.
		bool checked_;

	//! \brief If true then this item can not be unchecked.
		bool locked_;

		/*! \brief The constructor for VLNVOBJECT
		 *
		 * \param vlnv the vlnv for the item.
		 *
		*/
		Item(const VLNV& vlnv);

		/*! \brief The constructor for FILE
		 *
		 * \param path Absolute file path to the file.
		 *
		*/
		Item(const QString& path);

		/*! \brief Operator == for this struct
		 *
		 * \param other Reference to the item to compare to.
		 *
		 * \return bool True if vlnv or file path matches.
		*/
		bool operator==(const Item& other) const;
	};

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	ObjectRemoveModel(QObject *parent);
	
	//! \brief The destructor
	virtual ~ObjectRemoveModel();

	/*! \brief Get the number of rows this model contains.
	 *
	 * \param parent Must always be invalid index because this is not hierarchical model.
	 *
	 * \return int The number of rows.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns this model contains.
	 *
	 * \param parent Must always be invalid because this is not hierarhical model.
	 *
	 * \return int The number of columns, always 2 for invalid parents.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the data for given model index.
	 *
	 * \param index Identifies the item that's data is wanted.
	 * \param role Specifies what kind of data is wanted.
	 *
	 * \return QVariant contains the data.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Get the data for given header section.
	 *
	 * \param section Identifies the header section.
	 * \param orientation Only Qt::Horizontal is supported.
	 * \param role Specifies the type of data that is wanted.
	 *
	 * \return QVariant contains the data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

	/*! \brief Get info on how the item can be handled.
	 *
	 * \param index Identifies the item.
	 * 
	 * \return Qt::ItemFlags Specify how the item can be handled.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Save the data on specified item.
	 *
	 * \param index Identifies the item to save the data to.
	 * \param value Contains the data to save.
	 * \param role Identifies the role of the data to save.
	 *
	 * \return bool True if data was saved successfully.
	*/
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	/*! \brief Add a new item to the table.
	 *
	 * \param item Reference to the item to add.
	 *
	*/
	void addItem(const Item& item);

	/*! \brief Create new item for given vlnv object.
	 *
	 * \param vlnv Reference to the vlnv item is created for.
	 *
	*/
	void createItem(const VLNV& vlnv, bool locked = false);

	/*! \brief Create new item for given file
	 *
	 * \param filePath Path to the file 
	 *
	*/
	void createItem(const QString& filePath);

	/*! \brief Get list of items that should be removed.
	 *
	 * \return QList containing the items to remove.
	*/
	QList<Item> getItemsToRemove() const;

public slots:

	//! \brief Handler for selection check box state changes.
	void onSelectionChange(int selectionState);

private:
	//! \brief No copying
	ObjectRemoveModel(const ObjectRemoveModel& other);

	//! \brief No assignment
	ObjectRemoveModel& operator=(const ObjectRemoveModel& other);

	//! \brief Contains the items for the model.
	QList<Item> items_;
};

#endif // OBJECTREMOVEMODEL_H
