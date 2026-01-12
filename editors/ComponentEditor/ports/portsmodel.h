//-----------------------------------------------------------------------------
// File: portsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 31.3.2011
//
// Description:
// Table model that can be used to display ports to be edited.
//-----------------------------------------------------------------------------

#ifndef PORTSMODEL_H
#define PORTSMODEL_H

#include <QSharedPointer>
#include <QString>
#include <QList>

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/DirectionTypes.h>

#include <QSortFilterProxyModel>

class AbstractionDefinition;
class PortAbstractionInterface;
class PortsInterface;

//-----------------------------------------------------------------------------
//! Table model that can be used to display ports to be edited.
//-----------------------------------------------------------------------------
class PortsModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *    @param [in] parameterFinder     Pointer to the parameter finder.
     *    @param [in] portInterface       Interface for accessing the component ports.
     *    @param [in] signalInterface     Interface for accessing abstraction signals.
	 *    @param [in] parent              Pointer to the owner of this model.
     */
    PortsModel(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<PortsInterface> portInterface,
        QSharedPointer<PortAbstractionInterface> signalInterface,
        QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~PortsModel() = default;

	/*!
     *  Get the number of rows in the model.
	 *
	 *    @param [in] parent      Model index of the parent of the item. Must be invalid.
	 *
	 *    @return Number of rows currently in the model.
     */
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *    @param [in] index   Identifies the item that's data is wanted.
	 *    @param [in] role    Specifies what kind of data is wanted
	 *
	 *    @return QVariant containing the data
     */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;

	/*!
     *  Get the data for the headers.
	 *
	 *    @param [in] section         The column that's header is wanted
	 *    @param [in] orientation     Only Qt::Horizontal is supported
	 *    @param [in] role            Specified the type of data that is wanted.
	 *
	 *    @return QVariant containing the data to be displayed.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*!
     *  Set the data for specified item.
	 *
	 *    @param [in] index   Specifies the item that's data is modified
	 *    @param [in] value   The value to be set.
	 *    @param [in] role    The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *    @return True if data was successfully set.
	 */
	virtual bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);

	/*!
     *  Get information on how specified item can be handled.
	 *
	 *    @param [in]     index Specifies the item that's flags are wanted.
	 *
	 *    @return Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
     *  Check if the model is in valid state or not.
	 *
	 *    @return True if all items in model are valid.
     */
	bool isValid() const;

	/*!
     *  Get the index of the identified port.
	 *
	 *    @param [in] portName    Name of the identified port.
	 *
	 *    @return QModelIndex of first column of the specified port. Invalid index if named port is not found.
     */
    virtual QModelIndex index(QString const& portName) const;

    /*!
     *  Reset the model.
     */
    void resetModel();

    /*!
     *  Sets the edited model and locks all current ports.
     */
    void resetModelAndLockCurrentPorts();

    /*!
     *  Handle port creation from abstraction definition signals.
     *
     *    @param [in] abstraction     The selected abstraction definition.
     *    @param [in] busMode         Interface mode of the selected bus interface.
     *    @param [in] busMonitorMode  Monitored bus interface of the selected bus interface.
     *    @param [in] systemGroup     System group of the selected bus interface.
     */
    void onCreatePortsFromAbstraction(QSharedPointer<const AbstractionDefinition> abstraction,
        General::InterfaceMode const& busMode,
        General::InterfaceMode const& busMonitorMode,
        QString const& systemGroup);

    //! No copying. No assignment.
    PortsModel(const PortsModel& other) = delete;
    PortsModel& operator=(const PortsModel& other) = delete;

protected:

    /*!
     *  Get the used port interface.
     *
     *    @return The port interface.
     */
    QSharedPointer<PortsInterface> getInterface() const;

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *    @param [in] index   The index being evaluated.
     *
     *      return      True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const override = 0;

    virtual bool isPortTypeColumn(QModelIndex const& index) const = 0;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *    @param [in] index   The index of target data.
     *
     *      return      Expression in the given index, or plain value.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const override;

    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *    @param [in] index   The selected index.
     *
     *    @return The formatted value of an expression in the selected index.
     */
    virtual QVariant formattedExpressionForIndex(QModelIndex const& index) const = 0;

    /*!
     *  Get the expression of the selected index.
     *
     *    @param [in] index   The selected index.
     *
     *    @return The expression of the selected index.
     */
    virtual QVariant expressionForIndex(QModelIndex const& index) const;

    /*!
     *  Gets the value for the given index.
     *
     *    @param [in] index   The index of target data.
     *
     *      return      The data in the given index.
     */
    virtual QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in the cell given by the column.
     *
     *    @param [in] index   The index being validated.
     *
     *      return      True, if the data in the parameter is valid, false otherwise.
     */
    virtual bool validateIndex(QModelIndex const& index) const override;

    /*!
     *  Get all the references made to the selected parameter from the selected row.
     *
     *    @param [in] row         The selected row.
     *    @param [in] valueID     The target parameter.
     *
     *    @return Number of references made to the selected id from the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const&  valueID) const override;

    /*!
     *   Checks if given index is locked.
     *
     *    @param [in] index   The index to check.
     *
     *    @return True if the index is locked, otherwise false.
     */
    bool isLocked(QModelIndex const& index) const;

    /*!
     *  Get the icon path of the selected index.
     *
     *    @param [in] portIndex   The selected index.
     *
     *    @return The icon path for the selected index.
     */
    QString getIconPath(int const& portIndex) const;

