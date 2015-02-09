/* 
 *  	Created on: 25.8.2012
 *      Author: Antti Kamppi
 * 		filename: registertablemodel.h
 *		Project: Kactus 2
 */

#ifndef REGISTERTABLEMODEL_H
#define REGISTERTABLEMODEL_H

#include <IPXACTmodels/register.h>
#include <IPXACTmodels/field.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

class Choice;
/*! \brief The model to manage the details of a single register.
 *
 */
class RegisterTableModel : public ParameterizableTable
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] reg                     Pointer to the register being edited.
	 *      @param [in] componentChoices        Choices in the containing component.
	 *      @param [in] expressionParser        Pointer to the expression parser.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the owner of the model.
	 */
	RegisterTableModel(QSharedPointer<Register> reg,
        QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices, 
        QSharedPointer <ExpressionParser> expressionParser,
        QSharedPointer <ParameterFinder> parameterFinder,
        QSharedPointer <ExpressionFormatter> expressionFormatter,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~RegisterTableModel();

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

	/*! \brief Check if the register table model is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

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
    virtual bool validateColumnForParameter(QModelIndex const& index) const;

    /*!
     *  Gets the number of all the references made on the selected row to the selected parameter.
     *
     *      @param [in] row         The row of the selected item.
     *      @param [in] valueID     The selected parameter.
     *
     *      @return The amount of references made on the selected row to the selected parameter.
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

signals:

	//! \brief Emitted when the contents of the model change.
	void contentChanged();

	//! \brief Emitted when a new field is added to the given index.
	void fieldAdded(int index);

	//! \brief Emitted when a field is removed from the given index.
	void fieldRemoved(int index);

private:
	
	//! \brief No copying
	RegisterTableModel(const RegisterTableModel& other);

	//! \brief No assignment
	RegisterTableModel& operator=(const RegisterTableModel& other);

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of target data.
     *
     *      @return     The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

	//! \brief Pointer to the register being edited.
	QSharedPointer<Register> reg_;

    //! The choices available in the containing component;
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices_;

	//! \brief Contains the fields being edited.
	QList<QSharedPointer<Field> >& fields_;

    //! Expression formatter, formats the referencing expression to show parameter names.
    QSharedPointer <ExpressionFormatter> expressionFormatter_;
};

#endif // REGISTERTABLEMODEL_H
