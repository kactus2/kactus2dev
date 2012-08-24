/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapmodel.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPMODEL_H
#define MEMORYMAPMODEL_H

#include <models/memorymap.h>
#include <models/memorymapitem.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

/*! \brief The model to manage the details of a single memory map.
 *
 */
class MemoryMapModel : public QAbstractTableModel {
	Q_OBJECT

public:
	
	/*! \brief The constructor
	 *
	 * \param memoryMap Pointer to the memory map being edited.
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	MemoryMapModel(QSharedPointer<MemoryMap> memoryMap,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~MemoryMapModel();

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Get the header data for specified header.
	 *
	 * \param section The section specifies the row/column number for the header.
	 * \param orientation Specified if horizontal or vertical header is wanted.
	 * \param role Specifies the type of the requested data.
	 *
	 * \return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

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

	/*! \brief Check if the memory map model is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

public slots:

	/*! \brief Add a new item to the given index.
	 *
	 * \param index The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! \brief Remove the item in the given index.
	 *
	 * \param index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(const QModelIndex& index);

signals:

	//! \brief Emitted when the contents of the model change.
	void contentChanged();

	//! \brief Emitted when a new memory map item is added to the given index.
	void itemAdded(int index);

	//! \brief Emitted when a memory map item is removed from the given index.
	void itemRemoved(int index);

private:
	
	//! \brief No copying
	MemoryMapModel(const MemoryMapModel& other);

	//! \brief No assignment
	MemoryMapModel& operator=(const MemoryMapModel& other);

	//! \brief Pointer to the memory map being edited.
	QSharedPointer<MemoryMap> memoryMap_;

	//! \brief Contains the memory map items being edited.
	QList<QSharedPointer<MemoryMapItem> >& items_;
};

#endif // MEMORYMAPMODEL_H
