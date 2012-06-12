/* 
 *  	Created on: 22.2.2012
 *      Author: Antti Kamppi
 * 		filename: segmentsmodel.h
 *		Project: Kactus 2
 */

#ifndef SEGMENTSMODEL_H
#define SEGMENTSMODEL_H

#include <models/addressspace.h>
#include <models/segment.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

/*! \brief The model that can be used to display the segments to be edited.
 *
 */
class SegmentsModel : public QAbstractTableModel {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param addrSpace Pointer to the address space being edited.
	 * \param parent Pointer to the owner of this model.
	 *
	*/
	SegmentsModel(QSharedPointer<AddressSpace> addrSpace,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~SegmentsModel();

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
	 * \return The number of columns to be displayed (always 4).
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

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

	/*! \brief Check if the model is in valid state or not.
	 *
	 * \return True if all items in model are valid.
	*/
	bool isValid() const;

public slots:

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

	//! \brief Emitted when a new segment is added to the address space.
	void segmentAdded(QSharedPointer<Segment> segment);

	//! \brief Emitted when a segment is removed from the address space.
	void segmentRemoved(const QString& segmentName);

	//! \brief Emitted when a segment is renamed.
	void segmentRenamed(const QString& oldName, const QString& newName);

	//! \brief Emitted when the range or offset of a segment has changed.
	void segmentChanged(QSharedPointer<Segment> segment);

private:
	//! \brief No copying
	SegmentsModel(const SegmentsModel& other);

	//! \brief No assignment
	SegmentsModel& operator=(const SegmentsModel& other);

	//! \brief Pointer to the data structure that contains the real segments.
	QList<QSharedPointer<Segment> >& segments_;
};

#endif // SEGMENTSMODEL_H
