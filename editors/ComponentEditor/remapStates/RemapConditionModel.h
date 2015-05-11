//-----------------------------------------------------------------------------
// File: RemapConditionModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 09.04.2015
//
// Description:
// Table model that displays editable remap conditions.
//-----------------------------------------------------------------------------

#ifndef REMAPCONDITIONMODEL_H
#define REMAPCONDITIONMODEL_H

#include <IPXACTmodels/remapport.h>

#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <editors/ComponentEditor/common/ReferencingTableModel.h>
#include <editors/ComponentEditor/common/ParameterizableTable.h>

#include <IPXACTmodels/port.h>

#include <QAbstractTableModel>
#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Table model that displays editable remap conditions.
//-----------------------------------------------------------------------------
class RemapConditionModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *     @param [in] remapPorts              The remap ports of the remap state.
     *     @param [in] componentPorts          The ports of the component.
     *     @param [in] expressionParser        Expression parse for configurable elements.
     *     @param [in] parameterFinder         The parameter finder.
     *     @param [in] expressionFormatter     Changes the referenced ids to names.
     *     @param [in] parent                  The owner of this model.
     */
    RemapConditionModel(QSharedPointer<QList<QSharedPointer<RemapPort> > > remapPorts,
        QList<QSharedPointer<Port> > componentPorts,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QObject* parent);

    /*!
     *  The destructor.
     */
    virtual ~RemapConditionModel();

  	/*!
	 *  Get the number of rows in the model.
	 *
	 *     @param [in] parent  Index of the parent of the item.
     *
     *     @return Number of rows currently in the model.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*! Get the number of columns in the model
	 *
	 *     @param [in] parent  Index of the parent of the item.
	 *
	 *     @return Number of columns currently in the model.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

    /*!
     *  Get the data for the specified item for the specified role.
     *
     *     @param [in] index   The index of the item.
     *     @param [in] role    Specifies what kind of data is wanted.
     *
     *     @return The data for the given index.
     */
    virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

    /*!
     *  Get the data for the headers.
     *
     *     @param [in] section         The column of the header.
     *     @param [in] orientation     The orientation of the header data.
     *     @param [in] role            Specifies the wanted data role.
     *
     *     @return The header data for the given section.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Sets the data for the given index.
     *
     *     @param [in] index   The index of the item being modified.
     *     @param [in] value   The new value for the item.
     *     @param [in] role    The role for editing the data.
     *
     *     @return True, if data was successfully set, false otherwise.
     */
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /*!
     *  Get information on how specified item can be handled.
     *
     *     @param [in] index   The index of the item.
     *
     *     @return Qt::ItemFlags determining the handling information of the item.
     */
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    /*!
     *  Check if the model is in a valid state.
     *
     *     @return True, if the model is valid, false otherwise.
     */
    virtual bool isValid() const;

    /*!
     *  Update the ports of the model.
     *
     *     @param [in] newPorts    The new ports for the model.
     */
    void updatePorts(QList<QSharedPointer<Port> > newPorts);

    /*!
     *  Get the currently available, non-selected ports.
     *
     *     @return A list of port names.
     */
    QStringList getAvailablePorts();

public slots:

	/*! A new item should be added to given index.
	 *
	 *     @param [in] index   The position where new item should be added at.
	 */
	virtual void onAddItem(QModelIndex const& index);

	/*! An item should be removed from the model.
	 * 
	 *     @param [in] index   Identifies the item that should be removed.
     */
	virtual void onRemoveItem(QModelIndex const& index);

signals:

    /*!
     *  Emitted when the contents of the model change.
     */
    void contentChanged();

    /*!
     *  Passes an error message to the user.
     *
     *     @param [in] msg     The error message.
     */
    void errorMessage(const QString& msg) const;

    /*!
     *  Passes a notification message to the user.
     *
     *     @param [in] msg     The notification message.
     */
    void noticeMessage(const QString& msg) const;

    /*!
     *  Emitted when the list of available ports changes.
     *
     *     @param [in] availablePorts  The new list of available ports.
     */
    void newAvailablePorts(QStringList availablePorts) const;

protected:
    
    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *     @param [in] index   The index being evaluated.
     *
     *     @return True, if the column can accept expressions, false otherwise.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the index, or the plain value, if an expression is not available.
     *
     *     @param [in] index   The index of the item.
     *
     *     @return The expression for the index if available, otherwise the value for the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data for the column.
     *
     *     @param [in] index   The column of the item to validate.
     *
     *     @return True, if the data of the item is valid for the column, false otherwise.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

    /*!
     *  Gets the number of all the references made to a selected id on the selected row.
     *
     *     @param [in] row         The row of the selected item.
     *     @param [in] valueID     The id of the referenced parameter.
     *
     *     @return The amount of references made to the selected id on the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const;

private:

	//! No copying
    RemapConditionModel(const RemapConditionModel& other);

	//! No assignment
   RemapConditionModel& operator=(const RemapConditionModel& other);

   /*!
    *  Setup the remap ports in the remap state to show as visible remap ports.
    */
   void setupVisibleRemapPorts();

   /*!
    *  Check if the remap port is already in the visible remap ports.
    *
    *      @param [in] remapStateRemapPort  The remap port.
    *      @param [in] arrayIndex           The index of the array.
    *
    *       @return True, if the remap port is already visible, false otherwise.
    */
   bool remapPortIsAlreadyVisible(QSharedPointer<RemapPort> remapStateRemapPort, int arrayIndex);

   /*!
    *  Get the end of the array.
    *
    *      @param [in] remapPortName    The name of the remap port.
    *      @param [in] newPortIndex     The index of the remap port.
    *
    *      @return The end of the array value.
    */
   QString getEndOfArray(QString const& remapPortName, int newPortIndex);

   /*!
    *  Gets the value for the given index.
    *
    *      @param [in] index    The index of the data.
    *
    *      @return The data in the given index.
    */
   QVariant valueForIndex(QModelIndex const& index) const;

   /*!
    *  Save the visible remap ports to the remap state remap ports.
    */
   void save();

   /*!
    *  Get the starting index of the array.
    *
    *      @param [in] remapPortName    The name of the remap port.
    *
    *      @return The starting index of the array.
    */
   int getArrayStartIndex(QString const& remapPortName);

   //-----------------------------------------------------------------------------
   // Data.
   //-----------------------------------------------------------------------------

   //! A list of remap ports belonging to the remap state.
   QSharedPointer<QList<QSharedPointer<RemapPort> > > remapPortsOfRemapState_;

   //! A list of remap ports modified to be visible in the model
   QSharedPointer<QList<QSharedPointer<RemapPort> > > remapPortsVisibleInModel_;

   //! The expression formatter.
   QSharedPointer<ExpressionFormatter> expressionFormatter_;

   //! A list of ports in the component.
   QList<QSharedPointer<Port> > componentPorts_;
};

#endif // REMAPCONDITIONMODEL_H
