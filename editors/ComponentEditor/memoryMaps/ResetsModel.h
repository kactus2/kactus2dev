//-----------------------------------------------------------------------------
// File: ResetsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.01.2019
//
// Description:
// The model to manage the resets of a field.
//-----------------------------------------------------------------------------

#ifndef RESETSMODEL_H
#define RESETSMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <QSharedPointer>
#include <QList>

class FieldReset;
class FieldValidator;

//-----------------------------------------------------------------------------
//! The model to manage the resets of a field.
//-----------------------------------------------------------------------------
class ResetsModel : public ReferencingTableModel, public ParameterizableTable
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] resets                  The resets of the selected field.
     *      @param [in] expressionParser        Pointer to the expression parser.
     *      @param [in] parameterFinder         Pointer to the parameter finder.
     *      @param [in] expressionFormatter     Pointer to the expression formatter.
     *      @param [in] fieldValidator          Validator used for fields.
     *      @param [in] parent                  Pointer to the owner of the model.
     */
    ResetsModel(QSharedPointer<QList<QSharedPointer<FieldReset> > > resets,
        QSharedPointer <ExpressionParser> expressionParser, QSharedPointer <ParameterFinder> parameterFinder,
        QSharedPointer <ExpressionFormatter> expressionFormatter, QSharedPointer<FieldValidator> fieldValidator,
        QObject *parent);

    /*!
     *  The destructor.
     */
    virtual ~ResetsModel();

    /*!
     *  Get the number of rows an item contains.
     *
     *      @param [in] parent  Identifies the parent that's row count is requested.
     *
     *      @return Number of rows the item has.
     */
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the number of columns the item has to be displayed.
     *
     *      @param [in] parent  Identifies the parent that's column count is requested.
     *
     *      @return The number of columns to be displayed.
     */
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the item flags that defines the possible operations for the item.
     *
     *      @param [in] index   Model index that identifies the item.
     *
     *      @return Qt::ItemFlags specify the possible operations for the item.
     */
    Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Get the header data for specified header.
     *
     *      @param [in] section         The section specifies the row/column number for the header.
     *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
     *      @param [in] role            Specifies the type of the requested data.
     *
     *      @return QVariant Contains the requested data.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Get the data for specified item.
     *
     *      @param [in] index   Specifies the item that's data is requested.
     *      @param [in] role    The role that defines what kind of data is requested.
     *
     *      @return QVariant Contains the data for the item.
     */
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /*!
     *  Save the data to the model for specified item.
     *
     *      @param [in] index   The model index of the item that's data is to be saved.
     *      @param [in] value   The data that is to be saved.
     *      @param [in] role    The role specifies what kind of data should be saved.
     *
     *      @return True if saving happened successfully.
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

protected:

    /*!
     *  Check if the column index is valid for expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return     True, if column can have expressions, otherwise false.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of target data.
     *
     *      @return     Expression or plain value in the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in the column.
     *
     *      @param [in] index   The index being validated.
     *
     *      @return     True, if the data is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

    /*!
     *  Gets the number of all the references made on the selected row to the selected parameter.
     *
     *      @param [in] row         The row of the selected item.
     *      @param [in] valueID     The selected parameter.
     *
     *      @return The amount of references made on the selected row to the selected parameter.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const;

    public slots:

    /*!
     *  Add a new item to the given index.
     *
     *      @param [in] index   The index identifying the position for new item.
     */
    virtual void onAddItem(const QModelIndex& index);

    /*!
     *  Remove the item in the given index.
     *
     *      @param [in] index   The index identifying the item to remove.
     */
    virtual void onRemoveItem(const QModelIndex& index);

signals:

    /*!
     *  Emitted when the contents of the model change.
     */
    void contentChanged();

private:

    //! No copying. No assignment.
    ResetsModel(const ResetsModel& other);
    ResetsModel& operator=(const ResetsModel& other);

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of target data.
     *
     *      @return     The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The resets being edited.
    QSharedPointer<QList<QSharedPointer<FieldReset> > > resets_;

    //! Expression formatter, formats the referencing expression to show parameter names.
    QSharedPointer <ExpressionFormatter> expressionFormatter_;

    //! The validator used for fields.
    QSharedPointer<FieldValidator> fieldValidator_;
};

#endif // RESETSMODEL_H
