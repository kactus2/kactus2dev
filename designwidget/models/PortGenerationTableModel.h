//-----------------------------------------------------------------------------
// File: PortGenerationTableModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 31.5.2013
//
// Description:
// Table model for draft port generation.
//-----------------------------------------------------------------------------

#ifndef PORTGENERATIONTABLEMODEL_H
#define PORTGENERATIONTABLEMODEL_H

#include <QList>
#include <QAbstractTableModel>

#include "models/generaldeclarations.h"

class BusInterface;
class Component;
class LibraryInterface;
class PortGenerationRow;
class ConnectionEndpoint;
//-----------------------------------------------------------------------------
//! PortGenerationTableModel class.
//-----------------------------------------------------------------------------
class PortGenerationTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:

    //! \brief Column numbering.
    enum
    {
        SRC_DESCRIPTION = 0,
        SRC_DIRECTION,
        SRC_NAME,
        SIZE,
        DRAFT_NAME,
        DRAFT_DIRECTION,
        DRAFT_DESCRIPTION
    };

    //! \brief The constructor.
    PortGenerationTableModel(QObject* parent = 0);

    //! \brief The destructor.
    virtual ~PortGenerationTableModel();
	
	/*! \brief Get the number of rows in the model.
	 *
	 * @param [in] parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * @return Number of rows currently in the model.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the number of columns in the model
	 *
	 * @param [in] parent Model index of the parent of the item. Must be invalid
	 * because this is not hierarchical model.
	 *
	 * \return Number of columns currently in the model.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! \brief Get the data for the specified item for specified role.
	 *
	 * @param [in] index Identifies the item that's data is wanted.
	 * @param [in] role Specifies what kind of data is wanted.
	 *
	 * @return QVariant containing the data.
	*/
	virtual QVariant data(const QModelIndex& index, 
		int role = Qt::DisplayRole ) const;

	/*! \brief Get the data for the headers
	 *
	 * @param [in] section The column that's header is wanted.
	 * @param [in] orientation Only Qt::Horizontal is supported.
	 * @param [in] role Specified the type of data that is wanted.
	 *
	 * @return QVariant containing the data to be displayed.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole ) const;

	/*! \brief Set the data for specified item.
	 *
	 * @param [in] index Specifies the item that's data is modified
	 * @param [in] value The value to be set.
	 * @param [in] role The role that is trying to modify the data. Only Qt::EditRole
	 * is supported.
	 *
	 * @return True if data was successfully set in non-locked column.
	*/
	virtual bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole );

	/*! \brief Set the header data for specified section.
	 *
	 * @param [in] section Specifies the item that's data is modified
	 * @param [in] orientation The header's orientation. Only Qt::Horizontal is supported.
	 * @param [in] value The value to be set.
	 * @param [in] role The role that is trying to modify the data. Only Qt::EditRole
	 * is supported.
	 *
	 * @return True if the header was successfully set.
	*/
    bool setHeaderData( int section, Qt::Orientation orientation, 
        const QVariant & value, int role = Qt::EditRole );

	/*! \brief Get information on how specified item can be handled.
	 *
	 * @param [in] index Specifies the item that's flags are wanted.
	 *
	 * @return Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Check if the model is in valid state or not.
	 *
	 * @return True if all items in model are valid.
	*/
	bool isValid() const;

	/*! \brief Creates the initial content according to specified interfaces.
     *
     * @param [in] lh                   The library interface.
     * @param [in] sourceComponent      Opposing bus interface item.
     * @param [in] draftComponent       Bus interface item in draft component.
     * @param [in] selectedMode         Bus interface mode for draft component.
	*/
    void initialize(LibraryInterface* lh, ConnectionEndpoint const* sourceComponent, 
        ConnectionEndpoint const* draftComponent,
        General::InterfaceMode selectedMode);

	//! \brief Locks a column.
	void lockColumn(int const column);

    //! \brief Opens a column lock.	
	void openColumn(int const column);

    /*! \brief Checks if a column is locked.
	 *
	 * @return True if column was locked.
	*/
	bool columnLocked(int column) const;

private:
    //! \brief No copying.
	PortGenerationTableModel(const PortGenerationTableModel& other);

	//! No assignment.
	PortGenerationTableModel& operator=(const PortGenerationTableModel& other);

    /*! \brief Checks if table or draft component has same name for anohter port.
	 *
     * @param [in] row The row whose generated name is compared to others.
     *  
	 * @return True if a port with same name was found.
	*/
    bool nameDuplicates(int const row) const;

    /*! \brief Generates a unique name for a port.
	 *
     * @param [in] name                 The port name in the opposite interface.
     * @param [in] opposingDirection    The port direction  in the opposite interface.
     * @param [in] draftDirection       The port direction in the draft interface.
     * @param [in] delimiter            Delimiter for in/out/inout in port name.

	 * @return Unique name for a port in the draft component.
	*/
    QString generateName(QString name, General::Direction opposingDirection, 
        General::Direction draftDirection, QString delimiter = "_");

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! \brief Pointer to the draft component.
    QSharedPointer<Component> draftComponent_;

    //! \brief Columns whose content cannot be changed by the user.
    QList<int> lockedColumns_;

    //! \brief Table rows.
    QList< QSharedPointer<PortGenerationRow> > rows_;

    //! \brief Table headers.
    QStringList headers_;
};

#endif // PORTGENERATIONTABLEMODEL_H
