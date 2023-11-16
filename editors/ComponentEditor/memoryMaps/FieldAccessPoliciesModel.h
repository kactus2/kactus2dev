//-----------------------------------------------------------------------------
// File: FieldAccessPoliciesModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Model to manage the field access policies of a field.
//-----------------------------------------------------------------------------

#ifndef FIELDACCESSPOLICIESMODEL_H
#define FIELDACCESSPOLICIESMODEL_H

#include <KactusAPI/include/FieldInterface.h>

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>

class FieldAccessPoliciesModel : public ReferencingTableModel, public ParameterizableTable
{
    Q_OBJECT

public:
        
    /*!
     *	The constructor.
     *  
     *      @param [in] fieldName           The name of the field being edited.
     *      @param [in] parameterFinder     Parameter finder to use.
     *      @param [in] fieldInterface      Field interface to access field data.
     *      @param [in] expressionParser    Expression parser to use.
     *      @param [in] parent              Parent object.
     */
    FieldAccessPoliciesModel(QString const& fieldName, QSharedPointer<ParameterFinder> parameterFinder, 
        FieldInterface* fieldInterface, QSharedPointer<ExpressionParser> expressionParser, QObject* parent);

    virtual ~FieldAccessPoliciesModel() = default;

    FieldAccessPoliciesModel(FieldAccessPoliciesModel& other) = delete;
    FieldAccessPoliciesModel& operator=(FieldAccessPoliciesModel& other) = delete;

    /*!
     *  Get the number of access policies the model contains.
     *
     *      @param [in] parent  Identifies the parent whose row count is requested.
     *
     *      @return Number of rows the model has.
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the number of columns the model has to be displayed.
     *
     *      @param [in] parent  Identifies the parent whose column count is requested.
     *
     *      @return The number of columns to be displayed.
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /*!
     *  Get the item flags that defines the possible operations for the item.
     *
     *      @param [in] index   Model index that identifies the item.
     *
     *      @return Qt::ItemFlags specifying the possible operations for the item.
     */
    Qt::ItemFlags flags(const QModelIndex& index) const;

    /*!
     *  Get the header data for specified header.
     *
     *      @param [in] section         The section specifies the row/column number for the header.
     *      @param [in] orientation     Specified if horizontal or vertical header is wanted.
     *      @param [in] role            Specifies the type of the requested data.
     *
     *      @return QVariant containing the requested data.
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    /*!
     *  Get the data for specified item.
     *
     *      @param [in] index   Specifies the item that's data is requested.
     *      @param [in] role    The role that defines what kind of data is requested.
     *
     *      @return QVariant containing the data for the item.
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

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

    /*!
     *  Get the list of acceptable mime types.
     *
     *      @return The list of acceptable mime types.
     */
    virtual QStringList mimeTypes() const;

public slots:

    /*!
     *	Handler for adding a new field access policy.
     *  
     *      @param [in] index     The model index of the item that was selected.
     */
    void onAddRow(QModelIndex const& index);

    /*!
     *	Handler for removing a field access policy.
     *  
     *      @param [in] index     The model index for the item/row to be removed.
     */
    void onRemoveItem(QModelIndex const& index);

    /*!
     *  Copy the items in the selected rows.
     *
     *      @param [in] indices   List of indexes pointing to the selected rows.
     */
    void onCopyRows(QModelIndexList const& indices);

    /*!
     *  Paste the copied items.
     */
    void onPasteRows();

signals:

    /*!
     *	Emitted when there has been a change in the number of rows.
     */
    void invalidateFilter();

    /*!
     *	Emitted whenever a field access policy has been edited.
     */
    void contentChanged();

protected:

    /*!
     *  Get all the references made to the selected parameter from the selected row.
     *
     *      @param [in] row         The selected row.
     *      @param [in] valueID     The target parameter.
     *
     *      @return Number of references made to the selected id from the selected row.
     */
    int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const final;

    /*!
     *  Check if the column index is valid for containing expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      return True, if column can have expressions, false otherwise.
     */
    bool isValidExpressionColumn(QModelIndex const& index) const final;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of target data.
     *
     *      return      Expression in the given index, or plain value.
     */
    QVariant expressionOrValueForIndex(QModelIndex const& index) const final;

    /*!
     *  Validates the data in the cell given by the column.
     *
     *      @param [in] index   The index being validated.
     *
     *      return      True, if the data in the parameter is valid, false otherwise.
     */
    bool validateIndex(QModelIndex const& index) const override;

private:

    QVariant valueForIndex(QModelIndex const& index) const;

    QVariant expressionForIndex(QModelIndex const& index) const;

    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The formatted value of an expression in the selected index.
     */
    QVariant formattedExpressionForIndex(QModelIndex const& index) const;

    /*!
     *	Increase the references of the parameters in a field access policy given by row, when copy pasting a
     *  the field access policy.
     *  
     *      @param [in] row     Description
     */
    void increaseReferencesInFieldAccessPolicy(int const& row);

    //! The field interface to use.
    FieldInterface* fieldInterface_;

    //! The name of the current field.
    std::string fieldName_;
};


#endif // FIELDACCESSPOLICIESMODEL_H
