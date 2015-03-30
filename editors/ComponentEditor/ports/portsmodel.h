//-----------------------------------------------------------------------------
// File: portsmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 31.3.2011
//
// Description:
// Table model that can be used to display ports to be edited.
//-----------------------------------------------------------------------------

#ifndef PORTSMODEL_H
#define PORTSMODEL_H

#include <QAbstractTableModel>
#include <QMap>
#include <QSharedPointer>
#include <QString>
#include <QList>
#include <QFile>

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

class Model;
class Port;

//-----------------------------------------------------------------------------
//! Table model that can be used to display ports to be edited.
//-----------------------------------------------------------------------------
class PortsModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] model                   Pointer to the model being edited.
     *      @param [in] expressionParser        Pointer to the expression parser.
     *      @param [in] parameterFinder         Pointer to the parameter finder.
     *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the owner of this model.
	 *
	*/
	PortsModel(QSharedPointer<Model> model,
        QSharedPointer <ExpressionParser> expressionParser,
        QSharedPointer <ParameterFinder> parameterFinder,
        QSharedPointer <ExpressionFormatter> expressionFormatter,
        QObject *parent);
	
	//! The destructor
	virtual ~PortsModel();

	/*! Get the number of rows in the model.
	 *
	 *      @param [in] parent      Model index of the parent of the item. Must be invalid
	 *                              because this is not hierarchical model.
	 *
	 *      @return Number of rows currently in the model.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! Get the number of columns in the model
	 *
	 *      @param [in] parent      Model index of the parent of the item. Must be invalid
	 *                              because this is not hierarchical model.
	 *
	 *      @return Always returns 9
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex() ) const;

	/*! Get the data for the specified item for specified role.
	 *
	 *      @param [in]     index Identifies the item that's data is wanted.
	 *      @param [in]     role Specifies what kind of data is wanted
	 *
	 *      @return QVariant containing the data
	*/
	virtual QVariant data(const QModelIndex& index, 
		int role = Qt::DisplayRole ) const;

	/*! Get the data for the headers
	 *
	 *      @param [in] section         The column that's header is wanted
	 *      @param [in] orientation     Only Qt::Horizontal is supported
	 *      @param [in] role            Specified the type of data that is wanted.
	 *
	 *      @return QVariant containing the data to be displayed.
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*! Set the data for specified item.
	 *
	 *      @param [in] index   Specifies the item that's data is modified
	 *      @param [in] value   The value to be set.
	 *      @param [in] role    The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return True if data was successfully set.
	 */
	virtual bool setData(QModelIndex const& index, QVariant const& value, int role = Qt::EditRole);


	/*! Get information on how specified item can be handled.
	 *
	 *      @param [in]     index Specifies the item that's flags are wanted.
	 *
	 *      @return Qt::ItemFlags that define how object can be handled.
	*/
	virtual Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! Check if the model is in valid state or not.
	 *
	 *      @return True if all items in model are valid.
	*/
	bool isValid() const;

	/*! Get the index of the identified port.
	 *
	 *      @param [in] portName    Identifies the port.
	 *
	 *      @return QModelIndex of first column of the specified port. Invalid index if named port is not found. 
	*/
	virtual QModelIndex index(QSharedPointer<Port> port) const;
   
    /*!
     *  Sets the edited model and locks all current ports.
     *
     *      @param [in] model   The model whose ports to edit.
     */
    void setModelAndLockCurrentPorts(QSharedPointer<Model> model);

protected:

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      return      True, if column can have expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of target data.
     *
     *      return      Expression in the given index, or plain value.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in the cell given by the column.
     *
     *      @param [in] index   The index being validated.
     *
     *      return      True, if the data in the parameter is valid, false otherwise.
     */
    virtual bool validateColumnForParameter(QModelIndex const& index) const;

    /*!
     *  Get all the references made to the selected parameter from the selected row.
     *
     *      @param [in] row         The selected row.
     *      @param [in] valueID     The target parameter.
     *
     *      @return Number of references made to the selected id from the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const;

public slots:

	/*! Remove a row from the model
	 *
	 *      @param [in] row Specifies the row to remove
	*/
	void onRemoveRow(int row);
    

	//! Add a new empty row to the model
	void onAddRow();

	/*! A new item should be added to given index.
	 *
	 *      @param [in] index   The position where new item should be added at.
	*/
	void onAddItem(const QModelIndex& index);

	/*! An item should be removed from the model.
	 * 
	 *      @param [in] index   Identifies the item that should be removed.
	*/
	void onRemoveItem(const QModelIndex& index);

	/*! Add a new port to the model.
	 *
	 *      @param [in] port    The port to be added to the model.
	*/
	virtual void addPort(QSharedPointer<Port> port);

signals:

	//! Emitted when contents of the model change
	void contentChanged();

	//! Prints an error message to the user.
	void errorMessage(const QString& msg) const;
	
	//! Prints a notification to user.
	void noticeMessage(const QString& msg) const;

private:
	
	//! No copying
	PortsModel(const PortsModel& other);

	//! No assignment
	PortsModel& operator=(const PortsModel& other);
 
    /*!
     *  Gets the port in a given row.
     *
     *      @param [in] row   The row to search for.
     *
     *      @return The port on the given row.
     */
    QSharedPointer<Port> portOnRow(int row) const;

    /*!
     *   Locks the name, direction  and type columns of a port.
     *
     *      @param [in] modelParam  The parameter model to lock.
     */
    void lockPort(QSharedPointer<Port> port);

    /*!
     *   Unlocks the name, direction and type columns of a port.
     *
     *      @param [in] modelParam  The parameter model to lock.
     */
    void unlockPort(QSharedPointer<Port> port);

    /*!
     *   Locks the given index disabling editing.
     *
     *      @param [in] index   The index to lock.
     */
    void lockIndex(QModelIndex const& index);

  /*!
     *   Unlocks the given index disabling editing.
     *
     *      @param [in] index   The index to unlock.
     */
    void unlockIndex(QModelIndex const& index);

    /*!
     *   Checks if given index is locked.
     *
     *      @param [in] index   The index to check.
	 *
	 *      @return True if the index is locked, otherwise false.
     */
    bool isLocked(QModelIndex const& index) const;

    /*!
     *   Checks if given row is locked.
     *
     *      @param [in] row   The row to check.
	 *
	 *      @return True if the row is locked, otherwise false.
     */
    bool rowIsLocked(int row);

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of target data.
     *
     *      return      The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Changes the type name and definition on a row according to the port width.
     *
     *      @param [in] port    The port whose type name and definition to update.
     *      @param [in] row     The row number of the port.
     */
    void setTypeNameAndDefinitionOnRow(QSharedPointer<Port> port, int row);

    /*!
     *  Checks if a given port has an expression as a left or right bound.
     *
     *      @param [in] port   The port to check.
     *
     *      @return True, if left or right bound is an expression, otherwise false.
     */
    bool hasExpressionInLeftOrRightBound(QSharedPointer<Port> port) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Pointer to the model being edited.
    QSharedPointer<Model> model_;

    //! The locked indexes that cannot be edited.
    QList<QPersistentModelIndex> lockedIndexes_;

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! Expression formatter, formats the referencing expressions to show parameter names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // PORTSMODEL_H
