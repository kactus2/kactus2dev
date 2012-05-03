/* 
 *  	Created on: 3.5.2012
 *      Author: Antti Kamppi
 * 		filename: fieldtablemodel.h
 *		Project: Kactus 2
 */

#ifndef FIELDTABLEMODEL_H
#define FIELDTABLEMODEL_H

#include <QAbstractTableModel>

/*! \brief The model to control the fields of a register.
 *
 */
class FieldTableModel : public QAbstractTableModel {
	Q_OBJECT

public:

	FieldTableModel(QObject *parent);
	
	//! \brief The destructor
	virtual ~FieldTableModel();

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

private:

	//! \brief No copying
	FieldTableModel(const FieldTableModel& other);

	//! \brief No assignment
	FieldTableModel& operator=(const FieldTableModel& other);
};

#endif // FIELDTABLEMODEL_H
