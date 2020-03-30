//-----------------------------------------------------------------------------
// File: AbstractParameterModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 18.11.2014
//
// Description:
// Base class for models editing parameters and model parameters.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTPARAMETERMODEL_H
#define ABSTRACTPARAMETERMODEL_H

#include "ParameterizableTable.h"
#include "ReferencingTableModel.h"

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class ParametersInterface;

//-----------------------------------------------------------------------------
//! Base class for models editing parameters and model parameters.
//-----------------------------------------------------------------------------
class AbstractParameterModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] parameterInterface  Interface for accessing parameters.
     *      @param [in] expressionParser    Expression parser for configurable elements.
	 *      @param [in] parameterFinder     Pointer to the instance for finding parameters.
	 *      @param [in] parent              The parent object.
	 */
    AbstractParameterModel(QSharedPointer<ParametersInterface> parameterInterface,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterFinder> parameterFinder, QObject *parent);

    /*!
     *  The destructor.
     */
	virtual ~AbstractParameterModel() = default;

    /*!
     *  Get the number of rows in the model.
	 *
	 *      @param [in]  parent Model index of the parent of the item.
	 *
	 *      @return  Number of rows currently in the model.
	 */
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*!
     *  Get the data for the specified item for specified role.
	 *
	 *      @param [in]  index  Identifies the item that's data is wanted.
	 *      @param [in]  role   Specifies what kind of data is wanted.
	 *
	 *      @return  The data for the given index.
     */
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole ) const;

	/*!
     *  Get the data for the headers.
	 *
	 *      @param [in]  section        The column that's header is wanted.
	 *      @param [in]  orientation    The orientation of the header data.
	 *      @param [in]  role           Specified the type of data that is wanted.
	 *
	 *      @return  The header data for the given section.
     */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;

	/*!
     *  Set the data for specified item.
	 *
	 *      @param [in]  index  Specifies the item that's data is modified
	 *      @param [in]  value  The value to be set.
	 *      @param [in]  role   The role that is trying to modify the data. Only Qt::EditRole is supported.
	 *
	 *      @return  True, if data was successfully set, otherwise false.
     */
	virtual bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole );

	/*!
     *  Get information on how specified item can be handled.
	 *
	 *      @param [in]  index Specifies the item that's flags are wanted.
	 *
	 *      @return  Qt::ItemFlags that define how object can be handled.
     */
	virtual Qt::ItemFlags flags(QModelIndex const& index) const;

public slots:

	/*!
     *  A new item should be added to given index.
	 *
	 *      @param [in]  index The position where new item should be added at.
     */
	virtual void onAddItem(QModelIndex const& index) = 0;

	/*!
     *  An item should be removed from the model.
	 * 
	 *      @param [in]  index Identifies the item that should be removed.
     */
	virtual void onRemoveItem(QModelIndex const& index) = 0;

    /*!
     *  Handles calculation of references to the indexed parameters.
     *
     *      @param [in] indexes     The selected indexes.
     */
    virtual void onGetParametersMachingIndexes(QModelIndexList indexes);

signals:

	/*!
     *  Emitted when contents of the model change.
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

    /*!
     *  Recalculate references made to the selected parameters.
     *
     *      @param [in] parameterList       The selected parameters.
     *      @param [in] parameterInterface  Interface for accessing parameters.
     */
    void recalculateReferencesToParameters(QVector<QString> const& parameterList,
        QSharedPointer<ParametersInterface> parameterInterface);

protected:
                  
    /*!
     *  Get the interface for accessing parameters.
     *
     *      @return Interface for accessing parameters.
     */
    QSharedPointer<ParametersInterface> getInterface() const;

    /*!
     *  Gets the column for name.
     *
     *      @return The column for editing name selection.
     */
    virtual int nameColumn() const = 0;
        
    /*!
     *  Gets the column for display name.
     *
     *      @return The column for editing display name selection.
     */
    virtual int displayNameColumn() const = 0;

    /*!
     *  Gets the column for value type.
     *
     *      @return The column for editing type selection.
     */
    virtual int typeColumn() const = 0;
        
    /*!
     *  Gets the column for the left value of bit width vector.
     *
     *      @return The column for editing the left value of the bit width vector.
     */
    virtual int bitWidthLeftColumn() const = 0;

    /*!
     *  Gets the column for the right value of bit width vector.
     *
     *      @return The column for editing the right value of the bit width vector.
     */
    virtual int bitWidthRightColumn() const = 0;

    /*!
     *  Gets the column for minimum value.
     *
     *      @return The column for editing the minimum value.
     */
    virtual int minimumColumn() const = 0;
        
    /*!
     *  Gets the column for maximum value.
     *
     *      @return The column for editing the maximum value.
     */
    virtual int maximumColumn() const = 0;
        
    /*!
     *  Gets the column for choices.
     *
     *      @return The column for editing choice selection.
     */
    virtual int choiceColumn() const = 0;

    /*!
     *  Gets the column for values.
     *
     *      @return The column for editing value selection.
     */
    virtual int valueColumn() const = 0;
    
    /*!
     *  Gets the column for resolve.
     *
     *      @return The column for editing value selection.
     */
    virtual int resolveColumn() const = 0;
    
    /*!
     *  Gets the column for array left.
     *
     *      @return The column for editing array left side.
     */
    virtual int arrayLeftColumn() const = 0;  

    /*!
     *  Gets the column for array right.
     *
     *      @return The column for editing array right side.
     */
    virtual int arrayRightColumn() const = 0;

    /*!
     *  Gets the column for description.
     *
     *      @return The column for editing description.
     */
    virtual int descriptionColumn() const = 0;

    /*!
     *  Gets the column index for parameter id.
     *
     *      @return     The column index for parameter id.
     */
    virtual int idColumn() const = 0;

    /*!
     *  Gets the column index for usage count.
     *
     *      @return     The column index for usage count.
     */
    virtual int usageCountColumn() const = 0;

    /*!
     *  Checks if the given column is mandatory.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the column is mandatory, otherwise false.
     */
    virtual bool isMandatoryColumn(int column) const;

    /*!
     *  Validates the data in a parameter corresponding to a given column.
     *
     *      @param [in] column      The column whose data to validate.
     *      @param [in] parameter   The parameter whose data to validate.
     *
     *      @return True, if the data in the parameter is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return     True, if column can have expressions, false otherwise.
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
     *  Gets all the references to id from the register on the selected row.
     *
     *      @param [in] row         The row of the selected register.
     *      @param [in] valueID     The id of the target parameter.
     *
     *      @return The amount of references made to the selected parameter.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const;

    /*!
     *  Check if a parameter at row can be removed.
     *
     *      @param [in] row     The row of the parameter.
     *
     *      @return True if the parameter can be removed, false otherwise.
     */
    bool canRemoveRow(int const& row) const;

private:

	//! No copying.
	AbstractParameterModel(const AbstractParameterModel& other);

	//! No assignment.
	AbstractParameterModel& operator=(const AbstractParameterModel& other);
    
    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The formatted value of an expression in the selected index.
     */
    virtual QVariant formattedExpressionForIndex(QModelIndex const& index) const;

    /*!
     *  Get the expression of the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The expression of the selected index.
     */
    virtual QVariant expressionForIndex(QModelIndex const& index) const;

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index whose data to get.
     *
     *      @return The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Gets the background color for the given index.
     *
     *      @param [in] index   The index whose background color to get.
     *
     *      @return The color for the background.
     */
    QVariant backgroundColorForIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing parameters.
    QSharedPointer<ParametersInterface> parameterInterface_;
};

#endif // ABSTRACTPARAMETERMODEL_H