public slots:

	/*!
     *  Remove a row from the model.
	 *
	 *    @param [in] row Specifies the row to remove
     */
	void onRemoveRow(int row);
    

	/*!
     *  Add a new empty row to the model
     */
	void onAddRow();

	/*!
     *  A new item should be added to given index.
	 *
	 *    @param [in] index   The position where new item should be added at.
     */
	void onAddItem(const QModelIndex& index);

	/*!
     *  An item should be removed from the model.
	 * 
	 *    @param [in] index   Identifies the item that should be removed.
     */
	void onRemoveItem(const QModelIndex& index);

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

    /*!
     *  Change the vendor extensions editor data.
     *
     *    @param [in] index   Index of the selected port.
     */
    void portExtensionDataChanged(QModelIndex const& index);

    /*!
     *  Invalidate the filter in the other ports model.
     */
    void invalidateOtherFilter();

    /*!
     *  Inform of a change in the amount of editable ports.
     */
    void portCountChanged();

private:

    /*!
     *  Get the column for row number.
     *
     *    @return Row number column.
     */
    virtual int rowNumberColumn() const = 0;

    /*!
     *  Get the column for name.
     *
     *    @return Name column.
     */
    virtual int nameColumn() const = 0;

    /*!
     *  Get the column for type definitions.
     *
     *    @return Type definitions column.
     */
    virtual int typeColumn() const = 0;

    /*!
     *  Get the column for array left.
     *
     *    @return Array left column.
     */
    virtual int arrayLeftColumn() const = 0;

    /*!
     *  Get the column for array right.
     *
     *    @return Array right column.
     */
    virtual int arrayRightColum() const = 0;

    /*!
     *  Get the column for tags.
     *
     *    @return Tags column.
     */
    virtual int tagColumn() const = 0;

    /*!
     *  Get the column for ad hoc visibility.
     *
     *    @return Ad hoc visibility column.
     */
    virtual int adHocColumn() const = 0;

    /*!
     *  Get the column for description.
     *
     *    @return Description column.
     */
    virtual int descriptionColumn() const = 0;

    /*!
     *  Check if the selected item is disabled.
     *
     *    @param [in] index           Index of the selected item.
     *
     *    @return True, if the indexed item is disabled, false otherwise.
     */
    virtual bool indexedItemIsDisabled(QModelIndex const& index) const = 0;

    /*!
     *  Check if the selected item is mandatory.
     *
     *    @param [in] index   Index of the selected item.
     *
     *    @return True, if the indexed item is mandatory, false otherwise.
     */
    virtual bool indexedItemIsMandatory(QModelIndex const& index) const = 0;

    /*!
     *  Check if the selected item is locked.
     *
     *    @param [in] index   Index of the selected item.
     *
     *    @return True, if the indexed item is locked, false otherwise.
     */
    virtual bool indexedItemIsLocked(QModelIndex const& index) const = 0;

    /*!
     *  Get a list of the locked port indexes from the selected index.
     *
     *    @param [in] portIndex   Index of the selected item.
     *
     *    @return List of the locked port indexes.
     */
    virtual QModelIndexList getLockedPortIndexes(QModelIndex const& portIndex) const = 0;

    /*!
     *  Add a new wire or transactional port.
     *
     *    @param [in] port    The selected port.
     */
    virtual void addNewPort() = 0;

    /*!
     *   Locks the name, direction  and type columns of a port.
     *
     *    @param [in] portName    Name of the selected port.
     */
    void lockPort(QString const& portName);

    /*!
     *   Unlocks the name, direction and type columns of a port.
     *
     *    @param [in] portName    Name of the selected port.
     */
    void unlockPort(QString const& portName);

    /*!
     *   Locks the given index disabling editing.
     *
     *    @param [in] index   The index to lock.
     */
    void lockIndex(QModelIndex const& index);
    
    /*!
     *   Unlocks the given index disabling editing.
     *
     *    @param [in] index   The index to unlock.
     */
    void unlockIndex(QModelIndex const& index);

    /*!
     *   Checks if given row is locked.
     *
     *    @param [in] row   The row to check.
	 *
	 *    @return True if the row is locked, otherwise false.
     */
    bool rowIsLocked(int row) const;

    /*!
     *  Check if the selected interface modes match.
     *
     *    @param [in] busMode             Interface mode of the selected bus interface.
     *    @param [in] busSystemGroup      System group of the selected bus interface.
     *    @param [in] signalMode          Interface mode of the selected signal.
     *    @param [in] signalSystemGroup   System group of the selected signal.
     *
     *    @return True, if the interface modes match, false otherwise.
     */
    bool modesMatch(General::InterfaceMode const& busMode, QString const& busSystemGroup,
        General::InterfaceMode const& signalMode, QString const& signalSystemGroup) const;

    /*!
     *  Get the direction for a port created from a signal.
     *
     *    @param [in] busMode             Interface mode of the selected bus interface.
     *    @param [in] signalDirection     Direction of the selected signal.
     *
     *    @return Direction for the port created from the selected signal.
     */
    DirectionTypes::Direction getDirectionFromSignal(General::InterfaceMode const& busMode,
        DirectionTypes::Direction const& signalDirection) const;

    /*!
     *  Get the initiative for a port created from a signal.
     *
     *    @param [in] busMode             Interface mode of the selected bus interface.
     *    @param [in] signalInitiative    Initiative of the selected signal.
     *
     *    @return Initiative for the port created from the selected signal.
     */
    QString getInitiativeFromSignal(General::InterfaceMode const& busMode, QString const& signalInitiative) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing the component ports.
    QSharedPointer<PortsInterface> portsInterface_;

    //! Interface for accessing abstraction signals.
    QSharedPointer<PortAbstractionInterface> signalInterface_;

    //! The locked indexes that cannot be edited.
    QList<QPersistentModelIndex> lockedIndexes_;
};

#endif // PORTSMODEL_H
