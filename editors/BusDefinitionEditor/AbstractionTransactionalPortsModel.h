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
class LibraryInterface;
class PortAbstractionInterface;

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
     *      @param [in] libraryAccess   Interface to the library.
     *      @param [in] portInterface   Interface for accessing port abstractions.
     *      @param [in] parent          Pointer to the owner of this model.
	 */
    AbstractionTransactionalPortsModel(LibraryInterface* libraryAccess, PortAbstractionInterface* portInterface,
        QObject *parent);

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
     *  Reset the port abstraction model.
     */
    void resetPortModel();

	/*!
	 *  Set the bus definition for the model.
	 *
	 *      @param [in] busDefinition   The bus definition to set.
	 */
    void setBusDef(QSharedPointer<BusDefinition> busDefinition);

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

    /*!
     *  Get a list signal rows contained within the selected indexes.
     *
     *      @param [in] selection   Indexes of the selected ports.
     *
     *      @return List of signal rows contained within the selected indexes.
     */
    QVector<int> getSelectedSignalRows(QModelIndexList const& selection);

    /*!
     *  Get the missing system groups for the selected port.
     *
     *      @param [in] signalIndex     Signal of the selected port.
     *
     *      @return The system groups that have not been connected to the system signals of the selected port.
     */
    QStringList getMissingSystemGroupsForSignal(int const& signalIndex) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The bus definition detailed in the abstraction definition.
    QSharedPointer<BusDefinition> busDefinition_;

    //! Interface for accessing port abstractions.
    PortAbstractionInterface* portInterface_;

    //! Interface to the library.
    LibraryInterface* libraryAccess_;
};

#endif // ABSTRACTIONTRANSACTIONALPORTSMODEL_H
