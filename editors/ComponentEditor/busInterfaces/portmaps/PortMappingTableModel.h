//-----------------------------------------------------------------------------
// File: PortMappingTableModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.05.2016
//
// Description:
// The base class for logical table model and physical table model.
//-----------------------------------------------------------------------------

#ifndef PORTMAPPINGTABLEMODEL_H
#define PORTMAPPINGTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

//-----------------------------------------------------------------------------
//! The base class for logical table model and physical table model.
//-----------------------------------------------------------------------------
class PortMappingTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *      @param [in] parent  The owner of this model
	 */
	PortMappingTableModel(QObject *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~PortMappingTableModel();

    /*!
     *  Get the number of columns in the model
	 *
	 *      @param [in] parent      Model index of the parent of the item. Must be invalid because this is not
     *                              a hierarchical model.
	 *
	 *      @return Always returns 3
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

    /*!
     *  Get the data for the headers
	 *
	 *      @param [in]  section        The column that's header is wanted.
	 *      @param [in]  orientation    The orientation of the header data.
	 *      @param [in]  role           Specified the type of data that is wanted.
	 *
	 *      @return  The header data for the given section.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
    
	/*! Get the data for the specified item for specified role.
	 *
	 *      @param [in] index Identifies the item that's data is wanted.
	 *      @param [in] role Specifies what kind of data is wanted
	 *
	 *      @return The requested data.
	*/
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const = 0;

    /*!
     *  Get information on how specified item can be handled.
	 *
	 *      @param [in]  index Specifies the item that's flags are wanted.
	 *
	 *      @return  Qt::ItemFlags that define how object can be handled.
	 */
	virtual Qt::ItemFlags flags(QModelIndex const& index) const;

private:

	//! No copying.
	PortMappingTableModel(const PortMappingTableModel& other);

	//! No assignment.
	PortMappingTableModel& operator=(const PortMappingTableModel& other);
};

#endif // PORTMAPPINGTABLEMODEL_H
