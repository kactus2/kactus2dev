//-----------------------------------------------------------------------------
// File: AbstractionTransactionalPortsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 05.06.2018
//
// Description:
// Data model for the transactional ports within abstraction definition.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTIONTRANSACTIONALPORTSMODEL_H
#define ABSTRACTIONTRANSACTIONALPORTSMODEL_H

#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/common/Qualifier.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalPort.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>
#include <QString>
#include <QModelIndex>

class BusDefinition;
class AbstractionDefinition;
class Protocol;

//-----------------------------------------------------------------------------
//! Data model for the transactional ports within abstraction definition.
//-----------------------------------------------------------------------------
class AbstractionTransactionalPortsModel : public QAbstractTableModel
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   Pointer to the owner of this model.
	 */
    AbstractionTransactionalPortsModel(QObject *parent);
	
	/*!
	 *  The destructor.
	 */
    virtual ~AbstractionTransactionalPortsModel() = default;

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
	 *  Set the abstraction definition for the model.
	 *
	 *      @param [in] absDef  The Abstraction definition to set.
	 */
	void setAbsDef(QSharedPointer<AbstractionDefinition> absDef);
    
	/*!
	 *  Set the bus definition for the model.
	 *
	 *      @param [in] busDefinition   The bus definition to set.
	 */
    void setBusDef(QSharedPointer<BusDefinition> busDefinition);

	/*!
	 *  Write the ports from the table to the abstraction definition.
	 */
	void save();

