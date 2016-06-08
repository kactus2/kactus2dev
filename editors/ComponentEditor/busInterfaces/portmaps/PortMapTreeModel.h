//-----------------------------------------------------------------------------
// File: PortMapTreeModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.05.2016
//
// Description:
// A model for displaying bus interface port maps.
//-----------------------------------------------------------------------------

#ifndef PORTMAPTREEMODEL_H
#define PORTMAPTREEMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>

#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QModelIndex>
#include <QIcon>
#include <QAbstractItemModel>
#include <QSharedPointer>

class AbstractionDefinition;
class BusInterface;
class Component;
class LibraryInterface;
class VLNV;
class ExpressionParser;
class PortAbstraction;
class PortMap;
class ExpressionFormatter;
class PortMapValidator;
class Port;

//-----------------------------------------------------------------------------
//! A model for displaying bus interface port maps.
//-----------------------------------------------------------------------------
class PortMapTreeModel : public QAbstractItemModel, ParameterizableTable
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] busif                   The containing bus interface.
     *      @param [in] component               The component to which this model is made.
     *      @param [in] handler                 The library handler.
     *      @param [in] expressionParser        The used expression parser.
     *      @param [in] expressionFormatter     The used expression formatter.
     *      @param [in] parameterFinder         The used parameter finder.
     *      @param [in] portMapValidator        Validator used for port maps.
     *      @param [in] parent                  The owner of this model.
     */
    PortMapTreeModel(QSharedPointer<BusInterface> busif, QSharedPointer<Component> component,
        LibraryInterface* handler, QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<PortMapValidator> portMapValidator, QObject *parent);

    /*!
     *  The destructor.
     */
    ~PortMapTreeModel();

    /*!
     *  Refreshes the model so that modifications from outside are made visible.
     */
    void refresh();


    /*!
     *  Resets the model and restores the port maps from the component.
     */
    void reset();

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
     *  Get the flags that identify possible methods for given object.
     *
     *      @param [in] index   Model index identifying the object that's flags are requested.
     *
     *      @return Qt::ItemFlags that specify how the object can be handled.
     */
    Qt::ItemFlags flags(QModelIndex const& index) const;

	/*!
     *  Set the abstraction definition that is used in this port map.
	 *
	 *      @param [in] vlnv    Identifies the abstraction definition.
     *      @param [in] mode    The used bus interface mode.
	 */
	void setAbsType(const VLNV& vlnv, General::InterfaceMode mode);

    /*!
     *  Get the list of acceptable mime types.
     *
     *      @return The list of acceptable mime types.
     */
    QStringList mimeTypes() const;

    /*!
     *  Get the supported drop actions.
     *
     *      @return The supported drop actions.
     */
    virtual Qt::DropActions supportedDropActions() const;

    /*!
     *  Drop the mime data.
     *
     *      @param [in] data    The actual data.
     *      @param [in] action  The used drop action.
     *      @param [in] row     The selected row.
     *      @param [in] column  The selected column.
     *      @param [in] parent  Parent index of the new data.
     */
    virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
        const QModelIndex &parent);

public slots:

    /*!
     *  Handler for adding a new port map.
     *
     *      @param [in] itemIndex   The index of the new port.
     */
    void onAddPortMap(QModelIndex const& itemIndex);

    /*!
     *  Handler for removing a port map.
     *
     *      @param [in] itemIndex   The index of the removed port.
     */
    void onRemovePort(QModelIndex const& itemIndex);

    /*!
     *  Handler for removing all port maps connected to the logical port.
     *
     *      @param [in] itemIndex   The index of the logical port.
     */
    void onRemoveAllChildItemsFrom(QModelIndex const& itemIndex);

    /*!
     *  Handler for removing all port maps from all the logical ports.
     */
    void onRemoveAllPortMappings();

    /*!
     *  Add a port map to the correct parent item.
     *
     *      @param [in] newPortMap  The selected port map.
     */
    void onAddConnectedPortMap(QSharedPointer<PortMap> newPortMap);

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

