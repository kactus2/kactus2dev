//-----------------------------------------------------------------------------
// File: ParameterArrayModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.03.2015
//
// Description:
// Model for editing an array within parameter value.
//-----------------------------------------------------------------------------

#ifndef PARAMETERARRAYMODEL_H
#define PARAMETERARRAYMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/validators/ParameterValidator2014.h>
#include <IPXACTmodels/choice.h>

#include <QSharedPointer>
#include <QList>
#include <QStringList>
#include <QColor>

//-----------------------------------------------------------------------------
// class ParameterArrayModel.
//-----------------------------------------------------------------------------
class ParameterArrayModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] sizeOfArray             Size of the array to be constructed.
     *      @param [in] expressionParser        Pointer to the expression parser.
     *      @param [in] parameterFinder         Pointer to parameter finder.
     *      @param [in] expressionFormatter     Pointer to the expression formatter.
     *      @param [in] selectedChoice          The currently active choice.
     *      @param [in] valueBackgroundColor    The colour for the background of value column.
     *      @param [in] arrayStartIndex         The starting index of the array.
     *      @param [in] parent                  The parent of this model.
     */
    ParameterArrayModel(int sizeOfArray,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<Choice> selectedChoice,
        QColor valueBackGroundColor,
        int arrayStartIndex,
        QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~ParameterArrayModel();

    /*!
     *  Returns the amount of columns in the model.
     *
     *      @param [in] parent  If given, the column count of the children of the given parent is returned.
     *
     *      @return The amount of columns.
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    /*!
     *  Returns the amount of rows in the model.
     *
     *      @param [in] parent  If given, the row count of the children of the given parent is returned.
     *
     *      @return The amount of rows.
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /*!
     *  Retrieves the header data for the given column (row headers are not supported in this model).
     *
     *      @param [in] section         The column from which the index is retrieved.
     *      @param [in] orientation     Whether the header is horizontal or vertical.
     *      @param [in] role            The role for which the header is retrieved.
     *
     *      @return The header data.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Retrieves the data for the appropriate role at the given index.
     *
     *      @param [in] index   The index of data to be returned.
     *      @param [in] role    The role for which data is returned.
     *
     *      @return The requested data.
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    /*!
     *  Sets the data at the given index.
     *
     *      @param [in] index   The index at which the data is set.
     *      @param [in] value   The new value to being set.
     *      @param [in] role    The role for which the data is set.
     *
     *      @return True, if data was set successfully, otherwise false.
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    /*!
     *  Returns the flags for an object at the given index.
     *
     *      @param [in] index   The index of the object queried.
     *
     *      @return The relevant flags.
     */
    Qt::ItemFlags flags(QModelIndex const& index) const;

    /*!
     *  Set the data for the array.
     *
     *      @param [in] arrayData   The data to be set for the array.
     */
    void setArrayData(QString const& arrayData);

    /*!
     *  Gets the data of the array.
     *
     *      @return The data in the array, in one string separated by ',' and in between {}.
     */
    QString getArrayData();

    /*!
     *  Set the type for the values in the array.
     *
     *      @param [in] parameterType   The new type for the parameters.
     */
    void setParameterType(QString const& parameterType);

signals:
    //! Emitted when the model content has changed.
    void contentChanged();

protected:

    /*!
     *  Checks if the column is valid for expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return True, if the column is valid for expressions, otherwise false.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index or the plain value if expression is not available.
     *
     *      @param [in] index   The index of the value to get.
     *
     *      @return The expression for the index if available, otherwise the value for the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  Gets all the references to an id from the selected array index.
     *
     *      @param [in] row         The index of the selected array value.
     *      @param [in] valueID     The parameter whose references are being searched for.
     *
     *      @return The amount of references made to the selected parameter.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const;

    /*!
     *  Validates the data in the array at index corresponding to the given column.
     *
     *      @param [in] index   The index of the array value.
     *
     *      @return True, if the value is valid for the column.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

private:
	// Disable copying.
    ParameterArrayModel(ParameterArrayModel const& rhs);

	// Disable assignment.
    ParameterArrayModel& operator=(ParameterArrayModel const& rhs);

    /*!
     *  Check if the index is valid.
     *
     *      @param [in] index   The index being checked.
     *
     *      @return True if the index is not valid, otherwise false.
     */
    bool indexIsNotValid(QModelIndex const& index) const;

    /*!
     *  Gets the value of the array in the given index.
     *
     *      @param [in] index   The index of the value.
     *
     *      @return The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Make the values in the array match the values of the choice.
     *
     *      @param [in] row     The index of the value to be matched to choice.
     *
     *      @return The value of the choice corresponding to the given value.
     */
    QString evaluateValueWithChoice(const int& row) const;

    //! Size of the array.
    int sizeOfArray_;

    //! The expression formatter.
    QSharedPointer<ExpressionFormatter> expressionformatter_;

    //! The values of the array.
    QStringList arrayValues_;

    //! The currently active choice.
    QSharedPointer<Choice> selectedChoice_;

    //! The validator for the array values.
    ParameterValidator2014* validator_;

    //! The parameter type.
    QString parameterType_;

    //! The colour for the background of the value column.
    QColor valueBackGroundColor_;

    //! The starting index of the array.
    int arrayStartIndex_;
};
#endif // PARAMETERARRAYMODEL_H
