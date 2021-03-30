//-----------------------------------------------------------------------------
// File: AbstractionTypesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.01.2018
//
// Description:
// Table model for bus interface abstraction types.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONTYPESMODEL_H
#define ABSTRACTIONTYPESMODEL_H

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/VLNV.h>

#include <QAbstractTableModel>
#include <QSharedPointer>

class BusInterface;
class AbstractionType;
class AbstractionTypeValidator;
class AbstractionTypeInterface;

//-----------------------------------------------------------------------------
//! Table model for bus interface abstraction types.
//-----------------------------------------------------------------------------
class AbstractionTypesModel : public QAbstractTableModel
{
    Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *      @param [in] abstractionInterface    Interface for accessing abstraction types.
	 *      @param [in] parent                  Pointer to the owner of this model.
     */
    AbstractionTypesModel(AbstractionTypeInterface* abstractionInterface, QObject* parent);

	/*!
     *  The destructor.
     */
    virtual ~AbstractionTypesModel();

	/*!
     *  Get the number of rows in the model.
	 *
	 *      @param [in] parent  Model index of the parent of the item.
	 *
	 *      @return Number of rows currently in the model.
     */
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the number of columns in the model.
	 *
	 *      @param [in] parent  Model index of the parent of the item.
	 *
	 *      @return Always returns 2.
     */
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

    /*!
     *  Get the data for the headers.
	 *
	 *      @param [in] section         The column that's header is wanted.
	 *      @param [in] orientation     Only Qt::Horizontal is supported.
	 *      @param [in] role            Specified the type of data that is wanted.
	 *
	 *      @return QVariant containing the data to be displayed.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in] index   Identifies the item that's data is wanted.
	 *      @param [in] role    Specifies what kind of data is wanted.
	 *
	 *      @return QVariant containing the data.
     */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is modified.
	 *      @param [in] value   The value to be set.
	 *      @param [in] role    The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	virtual bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);
    
    /*!
     *  Get information on how specified item can be handled.
	 *
	 *      @param [in] index   Specifies the item that's flags are wanted.
	 *
	 *      @return Qt::ItemFlags that define how object can be handled.
     */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Change the currently edited bus interface.
     */
    void onChangeSelectedBusInterface();

    /*!
     *  Returns the supported actions of a drop.
     *
     *      @return The drop actions supported by the model.
     */
    Qt::DropActions supportedDropActions() const;

    /*!
     *  Returns a list of supported MIME data types.
     *
     *      @return The supported MIME types.
     */
    QStringList mimeTypes() const;

    /*!
     *  Handler for the dropped MIME data.
     *
     *      @param [in] data    The data associated to the drop.
     *      @param [in] action  The drop action.  
     *      @param [in] row     The row beneath the drop position.
     *      @param [in] column  The column beneath the drop position.
     *      @param [in] parent  The parent index of the drop position.
     *
     *      @return True, if the model could handle the data, otherwise false.
     */
    bool dropMimeData(QMimeData const* data, Qt::DropAction action, int row, int column,
        QModelIndex const& parent);

    /*!
     *  Add a new abstraction type referencing the selected abstraction definition VLNV.
     *
     *      @param [in] newAbstractionVLNV  The selected abstraction definition VLNV.
     */
    void addNewAbstractionTypeWithVLNV(VLNV const& newAbstractionVLNV);

public slots:

	/*!
     *  Handles the addition of new abstraction types.
	 *
	 *      @param [in] index   Index of the new abstraction type.
     */
    virtual void onAddItem(QModelIndex const& index);

	/*!
     *  Handles the removal of abstraction types.
	 * 
	 *      @param [in] index   Identifies the abstraction type that should be removed.
     */
    virtual void onRemoveItem(QModelIndex const& index);

signals:

	/*!
     *  Emitted when contents of the model change
     */
	void contentChanged();

	/*!
     *  Prints an error message to the user.
     */
	void errorMessage(const QString& msg) const;
	
	/*!
     *  Prints a notification to user.
     */
	void noticeMessage(const QString& msg) const;

private:
	
	//! No copying. No assignment.
    AbstractionTypesModel(const AbstractionTypesModel& other);
    AbstractionTypesModel& operator=(const AbstractionTypesModel& other);

    /*!
     *  Get the color for the indexed abstraction type.
     *
     *      @param [in] index           The selected index.
     *
     *      @return Black for valid index, red for invalid index.
     */
    QVariant blackForValidRedForInvalid(QModelIndex const& index) const;

    /*!
     *  Validate the indexed abstraction type.
     *
     *      @param [in] index           The selected index.
     *
     *      @return True, if the indexed abstraction type is valid, false otherwise.
     */
    bool validateIndex(QModelIndex const& index) const;

    /*!
     *  Set a new abstraction reference to the selected abstraction type.
     *
     *      @param [in] abstractionIndex    Index of the selected abstraction type.
     *      @param [in] newVLNV             VLNV of the new abstraction reference.
     */
    void setVLNVForAbstraction(int const& abstractionIndex, VLNV const& newVLNV);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing abstraction types.
    AbstractionTypeInterface* abstractionInterface_;
};

#endif // ABSTRACTIONTYPESMODEL_H
