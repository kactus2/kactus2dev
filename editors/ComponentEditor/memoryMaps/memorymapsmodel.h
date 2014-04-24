/* 
 *  	Created on: 21.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapsmodel.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPSMODEL_H
#define MEMORYMAPSMODEL_H

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/memorymap.h>

#include <QAbstractTableModel>
#include <QSharedPointer>

/*! \brief The model to manage the memory maps summary.
 *
 */
class MemoryMapsModel : public QAbstractTableModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component that contains the memory maps to edit.
	 * \param parent Pointer to the owner of the model.
	 *
	*/
	MemoryMapsModel(QSharedPointer<Component> component,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~MemoryMapsModel();

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

	/*! \brief Check if the MemoryMapsModel is in a valid state.
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

	//! \brief Emitted when a new memory map is added to the given index.
	void memoryMapAdded(int index);

	//! \brief Emitted when a memory map is removed from the given index.
	void memoryMapRemoved(int index);

private:
	
	//! \brief No copying
	MemoryMapsModel(const MemoryMapsModel& other);

	//! \brief No assignment
	MemoryMapsModel& operator=(const MemoryMapsModel& other);

	//! \brief Pointer to the component that contains the memory maps to edit.
	QSharedPointer<Component> component_;

	//! \brief Contains the memory maps to show in the summary.
	QList<QSharedPointer<MemoryMap> >& memMaps_;
};

#endif // MEMORYMAPSMODEL_H
