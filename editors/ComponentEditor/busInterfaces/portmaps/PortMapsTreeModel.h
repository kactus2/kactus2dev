//-----------------------------------------------------------------------------
// File: PortMapsTreeModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 7.10.2013
//
// Description:
// A model for displaying port maps in component.
//-----------------------------------------------------------------------------

#ifndef PORTMAPSTREEMODEL_H
#define PORTMAPSTREEMODEL_H

#include <QAbstractItemModel>
#include <QTimer>
#include <QMap>
#include <QSharedPointer>

#include <IPXACTmodels/generaldeclarations.h>

class AbstractionDefinition;
class BusInterface;
class Component;
class LibraryInterface;
class PortMapsTreeItem;
class VLNV;

//-----------------------------------------------------------------------------
//! The data model for the port maps in component.
//-----------------------------------------------------------------------------
class PortMapsTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

    //-----------------------------------------------------------------------------
    //! Column ids.
    //-----------------------------------------------------------------------------
    enum PortMapsColumn
    {
        COLUMN_TREE = 0,
        COLUMN_PHYSICAL,
        COLUMN_WIDTH_LOGICAL,
        COLUMN_WIDTH_PHYSICAL,
        COLUMN_COUNT
    };

    /*!
     *  Constructor.
     *
     *      @param [in] component  The component to which this model is made.
     */
    PortMapsTreeModel(BusInterface* busif,
        QSharedPointer<Component> component,
        LibraryInterface* handler,
        QObject *parent);

    /*!
     *  Destructor.
     */
    ~PortMapsTreeModel();

    /*!
     *  Refreshes the model so that modifications from outside are made visible.
     */
    void refresh();


    /*!
     *  Resets the model and restores the port maps from the component.
     */
    void reset();

   	/*! Create a port map for given ports
	 *
	 * @param [in] physicalPort Name of the physical port
	 * @param [in] logicalPort Name of the logical port
	 *
	*/
	void createMap(const QString& physicalPort, const QString& logicalPort);

	/*! Create a port map for given port map
	 *
	 * @param [in] portMap Port map to add to model.
	 *
	*/
	void createMap(QSharedPointer<General::PortMap> portMap);


    /*!
     *  Return header data for the given header column.
     *
     *      @param [in] section      The index of the header column.
	 *      @param [in] orientation  The orientation of the header, only Qt::Horizontal supported.
     *      @param [in] role         The role of the data.
     *
     *      @return QVariant containing the data.
    */
    virtual QVariant headerData(int section, Qt::Orientation orientation,
    		                    int role = Qt::DisplayRole) const;

    /*!
     *  Returns the number of columns in this model.
     *
     *      @param [in] parent Model index identifying the item whose column count is requested.
     *
     *      @return The number of columns.
     */
    virtual int columnCount(const QModelIndex & parent = QModelIndex() ) const;

    /*! 
     *  Returns the number of rows an item has.
     *
     *      @param [in] parent Model index identifying the item whose row count is wanted.
     *
     *      @return The number of rows the item has.
    */
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /*! Get the model index of the specified object.
     *
     * @param [in] row Row number of the object.
     * @param [in] column Column number of the object.
     * @param [in] parent Model index of the parent of the object.
     *
     * @return QModelIndex that identifies the object.
    */
    virtual QModelIndex index(int row, int column,
    		const QModelIndex &parent = QModelIndex()) const;

    /*! Get the model index of the parent of the object
     *
     * @param [in] child Model index that identifies the child of the object.
     *
     * @return QModelIndex that identifies the parent of the given object.
    */
    virtual QModelIndex parent(QModelIndex const& child) const;

    /*! Get the data associated with given object.
     *
     * @param [in] index Model index that identifies the object that's data is wanted.
     * @param [in] role Specifies the type of data wanted.
     *
     * @return QVariant Containing the requested data.
    */
    virtual QVariant data(const QModelIndex& index,
    		              int role = Qt::DisplayRole) const;

    virtual bool setData(const QModelIndex &index, const QVariant &value, int role /* = Qt::EditRole */);

    /*! Does the specified item have child items or not.
     *
     * @param [in] parent Model index identifying the object that's children are asked.
     *
     * @return True if object has child objects.
    */
    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const;

    /*! Get the flags that identify possible methods for given object.
     *
     * @param [in] index Model index identifying the object that's flags are requested.
     *
     * @return Qt::ItemFlags that specify how the object can be handled.
    */
    Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Returns the number of files in the model.
     */
    int getTotalStepCount() const;

    /*!
     *  Evaluates the validness of the model.
     *
     *      @return True, if everything is valid, otherwise false.
     */
    bool isValid() const;

    /*!
     *  Evaluates the validness of the model.
     *
     *      @param [inout] errorList The list to add the possible error messages to.
     *
     *      @return True, if everything is valid, otherwise false.
     */
    bool isValid(QStringList& errorList) const;

	/*! Get list of logical ports in this model.
	 *
	 * @return QStringList containing the port names
	*/
	QStringList logicalPorts() const;

	/*! Set the abstraction definition that is used in this port map.
	 *
	 * @param [in] vlnv Identifies the abstraction definition.
	 *
	*/
	void setAbsType(const VLNV& vlnv, General::InterfaceMode mode);

	/*! Check if the two ports can be mapped together.
	 *
	 * @param [in] physicalPort Identifies the physical port.
	 * @param [in] logicalPort  Identifies the logical port.
	 *
	 * @return True, if port mapping can be done, otherwise false.
	*/
    bool canCreateMap( const QString& physicalPort, const QString& logicalPort ) const;

