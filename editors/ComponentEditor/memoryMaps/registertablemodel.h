//-----------------------------------------------------------------------------
// File: registertablemodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 25.08.2012
//
// Description:
// The model to manage the details of a single register.
//-----------------------------------------------------------------------------

#ifndef REGISTERTABLEMODEL_H
#define REGISTERTABLEMODEL_H

#include <IPXACTmodels/Component/Register.h>
#include <IPXACTmodels/Component/Field.h>
#include <IPXACTmodels/common/Document.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>
#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>

class FieldValidator;
class FieldExpressionsGatherer;
class ReferenceCalculator;
class FieldInterface;

//-----------------------------------------------------------------------------
//! The model to manage the details of a single register.
//-----------------------------------------------------------------------------
class RegisterTableModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] fieldInterface      Interface for fields.
	 *      @param [in] expressionParser    Pointer to the expression parser.
	 *      @param [in] parameterFinder     Pointer to the parameter finder.
	 *      @param [in] docRevision         The IP-XACT standard revision in use.
	 *      @param [in] parent              Pointer to the owner of the model.
	 */
    RegisterTableModel(FieldInterface* fieldInterface, QSharedPointer <ExpressionParser> expressionParser, 
        QSharedPointer <ParameterFinder> parameterFinder, Document::Revision docRevision, QObject *parent);


	/*!
     *  The destructor.
     */
	virtual ~RegisterTableModel() = default;

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
    
    /*!
     *  Get the list of acceptable mime types.
     *
     *      @return The list of acceptable mime types.
     */
    virtual QStringList mimeTypes() const;

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

    /*!
     *  Copy the items in the selected rows.
     *
     *      @param [in] indexList   List of indexes pointing to the selected rows.
     */
    void onCopyRows(QModelIndexList indexList);

    /*!
     *  Paste the copied items.
     */
    void onPasteRows();

signals:

	//! Emitted when the contents of the model change.
	void contentChanged();

    /*!
     *  Informs of a need to redraw the visualization.
     */
    void graphicsChanged(int index);

    void addressingChanged(int index);


    /*
     *  Informs of field name change.
     *
     *      @param [in] oldName     The old name.
     *      @param [in] newName     The new name.
     */
    void fieldNameChanged(QString const& oldName, QString const& newName);

	//! Emitted when a new field is added to the given index.
	void fieldAdded(int index);

	//! Emitted when a field is removed from the given index.
	void fieldRemoved(int index);

private:
	
	//! No copying. No assignment.
	RegisterTableModel(const RegisterTableModel& other);
	RegisterTableModel& operator=(const RegisterTableModel& other);

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
     *      @param [in] index   The index of target data.
     *
     *      @return     The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

    /*!
     *  Decrease the number of references when removing a field.
     *
     *      @param [in] fieldName   Name of the selected field.
     */
    void decreaseReferencesWithRemovedField(QString const& fieldName);

    /*!
     *  Increase the number of references made in the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     */
    void increaseReferencesInPastedField(QString const& fieldName);

    /*!
     *  Calculates the parameters used in the selected field.
     *
     *      @param [in] fieldName   Name of the selected field.
     *
     *      @return A map containing pairs of referenced ids and the number of references made to them.
     */
    QMap<QString, int> getReferencedParameters(QString const& fieldName) const;

    /*!
     *	Check if the column of a given index is displaying values stored directly in the field according 
     *  to the 2014 IP-XACT standard revision.
     *  
     *      @param [in] index     Description
     *	    
     * 	    @return True, if the column is a std14 column, otherwise false.
     */
    bool isStd14Column(QModelIndex const& index) const;

    /*!
     *	Get the right index value to display according to which IP-XACT standard is in use. The values for access,
     *  modified write, read action testable and test constraint should be retrieved from the field, if std2014 is
     *  in use. For std2022, the value is none (except for access displaying '[multiple]') if the field has
     *  multiple field access policies. If the field has one field access policy, the values displayed will be 
     *  the values of that field access policy. The values are empty if the field has no field access policies.
     *  
     *      @param [in] index     The index to get the value of.
     *	    
     * 	    @return The index value.
     */
    QVariant getIndexValueByStdRevision(QModelIndex const& index) const;

    /*!
     *	Set the index data for the first field access policy of the field.
     *  
     *      @param [in] index       The model index of the item that's data is to be saved.
     *      @param [in] value       The data that is to be saved.
     *      @param [in] fieldName   The name of the field.
     *	    
     * 	    @return True, if the setting the data succeeded, otherwise false.
     */
    bool setDataForFirstFieldAccessPolicy(QModelIndex const& index, QVariant const& value, std::string const& fieldName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for fields.
    FieldInterface* fieldInterface_;

    //! The IP-XACT standard revision in use.
    Document::Revision docRevision_;
};

#endif // REGISTERTABLEMODEL_H
