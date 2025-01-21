//-----------------------------------------------------------------------------
// File: PortWireTypeModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 24.11.2017
//
// Description:
// Table model that can be used to display port wire type definitions.
//-----------------------------------------------------------------------------

#ifndef PORTWIRETYPEMODEL_H
#define PORTWIRETYPEMODEL_H

#include <QAbstractItemModel>
#include <QSharedPointer>

class WireTypeDef;
class Port;

class PortTypeValidator;

//-----------------------------------------------------------------------------
//! Table model that can be used to display port wire type definitions.
//-----------------------------------------------------------------------------
class PortWireTypeModel : public QAbstractItemModel
{
    Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
     *    @param [in] typeValidator   Validator for port type definitions.
	 *    @param [in] parent          Pointer to the owner of this model.
     */
	PortWireTypeModel(QSharedPointer<PortTypeValidator> typeValidator, QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~PortWireTypeModel();

	/*!
     *  Get the number of rows in the model.
	 *
	 *    @param [in] parent  Model index of the parent of the item.
	 *
	 *    @return Number of rows currently in the model.
     */
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*!
     *  Get the number of columns in the model.
	 *
	 *    @param [in] parent  Model index of the parent of the item.
	 *
	 *    @return Always returns 2.
     */
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

    /*!
     *  Get the data for the headers.
	 *
	 *    @param [in] section         The column that's header is wanted.
	 *    @param [in] orientation     Only Qt::Horizontal is supported.
	 *    @param [in] role            Specified the type of data that is wanted.
	 *
	 *    @return QVariant containing the data to be displayed.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *    @param [in] index   Identifies the item that's data is wanted.
	 *    @param [in] role    Specifies what kind of data is wanted.
	 *
	 *    @return QVariant containing the data.
     */
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole ) const;

	/*!
     *  Set the data for specified item.
	 *
	 *    @param [in] index   Specifies the item that's data is modified.
	 *    @param [in] value   The value to be set.
	 *    @param [in] role    The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *    @return True if data was successfully set.
	 */
	virtual bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);

	/*!
     *  Create an index for the identified item.
	 *
     *    @param [in] row     Row of the item.
	 *    @param [in] column  Column of the item.
     *    @param [in] parent  Parent index of the item.
	 *
	 *    @return The created model index.
     */
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex() ) const;

    /*!
     *  Create a parent index for the selected item.
     *
     *    @param [in] child   The selected item.
     */
    QModelIndex parent(const QModelIndex &child) const;
    
    /*!
     *  Get information on how specified item can be handled.
	 *
	 *    @param [in] index   Specifies the item that's flags are wanted.
	 *
	 *    @return Qt::ItemFlags that define how object can be handled.
     */
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Change the currently edited port.
     *
     *    @param [in] newPort     The new port.
     */
    void onChangePortSelection(QSharedPointer<Port> newPort);

public slots:

	/*!
     *  Handles the addition of new port types.
	 *
	 *    @param [in] indexList   List of type indexes for creating new types.
     */
    void onAddItems(QModelIndexList const& indexList);

	/*!
     *  Handles the addition of new port type view references.
	 *
	 *    @param [in] indexList   List of type indexes to contain the new view references.
     */
    void onAddViewReferences(QModelIndexList const& indexList);

	/*!
     *  Handles the removal of items from the model.
	 * 
	 *    @param [in] indexList   Identifies the items that should be removed.
     */
    void onRemoveSelectedItems(QModelIndexList const& indexList);

	/*!
	 *  Handles the removal of all the view references from the selected types.
	 *
	 *    @param [in] indexList   Identifies the type containing the removed view references.
	 */
    void onRemoveAllViewsFromSelectedTypes(QModelIndexList const& indexList);

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
	PortWireTypeModel(const PortWireTypeModel& other);
	PortWireTypeModel& operator=(const PortWireTypeModel& other);

    /*!
     *  Get the view references of the selected port type.
     *
     *    @param [in] typeIndex   Index of the selected type.
     *
     *    @return The list of view references of the selected port type.
     */
    QSharedPointer<QStringList> getViewsForType(QModelIndex const& typeIndex) const;

    /*!
     *  Get the type definitions of the selected port type.
     *
     *    @param [in] typeIndex   Index of the selected type.
     *
     *    @return The list of type definitions of the selected port type.
     */
    QSharedPointer<QStringList> getTypeDefinitionsForType(QModelIndex const& typeIndex) const;

    /*!
     *  Get the selected wire type definition.
     *
     *    @param [in] index   Index of the selected wire type definition.
     */
    QSharedPointer<WireTypeDef> getIndexedWireTypeDefinition(QModelIndex index) const;

    /*!
     *  Get the color for valid and invalid indexes.
     *
     *    @param [in] index           The selected index.
     *    @param [in] typeDefinition  The indexed type definition.
     *
     *    @return Black color for valid indexes, red for invalid.
     */
    QVariant blackForValidRedForInvalid(QModelIndex const& index, QSharedPointer<WireTypeDef> typeDefinition)
        const;

    /*!
     *  Validate the selected index.
     *
     *    @param [in] index           The selected index.
     *    @param [in] typeDefinition  The indexed type definition.
     */
    bool validateIndex(QModelIndex const& index, QSharedPointer<WireTypeDef> typeDefinition) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The contained wire type definitions.
    QSharedPointer<QList<QSharedPointer<WireTypeDef> > > wireTypeDefinitions_;

    //! The validator for type definitions.
    QSharedPointer<PortTypeValidator> typeValidator_;
};

#endif // PORTWIRETYPEMODEL_H