public slots:

    /*!
     *  Called when a logical port is restored to port list.
     *
     *      @param [in] index   The index for the item to restore.
     */
    void restoreItem(QModelIndex const& index);

    /*!
     *  Called when an item is to be removed from the model.
     *
     *      @param [in] index   The index for the item to remove.
     */
    void removeItem(QModelIndex const& index);

    /*!
     *  Called when an item and the associated port maps are to be removed.
     *
     *      @param [in] index   The index for the item to remove.
     */
    void removeMapping(QModelIndex const& index);

signals:   

    /*!
     *  Emitted when the contents have changed.
     */
    void contentChanged();

    //! Prints an error message to the user.
    void errorMessage(const QString& msg) const;

    //! Prints a notification to user.
    void noticeMessage(const QString& msg) const;

    //! When port map is removed informs name of the logical port.
    void logicalRemoved(const QString& logicalName) const;

    //! When port map is restored informs name of the logical port.
    void logicalRestored(const QString& logicalName) const;

    //! When port map is removed informs name of the physical port.
    void physicalRemoved(const QString& physicalName) const;
   

private:
    // Disable copying.
    PortMapsTreeModel(PortMapsTreeModel const& rhs);
    PortMapsTreeModel& operator=(PortMapsTreeModel const& rhs);


    /*!
     * Returns a pointer to the item with a given name.
     *
     *      @param [in] name   The name of the item to find.
     *
     *      @return The item or 0, if item is not found.
     */
    PortMapsTreeItem* findItem(QString const& name);
   
    /*!
     *  Returns the index of a given item.
     *
     *      @param [in] item    The item whose index is requested.
     *      @param [in] column  The index column.
     *
     *      @return The index for the given item.
     */
    QModelIndex getItemIndex(PortMapsTreeItem* item, int column) const;

    //-----------------------------------------------------------------------------
    // Data.
    //----------------------------------------------------------------------------- 

    //! The parent component.
    QSharedPointer<Component> component_;

    //! The dependency tree root.
    PortMapsTreeItem* root_;

    //! Pointer to the bus interface being edited.
    BusInterface* busif_;

    //! Pointer to the data structure within the model containing the port maps.
    QList<QSharedPointer<General::PortMap> >& portMaps_;

    //! Pointer to the instance that manages the library.
    LibraryInterface* handler_;

    //! Pointer to the abstraction definition that is used.
    QSharedPointer<AbstractionDefinition> absDef_;

    //! Specifies the interface mode of this bus interface
    General::InterfaceMode interfaceMode_;
};

//-----------------------------------------------------------------------------

#endif // PORTMAPSTREEMODEL_H
