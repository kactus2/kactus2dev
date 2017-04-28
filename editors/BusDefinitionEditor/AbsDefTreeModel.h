//-----------------------------------------------------------------------------
// File: AbsDefTreeModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 27.04.2017
//
// Description:
// A model for displaying abstraction definitions.
//-----------------------------------------------------------------------------

#ifndef ABSDEFTREEMODEL_H
#define ABSDEFTREEMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WirePort.h>

#include <QModelIndex>
#include <QIcon>
#include <QAbstractItemModel>
#include <QSharedPointer>

class BusDefinition;

//-----------------------------------------------------------------------------
//! A model for displaying bus interface port maps.
//-----------------------------------------------------------------------------
class AbsDefTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent                  The owner of this model.
     */
    AbsDefTreeModel(QObject *parent);

    /*!
     *  The destructor.
     */
    ~AbsDefTreeModel();

    /*!
     *  Refreshes the model so that modifications from outside are made visible.
     */
    void refresh();

    /*!
     *  Returns the number of columns in this model.
     *
     *      @param [in] parent  Model index identifying the item whose column count is requested.
     *
     *      @return The number of columns.
     */
    virtual int columnCount(const QModelIndex & parent = QModelIndex() ) const;

    /*! 
     *  Returns the number of rows an item has.
     *
     *      @param [in] parent  Model index identifying the item whose row count is wanted.
     *
     *      @return The number of rows the item has.
     */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    
	/*!
	 *  Get the item flags that defines the possible operations for the item.
	 *
	 *      @param [in] index       Model index that identifies the item that's flags are requested.
     *
     *      @return Qt::ItemFlags specify the possible operations for the item.
	 */
	Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Get the model index of the specified object.
     *
     *      @param [in] row     Row number of the object.
     *      @param [in] column  Column number of the object.
     *      @param [in] parent  Model index of the parent of the object.
     *
     *      @return QModelIndex that identifies the object.
     */
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

    /*!
     *  Get the model index of the parent of the object
     *
     *      @param [in] child   Model index that identifies the child of the object.
     *
     *      @return QModelIndex that identifies the parent of the given object.
     */
    virtual QModelIndex parent(QModelIndex const& child) const;

    /*!
     *  Return header data for the given header column.
     *
     *      @param [in] section      The index of the header column.
	 *      @param [in] orientation  The orientation of the header, only Qt::Horizontal supported.
     *      @param [in] role         The role of the data.
     *
     *      @return QVariant containing the data.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Get the data associated with given object.
     *
     *      @param [in] index Model index that identifies the object that's data is wanted.
     *      @param [in] role Specifies the type of data wanted.
     *
     *      @return QVariant Containing the requested data.
     */
    virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in]  index  Specifies the item that's data is modified
	 *      @param [in]  value  The value to be set.
	 *      @param [in]  role   The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return  True, if data was successfully set, otherwise false.
	 */
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */);

	/*!
	 *  Set the abstraction definition for the model.
	 *
	 *      @param [in] absDef      The Abstraction definition to set.
	 */
	void setAbsDef(QSharedPointer<AbstractionDefinition> absDef);
    
	/*!
	 *  Set the bus definition for the model.
	 *
	 *      @param [in] busDefinition      The bus definition to set.
	 */
    void setBusDef(QSharedPointer<BusDefinition> busDefinition);

public slots:

	/*!
	 *  Adds all signal modes for selected ports.
	 *
	 *      @param [in] indexes     The indexes of the selected ports.
	 */
	void addSignalOptions(const QModelIndexList& indexes);

    /*!
     *  Handler for adding a new port map.
     *
     *      @param [in] itemIndex   The index of the new port.
     */
    void onAddItem(QModelIndex const& itemIndex);
    
	/*!
	 *  Adds all signal modes for selected ports.
	 *
	 *      @param [in] indexes     The indexes of the selected ports.
	 */
	void onRemoveItem(QModelIndex const& itemIndex);

signals:

    /*!
     *  Emitted when the contents have changed.
     */
    void contentChanged();

    /*!
     *  Emitted when a port is connected.
     *
     *      @param [in] portName    The name of the connected port.
     */
    void portConnected(QString const& portName);

    /*!
     *  Emitted when a port is disconnected.
     *
     *      @param [in] portName    The name of the disconnected port.
     */
    void portDisconnected(QString const& portName);

    /*!
     *  Inform that a port abstraction has been removed.
     *
     *      @param [in] portName    The port name.
     *      @param [in] mode        The interface mode.
     */
    void portRemoved(QString const& portName, const General::InterfaceMode mode);