public slots:

	/*!
	 *  Adds a new signal with mode as "any".
	 */
	void addSignal();

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
	//! No copying. No assignment.
    AbstractionTransactionalPortsModel(const AbstractionTransactionalPortsModel& other);
    AbstractionTransactionalPortsModel& operator=(const AbstractionTransactionalPortsModel& other);
    
	/*!
	 *  Save the port from table to a port abstraction.
	 *
	 *      @param [in] portAbs     Pointer to the port abstraction to save the port to.
	 *      @param [in] i           Index of the port in the table.
	 */
	void savePort(QSharedPointer<PortAbstraction> portAbs, int i);

	/*!
	 *  Creates a single row into the table from port abstraction.
	 *
	 *      @param [in] portAbs         The port abstraction of the port.
	 *      @param [in] modeSpesific    The mode specific definitions of the port.
	 *      @param [in] mode            The mode of the port to be created.
	 */
	void createRow(QSharedPointer<PortAbstraction> portAbs, QSharedPointer<TransactionalPort> modeSpesific,
		General::InterfaceMode mode);
     
	/*!
     *  Convert PortQualifier to string.
	 *
	 *      @param [in]  qualifier The Qualifier to convert.
	 *
	 *      @return A string representation for the Qualifier.
     */
	QString qualifierToString(Qualifier qualifier) const;

	/*!
     *  Convert a string to PortQualifier.
	 *
	 *      @param [in] str The string to convert.
	 *
	 *      @return A qualifier that matches the string.
     */
	Qualifier::Type toQualifier(QString const& str) const;

    /*!
     *  Send change data signals for all the affected items.
     *
     *      @param [in] changedIndexes  List of all the changed model indexes.
     */
    void sendDataChangeForAllChangedItems(QModelIndexList changedIndexes);

    //! SignalRow represents a single row in the table by grouping the port abstraction and transactional elements.
	struct SignalRow
    {
        //! Defines the Port represented in the row.
        QSharedPointer<PortAbstraction> abstraction_;

		//! Defines the mode of the transactional port (master, slave or system).
		General::InterfaceMode mode_;
        
        //! Defines the transactionalPort represented on the row.
        QSharedPointer<TransactionalPort> transactionalPort_;

		/*!
         *  The default constructor.
         */
		SignalRow();

		/*!
         *  Copy constructor
         */
		SignalRow(SignalRow const& other) = default;

		/*!
		 *  Comparison of two SignalRows.
		 *
		 *      @param [in] other   The SignalRow to compare this to.
		 *
		 *      @return True, if the SignalRows are equal, otherwise false.
		 */
		bool operator==(SignalRow const& other) const;

		/*!
		 *  Comparison of two SignalRows.
		 *
		 *      @param [in] other   The SignalRow to compare this to.
		 *
		 *      @return True, if the SignalRows are not equal, otherwise false.
		 */
		bool operator!=(SignalRow const& other) const;

		/*!
		 *  Less than comparison of two SignalRows for table ordering.
		 *
		 *      @param [in] other   The SignalRow to compare this to.
		 *
		 *      @return True, if this precedes other, otherwise false.
		 */
		bool operator<(SignalRow const& other) const;
	};

    /*!
     *  Create new master or slave signals for the selected ports.
     *
     *      @param [in] newSignalMode   The new interface mode.
     *      @param [in] selection       Indexes of the selected ports.
     */
    void createNewSignal(General::InterfaceMode newSignalMode, QModelIndexList const& selection);

    /*!
     *  Get a list of ports contained within the selected indexes.
     *
     *      @param [in] selection   Indexes of the selected ports.
     *
     *      @return List of ports contained within the selected indexes.
     */
    QVector<AbstractionTransactionalPortsModel::SignalRow> getIndexedPorts(QModelIndexList const& selection);

    /*!
     *  Check if selected signaled port has already been selected.
     *
     *      @param [in] portSignal      Signal of the selected port.
     *      @param [in] selectedPorts   List of the ports that have already been selected.
     *
     *      @return True, if the signaled port has been selected, false otherwise.
     */
    bool portHasBeenSelected(AbstractionTransactionalPortsModel::SignalRow const& portSignal,
        QVector<AbstractionTransactionalPortsModel::SignalRow> const& selectedPorts) const;

    /*!
     *  Check if the selected port already contains the selected signal.
     *
     *      @param [in] mode        The selected signal.
     *      @param [in] portName    Name of the selected port.
     *
     *      @return True, if the selected port already contains the selected signal, false otherwise.
     */
    bool modeExistsForPort(General::InterfaceMode const& mode, QString const& portName) const;

    /*!
     *  Get the missing system groups for the selected port.
     *
     *      @param [in] signal  Signal of the selected port.
     *
     *      @return The system groups that have not been connected to the system signals of the selected port.
     */
    QStringList getMissingSystemGroupsForSignal(AbstractionTransactionalPortsModel::SignalRow const& signal) const;

    /*!
     *  Check if the selected port contains other signals.
     *
     *      @param [in] portSignal  Signal of the selected port.
     *
     *      @return True, if the selected port contains other signals, false otherwise.
     */
    bool portHasOtherSignals(AbstractionTransactionalPortsModel::SignalRow const& portSignal) const;

    /*!
     *  Get the initiative of the selected port for the data function.
     *
     *      @param [in] port    The selected port.
     *
     *      @return Initiative of the selected port.
     */
    QString getInitiativeForData(QSharedPointer<TransactionalPort> port) const;

    /*!
     *  Get the correct initiative value from the selected initiative value for the set data function.
     *
     *      @param [in] newInitiativeValue  The selected initiative value.
     *
     *      @return Initiative value usable for the transactional port.
     */
    QString getInitiativeForSetData(QString const& newInitiativeValue) const;

    /*!
     *  Get the protocol type of the selected protocol.
     *
     *      @param [in] portProtocol    The selected protocol.
     *
     *      @return The type of the selected protocol.
     */
    QString getProtocolTypeText(QSharedPointer<Protocol> portProtocol) const;

    /*!
     *  Check if the protocol type of the selected protocol is empty.
     *
     *      @param [in] portProtocol    The selected protocol.
     *
     *      @return True, if the port protocol is empty, false otherwise.
     */
    bool portProcotolTypeIsEmpty(QSharedPointer<Protocol> portProtocol) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! The abstraction definition being edited.
	QSharedPointer<AbstractionDefinition> absDef_;

    //! The bus definition detailed in the abstraction definition.
    QSharedPointer<BusDefinition> busDefinition_;

	//! Contains the rows in the table.
	QList<SignalRow> table_;
};

#endif // ABSTRACTIONTRANSACTIONALPORTSMODEL_H
