/* 
 *  	Created on: 24.8.2012
 *      Author: Antti Kamppi
 * 		filename: addressblockmodel.h
 *		Project: Kactus 2
 */

#ifndef ADDRESSBLOCKMODEL_H
#define ADDRESSBLOCKMODEL_H

#include <IPXACTmodels/addressblock.h>
#include <IPXACTmodels/registermodel.h>

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QAbstractTableModel>
#include <QSharedPointer>

class Choice;

/*! \brief The model to manage the details of a single address block.
 *
 */
class AddressBlockModel : public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] addressBlock            Pointer to the address block being edited.
	 *      @param [in] componentChoices        The choices available in the containing component.
	 *      @param [in] expressionParser        Pointer to the expression parser.
     *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the owner of the model.
	 */
	AddressBlockModel(QSharedPointer<AddressBlock> addressBlock,
        QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~AddressBlockModel();

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(const QModelIndex& index) const;

	/*! \brief Get the header data for specified header.
	 *
	 * \param section The section specifies the row/column number for the header.
	 * \param orientation Specified if horizontal or vertical header is wanted.
	 * \param role Specifies the type of the requested data.
	 *
	 * \return QVariant Contains the requested data.
	*/
	virtual QVariant headerData(int section, Qt::Orientation orientation, 
		int role = Qt::DisplayRole) const;

	/*! \brief Get the data for specified item.
	 *
	 * \param index Specifies the item that's data is requested.
	 * \param role The role that defines what kind of data is requested.
	 *
	 * \return QVariant Contains the data for the item.
	*/
	virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

	/*! \brief Save the data to the model for specified item
	 *
	 * \param index The model index of the item that's data is to be saved.
	 * \param value The data that is to be saved.
	 * \param role The role specifies what kind of data should be saved.
	 *
	 * \return True if saving happened successfully.
	*/
	bool setData(const QModelIndex& index, const QVariant& value, 
		int role = Qt::EditRole);

	/*! \brief Check if the address block model is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

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
    virtual bool validateColumnForParameter(QModelIndex const& index) const;

    /*!
     *  Gets all the references to the selected id from the selected register.
     *
     *      @param [in] row         The row of the selected register.
     *      @param [in] valueID     The id of the referenced parameter.
     *
     *      @return The amount of references made to the target parameter.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const;

public slots:

	/*! \brief Add a new item to the given index.
	 *
	 * \param index The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(const QModelIndex& index);

	/*! \brief Remove the item in the given index.
	 *
	 * \param index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(const QModelIndex& index);

    /*!
     *  Change the value for address unit bits.
     *
     *      @param [in] newAddressUnitbits  The new value for address unit bits.
     */
    void addressUnitBitsChanged(int newAddressUnitbits);

signals:

	//! \brief Emitted when the contents of the model change.
	void contentChanged();

    //! \brief Prints an error message to the user.
    void errorMessage(const QString& msg) const;

	//! \brief Emitted when a new register item is added to the given index.
	void itemAdded(int index);

	//! \brief Emitted when a register item is removed from the given index.
	void itemRemoved(int index);

private:
	
	//! \brief No copying
	AddressBlockModel(const AddressBlockModel& other);

	//! \brief No assignment
	AddressBlockModel& operator=(const AddressBlockModel& other);

    /*!
     *  Get the value for the corresponding index.
     *
     *      @param [in] index   The index whose value is being searched for.
     */
    QVariant valueForIndex(const QModelIndex& index) const;

	//! \brief Pointer to the address block being edited.
	QSharedPointer<AddressBlock> addressBlock_;

	//! \brief Contains the register items to display.
	QList<QSharedPointer<RegisterModel> >& items_;

    //! \brief The choices available in the containing component.
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices_;

    //! The address unit bits of the memory map.
    unsigned int addressUnitBits_;

    //! The parameter finder.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! Expression formatter, formats the referencing expressions to show parameter names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // ADDRESSBLOCKMODEL_H