private:

    // Disable copying.
    AbsDefTreeModel(AbsDefTreeModel const& rhs);
    AbsDefTreeModel& operator=(AbsDefTreeModel const& rhs);

    /*!
     *  Get the left bound of a logical port.
     *
     *      @param [in] logicalPort     The selected logical port.
     *
     *      @return The left bound of a logical port (portSize - 1).
     */
    QVariant getLogicalLeftBound(QSharedPointer<PortAbstraction> logicalPort) const;

    /*!
     *  Get the indexed port map.
     *
     *      @param [in] parentIndex     The parent index of the port map.
     *      @param [in] row             The row of the port map.
     *
     *      @return The indexed port map on the selected row and belonging to the parent index.
     */
    QSharedPointer<WirePort> getIndexedWirePort(QModelIndex const& parentIndex, int row) const;

    /*!
     *  Create a parent index for a port map.
     *
     *      @param [in] childItem   The selected port map.
     *
     *      @return Parent index for the port map.
     */
    QModelIndex createParentIndexForPortMap(WirePort* childItem) const;

    /*!
     *  Get the name of the logical port on the selected index.
     *
     *      @param [in] index           The index of the port map.
     *      @param [in] portMap         The selected port map.
     *      @param [in] logicalPort     The logical port containing the port map.
     *
     *      @return The name of the logical port.
     */
    QVariant getLogicalPortName(QModelIndex const& index, QSharedPointer<WirePort> portMap,
        QSharedPointer<PortAbstraction> logicalPort) const;

    /*!
     *  Get the value for the selected index.
     *
     *      @param [in] index           The selected index.
     *      @param [in] logicalPort     The logical port of the selected index.
     *      @param [in] portMap         The selected port map.
     */
    QVariant valueForIndex(QModelIndex const& index, QSharedPointer<PortAbstraction> logicalPort,
        QSharedPointer<WirePort> portMap) const;

    /*!
     *  Get an icon matching the given direction.
     *
     *      @param [in] direction   The selected direction.
     *
     *      @return The icon matching the selected direction.
     */
    QIcon getIconForDirection(DirectionTypes::Direction direction) const;

    /*!
     *  Check if an index is valid.
     *
     *      @param [in] index   The selected index.
     *
     *      @return True, if the selected index is valid, otherwise false.
     */
    bool indexIsValid(QModelIndex const& index) const;

    /*!
     *  Set the logical value at the selected index.
     *
     *      @param [in] index           The selected index.
     *      @param [in] value           The new logical value.
     *      @param [in] changedPortMap  The selected port map.
     */
    void setLogicalValue(QModelIndex const& index, const QVariant& value, QSharedPointer<WirePort> changedPortMap);

    /*!
     *  Set the physical value at the selected index.
     *
     *      @param [in] index           The selected index.
     *      @param [in] value           The new physical value.
     *      @param [in] changedPortMap  The selected port map.
     */
    void setPhysicalValue(QModelIndex const& index, const QVariant& value, QSharedPointer<WirePort> changedPortMap);

    /*!
     *  Emit a port connection signal.
     *
     *      @param [in] oldName     The old name of the port.
     *      @param [in] newName     The new name of the port.
     */
    void sendPortConnectionSignal(QString const& oldName, QString const& newName);

    /*!
     *  Get the background color for the selected index.
     *
     *      @param [in] index           The selected index.
     *      @param [in] logicalPort     Logical port of the selected index.
     *      @param [in] wirePort        Wire port of the index.
     */
    QVariant getBackgroundColour(QModelIndex const& index, QSharedPointer<PortAbstraction> logicalPort,
        QSharedPointer<WirePort> wirePort) const;

	/*!
	 *  Creates a single row into the table from port abstraction.
	 *
	 *      @param [in] portAbs         The port abstraction of the port.
	 *      @param [in] modeSpesific    The mode specific definitions of the port.
	 *      @param [in] mode            The mode of the port to be created.
	 */
	void createRow(QSharedPointer<PortAbstraction> portAbs, QSharedPointer<WirePort> modeSpesific,
		General::InterfaceMode mode);
     
	/*! Convert PortQualifier to string.
	 *
	 *      @param [in]  qualifier The Qualifier to convert.
	 *
	 *      @return A string representation for the Qualifier.
	*/
	QString toString(Qualifier qualifier) const;

	/*! Convert a string to PortQualifier.
	 *
	 *      @param [in] str The string to convert.
	 *
	 *      @return A qualifier that matches the string.
	*/
	Qualifier::Type toQualifier(QString const& str) const;

    //-----------------------------------------------------------------------------
    // Data.
    //----------------------------------------------------------------------------- 
    
    //! Represents a single port abstraction, along its modes.
	struct PortAbstractionEntry
    {
        //! Defines the Port represented in the row.
        QSharedPointer<PortAbstraction> portAbstraction_;

        //!???
        QList<QSharedPointer<WirePort> > wirePorts_;

		//! The default constructor.
		PortAbstractionEntry();

		//! Copy constructor
		PortAbstractionEntry(PortAbstractionEntry const& other);

		/*!
		 *  Comparison of two SignalRows.
		 *
		 *      @param [in] other   The SignalRow to compare this to.
		 *
		 *      @return True, if the SignalRows are equal, otherwise false.
		 */
		//bool operator==(SignalRow const& other) const;

		/*!
		 *  Comparison of two SignalRows.
		 *
		 *      @param [in] other   The SignalRow to compare this to.
		 *
		 *      @return True, if the SignalRows are not equal, otherwise false.
		 */
		//bool operator!=(SignalRow const& other) const;

		/*!
		 *  Less than comparison of two SignalRows for table ordering.
		 *
		 *      @param [in] other   The SignalRow to compare this to.
		 *
		 *      @return True, if this precedes other, otherwise false.
		 */
		//bool operator<(SignalRow const& other) const;*/
    };

    //! The abstraction definition being edited.
    QSharedPointer<AbstractionDefinition> absDef_;

    //! The bus definition detailed in the abstraction definition.
    QSharedPointer<BusDefinition> busDefinition_;

    //! Contains the rows in the tree.
    QList<QSharedPointer<PortAbstractionEntry> > table_;
};

//-----------------------------------------------------------------------------

#endif // ABSDEFTREEMODEL_H
