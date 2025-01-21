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

#include <editors/ComponentEditor/common/ReferencingTableModel.h>
#include <editors/ComponentEditor/common/ParameterizableTable.h>

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
class AbstractionPortsModel : public ReferencingTableModel, public ParameterizableTable
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
     *    @param [in] parameterFinder     The parameter finder to use.
     *    @param [in] expressionParser    The expression parser to use.
     *    @param [in] libraryAccess       Interface to the library.
     *    @param [in] portInterface       The port abstraction interface to use.
     *    @param [in] extendInterface     The port abstraction interface to use for extended abstractions.
     *    @param [in] parent              Pointer to the owner of this model.
	 */
    AbstractionPortsModel(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser, LibraryInterface* libraryAccess,
        PortAbstractionInterface* portInterface, PortAbstractionInterface* extendInterface, QObject *parent);

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
	 *    @param [in] parent      This should always be invalid model index.
     *
     *    @return                 Number of rows the item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
	 *  Get the number of columns the model contains.
	 *
	 *    @param [in] parent      This should always be invalid model index.
     *
     *    @return                 Always returns 9 for invalid model indexes.
	 */
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    
	/*!
	 *  Get the item flags that defines the possible operations for the item.
	 *
	 *    @param [in] index       Model index that identifies the item.
     *
     *    @return                 Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*!
	 *  Get the header data for specified header.
	 *
	 *    @param [in] section         The section specifies the row/column number for the header.
	 *    @param [in] orientation     Specifies if horizontal or vertical header is wanted.
	 *    @param [in] role            Specifies the type of the requested data.
     *
     *    @return                     QVariant contains the requested data.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	/*!
	 *  Get the data for the specified item.
	 *
	 *    @param [in] index       Specifies the item that's data is requested.
	 *    @param [in] role        The role that defines what kind of data is requested.
     *
     *    @return                 QVariant contains the data for the item.
	 */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*!
	 *  Save the data to the model for specified item.
	 *
	 *    @param [in] index   The model index of the item that's data is to be saved.
	 *    @param [in] value   The data that is to be saved.
	 *    @param [in] role    The role specifies what kind of data should be saved.
     *
     *    @return             True if saving happened successfully.
	 */
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

    /*!
     *	Save the qualifier data to the IPXACT model.
     *  
     *    @param [in] index   The model index of the item that's data is to be saved.
     *    @param [in] value   The data that is to be saved.
     */
    void setQualifierData(QModelIndex const& index, QVariant const& value);

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
	 *    @param [in] busDefinition      The bus definition to set.
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
     *    @param [in] indexes     Indexes of the selected ports.
     */
    void addMaster(QModelIndexList const& indexes);

    /*!
     *  Adds the slave mode for the selected ports.
     *
     *    @param [in] indexes     Indexes of the selected ports.
     */
    void addSlave(QModelIndexList const& indexes);

    /*!
     *  Adds a system mode for the selected ports.
     *
     *    @param [in] indexes     Indexes of the selected ports.
     */
    void addSystem(QModelIndexList const& indexes);

    /*!
     *  Adds all the unconnected system groups for the selected ports.
     *
     *    @param [in] indexes     Indexes of the selected ports.
     */
    void addAllSystems(QModelIndexList const& indexes);

    /*!
     *  Remove the item with the given index.
     *
     *    @param [in] index       The index where to remove the item.
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
	 *    @param [in] msg     The error message.
	 */
	void errorMessage(QString const& msg);

	/*!
	 *  Sends notification to be printed to user.
	 *
	 *    @param [in] msg   The notification message.
	 */
	void noticeMessage(QString const& msg);

    /*!
     *  Inform that a port abstraction has been removed.
     *
     *    @param [in] portName    The port name.
     *    @param [in] mode        The interface mode.
     */
    void portRemoved(QString const& portName, const General::InterfaceMode mode);

private:

    /*!
     * Check if the given index is locked and cannot be edited.
     *
     *    @param [in] index  The index to check.
     *
     *    @return True, if the index is locked, otherwise false.
     */
    bool isLocked(QModelIndex const& index) const;

    /*!
     * Check if the given row is from extended abstractions.
     *
     *    @param [in] row  The row number to check.
     *
     *    @return True, if the given row is extended, otherwise false.
     */
     bool isExtendedRow(int row) const;

    /*!
     * Add definitions for extended wire.
     *
     *    @param [in] port         Name of the port to extend.
     *    @param [in] mode         Mode of the port to extend.
     *    @param [in] systemGroup  System group of the port to extend.
     *
     */
    void extendWireMode(std::string const& port, General::InterfaceMode mode,
        std::string const& systemGroup = std::string());

    /*!
     * Add definitions for extended transcational port.
     *
     *    @param [in] port         Name of the port to extend.
     *    @param [in] mode         Mode of the port to extend.
     *    @param [in] systemGroup  System group of the port to extend.
     *
     */
    void extendTransactionalMode(std::string const& port, General::InterfaceMode mode,
        std::string const& systemGroup = std::string());

    /*!
     *  Send change data signals for all the affected items.
     *
     *    @param [in] changedIndexes  List of all the changed model indexes.
     */
    void sendDataChangeForAllChangedItems(QModelIndexList changedIndexes);

    /*!
     *  Create new master or slave signals for the selected ports.
     *
     *    @param [in] newSignalMode   The new interface mode.
     *    @param [in] selection       Indexes of the selected ports.
     */
    void createNewSignal(General::InterfaceMode newSignalMode, QModelIndexList const& selection);

    QVector<int> getSelectedSignalRows(QModelIndexList const& selection);

    /*!
     *  Get the missing system groups for the selected port.
     *
     *    @param [in] signal  Signal of the selected port.
     *
     *    @return The system groups that have not been connected to the system signals of the selected port.
     */

    QStringList getMissingSystemGroupsForSignal(int const& signalIndex) const;

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *    @param [in] index   The index being evaluated.
     *
     *    @return     True, if column can have expressions, false otherwise.
     */
    bool isValidExpressionColumn(QModelIndex const& index) const final;

    /*!
     *  Gets the expression for the given index or the plain value if expression is not available.
     *
     *    @param [in] index   The index whose expression to get.
     *
     *    @return The expression for the index if available, otherwise the value for the given index.
     */
    QVariant expressionOrValueForIndex(QModelIndex const& index) const final;

    /*!
     *	Get the unformatted expression for a selected index.
     *  
     *    @param [in] index     Index to get the expression of.
     *	    
     * 	    @return The unformatted expression.
     */
    QVariant expressionForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in an index.
     *
     *    @param [in] index   The index whose data to validate
     *
     *    @return True, if the data in the index is valid, otherwise false.
     */
    bool validateIndex(QModelIndex const& index) const final;

    /*!
     *  Gets the number of all the references made to a selected id on the selected row.
     *
     *    @param [in] row         The row of the selected item.
     *    @param [in] valueID     The id of the referenced parameter.
     *
     *    @return The amount of references made to the selected id on the selected row.
     */
    int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const final;

    /*!
     *  Get the value for the corresponding index.
     *
     *    @param [in] index   The index whose value is being searched for.
     */
    QVariant valueForIndex(const QModelIndex& index) const;

    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *    @param [in] index   The selected index.
     *
     *    @return The formatted value of an expression in the selected index.
     */
    QVariant formattedExpressionForIndex(QModelIndex const& index) const;

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
