//-----------------------------------------------------------------------------
// File: registerfilemodel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// The model to manage the register files of a single address block or another register file.
//-----------------------------------------------------------------------------

#ifndef REGISTERFILEMODEL_H
#define REGISTERFILEMODEL_H

#include <IPXACTmodels/Component/RegisterFile.h>
#include <IPXACTmodels/Component/RegisterBase.h>

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ReferencingTableModel.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ParameterFinder.h>

#include <QSharedPointer>

class Choice;
class Register;
class RegisterFile;
class RegisterValidator;
class RegisterFileValidator;
class RegisterExpressionsGatherer;
class RegisterFileExpressionsGatherer;
class ReferenceCalculator;

//-----------------------------------------------------------------------------
//! The model to manage the register files of a single address block or another register file.
//-----------------------------------------------------------------------------
class RegisterFileModel : public ReferencingTableModel, public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] registerData            The register data containing the register files to edit.
	 *      @param [in] expressionParser        The expression parser.
     *      @param [in] parameterFinder         The parameter finder.
	 *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] registerValidator       Validator for registers.
	 *      @param [in] parent                  The owner of the model.
	 */
	RegisterFileModel(QSharedPointer<QList<QSharedPointer<RegisterBase> > >  registerData,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<RegisterFileValidator> registerFileValidator,
		QObject *parent);

	//! The destructor.
	virtual ~RegisterFileModel() = default;

    //! No copying.
    RegisterFileModel(const RegisterFileModel& other) = delete;

    //! No assignment.
    RegisterFileModel& operator=(const RegisterFileModel& other) = delete;

	/*!
     *  Get the number of rows an item contains.
	 *
	 *      @param [in] parent  Identifies the parent that's row count is requested.
	 *
	 *      @return Number of rows the item has.
	 */
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*!
     *  Get the number of displayed columns.
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
     *  Check if the column is valid for containing expressions.
     *
     *      @param [in] index   The index of the column.
     *
     *      @return     True, if the column is valid for expressions, otherwise false.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of target data.
     *
     *      @return     Expression or plain value for the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  Validates the data in the index.
     *
     *      @param [in] index   The index of target data.
     *
     *      @return     True, if the data is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const;

    /*!
     *  Gets all the references to the selected id from the selected register.
     *
     *      @param [in] row         The row of the selected register.
     *      @param [in] valueID     The id of the referenced parameter.
     *
     *      @return The amount of references made to the target parameter.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString const& valueID) const;

public slots:

	/*!
     *  Add a new item to the given index.
	 *
	 *      @param [in] index   The index identifying the position for new item.
	 */
    void onAddItem(QModelIndex const& index);

	/*!
     *  Remove the item in the given index.
	 *
	 *      @param [in] index   The index identifying the item to remove.
	 */
	virtual void onRemoveItem(const QModelIndex& index);

    /*!
     *  Change the value for address unit bits.
     *
     *      @param [in] newAddressUnitbits  The new value for address unit bits.
     */
    void addressUnitBitsChanged(int newAddressUnitbits);

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
     *  Informs of a need to redraw the visualizer.
     */
    void graphicsChanged(int index);

    //! Prints an error message to the user.
    void errorMessage(const QString& msg) const;

    //! Emitted when a new register item is added to the given index.
    void itemAdded(int index);

    //! Emitted when a register item is removed from the given index.
    void itemRemoved(int index);

    void childAddressingChanged(int index);

private:

    /*!
     *  Get the value for the corresponding index.
     *
     *      @param [in] index   The index whose value is being searched for.
     */
    QVariant valueForIndex(const QModelIndex& index) const;

    /*!
     *  Decrease the number of references made from a removed register.
     *
     *      @param [in] removedRegister     The removed register.
     */
    void decreaseReferencesWithRemovedRegister(QSharedPointer<RegisterBase> removedRegister);

    /*!
     *  Get all the names of the registers and register files in register data.
     *
     *      @return The names of the registers and register files.
     */
    QStringList getAllNames() const;

    /*!
     *  Increase the number of references made in the copied register.
     *
     *      @param [in] pastedRegisterFile      The copied register file.
     *      @param [in] gatherer                Register expressions gatherer.
     *      @param [in] referenceCalculator     The reference calculator.
     */
    void increaseReferencesInPastedRegisterFile(QSharedPointer<RegisterFile> pastedRegisterFile,
        RegisterFileExpressionsGatherer& gatherer, ReferenceCalculator& referenceCalculator);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Contains all registers and register files.
    QSharedPointer<QList<QSharedPointer<RegisterBase> > > registerData_;

	//! Contains the register file items to display in this model.
    QList<QSharedPointer<RegisterFile> >  items_;

    //! The address unit bits of the memory map.
    unsigned int addressUnitBits_;

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! Expression formatter, formats the referencing expressions to show parameter names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! The validator used for registers.
    QSharedPointer<RegisterFileValidator> registerFileValidator_;

};

#endif // REGISTERFILEMODEL_H
