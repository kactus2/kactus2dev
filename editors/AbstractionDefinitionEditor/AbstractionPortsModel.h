//-----------------------------------------------------------------------------
// File: AbstractionPortsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 15.11.2019
//
// Description:
// Data model for the signals within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONPORTSMODEL_H
#define ABSTRACTIONPORTSMODEL_H

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>

#include <IPXACTmodels/common/Qualifier.h>

#include <IPXACTmodels/generaldeclarations.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>
#include <QString>

#include "AbstractionDefinitionSignalRow.h"

class BusDefinition;
class LibraryInterface;
class PortAbstractionInterface;
class Protocol;
class TransactionalPort;
class WirePort;

//-----------------------------------------------------------------------------
//! Data model for the signals within abstraction definition.
//-----------------------------------------------------------------------------
class AbstractionPortsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

    //! Roles for additional data.
    enum portTypeRoles
    {
        isWireRole = Qt::UserRole,
        isTransactionalRole = Qt::UserRole + 1,
        isExtendLockedRole = Qt::UserRole + 2,
        isPortLockedRole = Qt::UserRole + 3
    };

	/*!
	 *  The constructor.
	 *
     *      @param [in] libraryAccess   Interface to the library.
     *      @param [in] parent          Pointer to the owner of this model.
	 */
    AbstractionPortsModel(LibraryInterface* libraryAccess, PortAbstractionInterface* portInterface, 
        PortAbstractionInterface* extendInterface,
        QObject *parent);

	/*!
	 *  The destructor.
	 */
    virtual ~AbstractionPortsModel() = default;

    //! No copying. No assignment.
    AbstractionPortsModel(const AbstractionPortsModel& other) = delete;
    AbstractionPortsModel& operator=(const AbstractionPortsModel& other) = delete;


	/*!
	 *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent      This should always be invalid model index.
     *
     *      @return                 Number of rows the item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the number of columns the model contains.
	 *
	 *      @param [in] parent      This should always be invalid model index.
     *
     *      @return                 Always returns 9 for invalid model indexes.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    
	/*!
	 *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index       Model index that identifies the item.
     *
     *      @return                 Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
	 *  Get the header data for specified header.
	 *
	 *      @param [in] section         The section specifies the row/column number for the header.
	 *      @param [in] orientation     Specifies if horizontal or vertical header is wanted.
	 *      @param [in] role            Specifies the type of the requested data.
     *
     *      @return                     QVariant contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
	 *  Get the data for the specified item.
	 *
	 *      @param [in] index       Specifies the item that's data is requested.
	 *      @param [in] role        The role that defines what kind of data is requested.
     *
     *      @return                 QVariant contains the data for the item.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
	 *  Save the data to the model for specified item.
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
     *
     *      @return             True if saving happened successfully.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    
    /*!
     *  Set the extend port abstractions.
     */
    void setExtendedPorts();

    /*!
     *  Remove the extend port abstractions.
     */
    void removeExtendedPorts();

	/*!
	 *  Set the bus definition for the model.
	 *
	 *      @param [in] busDefinition      The bus definition to set.
	 */
    void setBusDef(QSharedPointer<BusDefinition> busDefinition);

    void resetPortModel();

public slots:

	/*!
	 *  Adds a new wire signal with mode as "any".
	 */
    void addWireSignal();

    /*!
     *  Adds a new transactional signal with mode as "any".
     */
    void addTransactionalSignal();

    /*!
     *  Adds the master mode for the selected ports.
     *
     *      @param [in] indexes     Indexes of the selected ports.
     */
    void addMaster(QModelIndexList const& indexes);

    /*!
     *  Adds the slave mode for the selected ports.
     *
     *      @param [in] indexes     Indexes of the selected ports.
     */
    void addSlave(QModelIndexList const& indexes);

    /*!
     *  Adds a system mode for the selected ports.
     *
     *      @param [in] indexes     Indexes of the selected ports.
     */
    void addSystem(QModelIndexList const& indexes);

    /*!
     *  Adds all the unconnected system groups for the selected ports.
     *
     *      @param [in] indexes     Indexes of the selected ports.
     */
    void addAllSystems(QModelIndexList const& indexes);

    /*!
     *  Remove the item with the given index.
     *
     *      @param [in] index       The index where to remove the item.
     */
    void onRemoveItem(QModelIndex const& index);
   
    /*!
     *  Handle the reloading of extend ports.
     */
    void onResetExtendPorts();

signals:

	/*!
	 *  Inform that the state of the model has changed.
	 */
	void contentChanged();

	/*!
	 *  Sends error message to be printed for the user.
	 *
	 *      @param [in] msg     The error message.
	 */
	void errorMessage(QString const& msg);

	/*!
	 *  Sends notification to be printed to user.
	 *
	 *      @param [in] msg   The notification message.
	 */
	void noticeMessage(QString const& msg);

    /*!
     *  Inform that a port abstraction has been removed.
     *
     *      @param [in] portName    The port name.
     *      @param [in] mode        The interface mode.
     */
    void portRemoved(QString const& portName, const General::InterfaceMode mode);

private:

    /*!
     * Check if the given index is locked and cannot be edited.
     *
     *     @param [in] index  The index to check.
     *
     *     @return True, if the index is locked, otherwise false.
     */
    bool isLocked(QModelIndex const& index) const;

    /*!
     * Check if the given row is from extended abstractions.
     *
     *     @param [in] row  The row number to check.
     *
     *     @return True, if the given row is extended, otherwise false.
     */
     bool isExtendedRow(int row) const;

    /*!
     * Add definitions for extended wire.
     *
     *     @param [in] port         Name of the port to extend.
     *     @param [in] mode         Mode of the port to extend.
     *     @param [in] systemGroup  System group of the port to extend.
     *
     */
    void extendWireMode(std::string const& port, General::InterfaceMode mode,
        std::string const& systemGroup = std::string());

    /*!
     * Add definitions for extended transcational port.
     *
     *     @param [in] port         Name of the port to extend.
     *     @param [in] mode         Mode of the port to extend.
     *     @param [in] systemGroup  System group of the port to extend.
     *
     */
    void extendTransactionalMode(std::string const& port, General::InterfaceMode mode,
        std::string const& systemGroup = std::string());

    /*!
     *  Send change data signals for all the affected items.
     *
     *      @param [in] changedIndexes  List of all the changed model indexes.
     */
    void sendDataChangeForAllChangedItems(QModelIndexList changedIndexes);

    /*!
     *  Create new master or slave signals for the selected ports.
     *
     *      @param [in] newSignalMode   The new interface mode.
     *      @param [in] selection       Indexes of the selected ports.
     */
    void createNewSignal(General::InterfaceMode newSignalMode, QModelIndexList const& selection);

    QVector<int> getSelectedSignalRows(QModelIndexList const& selection);

    /*!
     *  Get the missing system groups for the selected port.
     *
     *      @param [in] signal  Signal of the selected port.
     *
     *      @return The system groups that have not been connected to the system signals of the selected port.
     */

    QStringList getMissingSystemGroupsForSignal(int const& signalIndex) const;


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The bus definition detailed in the abstraction definition.
    QSharedPointer<BusDefinition> busDefinition_;

	//! Contains the rows in the table.
    QList<AbstractionDefinitionSignalRow> table_;

    //! Interface to the library.
    LibraryInterface* libraryAccess_;

    //! Interface to the abstraction definition ports being edited.
    PortAbstractionInterface* portInterface_;

    //! Interface to the abstraction definition being extended, if any.
    PortAbstractionInterface* extendInterface_;
};

#endif // ABSTRACTIONPORTSMODEL_H