protected:

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;
    
    /*!
     *  Gets the expression for the given index or the plain value if expression is not available.
     *
     *      @param [in] index   The index whose expression to get.
     *
     *      @return The expression for the index if available, otherwise the value for the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const; 

    /*!
     *  Validates the data in an index.
     *
     *      @param [in] index   The index whose data to validate
     *
     *      @return True, if the data in the index is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

private:

    // Disable copying.
    PortMapTreeModel(PortMapTreeModel const& rhs);
    PortMapTreeModel& operator=(PortMapTreeModel const& rhs);

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
    QSharedPointer<PortMap> getIndexedPortMap(QModelIndex const& parentIndex, int row) const;

    /*!
     *  Create a parent index for a port map.
     *
     *      @param [in] childItem   The selected port map.
     *
     *      @return Parent index for the port map.
     */
    QModelIndex createParentIndexForPortMap(PortMap* childItem) const;

    /*!
     *  Get the name of the logical port on the selected index.
     *
     *      @param [in] index           The index of the port map.
     *      @param [in] portMap         The selected port map.
     *      @param [in] logicalPort     The logical port containing the port map.
     *
     *      @return The name of the logical port.
     */
    QVariant getLogicalPortName(QModelIndex const& index, QSharedPointer<PortMap> portMap,
        QSharedPointer<PortAbstraction> logicalPort) const;

    /*!
     *  Get the name of the physical port on the selected index.
     *
     *      @param [in] itemIndex   The selected index.
     *      @param [in] portMap     The selected port map.
     *
     *      @return The name of the physical port.
     */
    QVariant getPhysicalPortName(QModelIndex const& itemIndex, QSharedPointer<PortMap> portMap) const;

    /*!
     *  Get the value for the selected index.
     *
     *      @param [in] index           The selected index.
     *      @param [in] logicalPort     The logical port of the selected index.
     *      @param [in] portMap         The selected port map.
     */
    QVariant valueForIndex(QModelIndex const& index, QSharedPointer<PortAbstraction> logicalPort,
        QSharedPointer<PortMap> portMap) const;

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
     *  Switch the logical port of the port map.
     *
     *      @param [in] index               The current index of the port map.
     *      @param [in] value               The name of the new logical port.
     *      @param [in] switchedPortMap     The selected port map.
     */
    void switchMappedLogicalPort(QModelIndex const& index, const QVariant& value,
        QSharedPointer<PortMap> switchedPortMap);

    /*!
     *  Set the logical value at the selected index.
     *
     *      @param [in] index           The selected index.
     *      @param [in] value           The new logical value.
     *      @param [in] changedPortMap  The selected port map.
     */
    void setLogicalValue(QModelIndex const& index, const QVariant& value, QSharedPointer<PortMap> changedPortMap);

    /*!
     *  Set the physical value at the selected index.
     *
     *      @param [in] index           The selected index.
     *      @param [in] value           The new physical value.
     *      @param [in] changedPortMap  The selected port map.
     */
    void setPhysicalValue(QModelIndex const& index, const QVariant& value, QSharedPointer<PortMap> changedPortMap);

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
     *      @param [in] portMap         Port map of the index.
     */
    QVariant getBackgroundColour(QModelIndex const& index, QSharedPointer<PortAbstraction> logicalPort,
        QSharedPointer<PortMap> portMap) const;

    //-----------------------------------------------------------------------------
    // Data.
    //----------------------------------------------------------------------------- 

    //! The parent component.
    QSharedPointer<Component> component_;

    //! The instance that manages the library.
    LibraryInterface* handler_;

    //! The bus interface being edited.
    QSharedPointer<BusInterface> containingBusInterface_;    

    //! The abstraction definition that is used.
    QSharedPointer<AbstractionDefinition> absDef_;

    //! Specifies the interface mode of this bus interface
    General::InterfaceMode interfaceMode_;

    //! The used expression formatter.
    QSharedPointer<ExpressionFormatter> formatter_;

    //! Structure for a port mapping.
    struct PortMapping 
    {
        //! The logical port of the port maps.
        QSharedPointer<PortAbstraction> logicalPort_;

        //! A list of port maps with the same logical port.
        QList<QSharedPointer<PortMap> > portMaps_;
    };

    //! A list of port mappings.
    QList<PortMapping> portMappings_;

    //! The used port map validator.
    QSharedPointer<PortMapValidator> portMapValidator_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPTREEMODEL_H
