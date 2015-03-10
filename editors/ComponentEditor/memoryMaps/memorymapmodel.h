/* 
 *  	Created on: 22.8.2012
 *      Author: Antti Kamppi
 * 		filename: memorymapmodel.h
 *		Project: Kactus 2
 */

#ifndef MEMORYMAPMODEL_H
#define MEMORYMAPMODEL_H

#include <editors/ComponentEditor/common/ParameterizableTable.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/memorymap.h>
#include <IPXACTmodels/memorymapitem.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class Choice;

/*! \brief The model to manage the details of a single memory map.
 *
 */
class MemoryMapModel : public ParameterizableTable
{
	Q_OBJECT

public:
	
	/*!
	 *  The constructor.
	 *
	 *      @param [in] memoryMap               Pointer to the memory map being edited.
	 *      @param [in] componentChoices        Choices in the containing component.
	 *      @param [in] expressionParser        Pointer to the expression parser.
	 *      @param [in] parameterFinder         Pointer to the parameter finder.
	 *      @param [in] expressionFormatter     Pointer to the expression formatter.
	 *      @param [in] parent                  Pointer to the owner of this model.
	 */
	MemoryMapModel(QSharedPointer<MemoryMap> memoryMap, 
        QSharedPointer <QList<QSharedPointer<Choice> > > componentChoices,
        QSharedPointer <ExpressionParser> expressionParser,
        QSharedPointer <ParameterFinder> parameterFinder,
        QSharedPointer <ExpressionFormatter> expressionFormatter,
		QObject *parent);
	
	//! \brief The destructor
	virtual ~MemoryMapModel();

	/*! \brief Get the number of rows an item contains.
	 *
	 * \param parent Identifies the parent that's row count is requested.
	 *
	 * \return Number of rows the item has.
	*/
	virtual int rowCount(QModelIndex const& parent = QModelIndex()) const;

	/*! \brief Get the number of columns the item has to be displayed.
	 *
	 * \param parent Identifies the parent that's column count is requested.
	 *
	 * \return The number of columns to be displayed.
	*/
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

	/*! \brief Get the item flags that defines the possible operations for the item.
	 *
	 * \param index Model index that identifies the item.
	 *
	 * \return Qt::ItemFlags specify the possible operations for the item.
	*/
	Qt::ItemFlags flags(QModelIndex const& index) const;

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
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*! \brief Save the data to the model for specified item
	 *
	 * \param index The model index of the item that's data is to be saved.
	 * \param value The data that is to be saved.
	 * \param role The role specifies what kind of data should be saved.
	 *
	 * \return True if saving happened successfully.
	*/
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);

	/*! \brief Check if the memory map model is in a valid state.
	 *
	 * \return bool True if the state is valid and writing is possible.
	*/
	bool isValid() const;

public slots:

	/*! \brief Add a new item to the given index.
	 *
	 * \param index The index identifying the position for new item.
	 *
	*/
	virtual void onAddItem(QModelIndex const& index);

	/*! \brief Remove the item in the given index.
	 *
	 * \param index The index identifying the item to remove.
	 *
	*/
	virtual void onRemoveItem(QModelIndex const& index);

protected:

    /*!
     *  Checks if the column index is valid for expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return True, if the column can have expressions, otherwise false.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const;

    /*!
     *  Gets the expression for the given index, or plain value if there is no expression.
     *
     *      @param [in] index   The index of the target data.
     *
     *      @return Expression or plain value in the given index.
     */
    virtual QVariant expressionOrValueForIndex(QModelIndex const& index) const;

    /*!
     *  validates the data in the column.
     *
     *      @param [in] index   The index being validated.
     *
     *      @return True, if the data is valid, otherwise false.
     */
    virtual bool validateColumnForParameter(QModelIndex const& index) const;

    /*!
     *  Gets the number of all the references made on the selected row to the selected parameter.
     *
     *      @param [in] row         The row of the selected item.
     *      @param [in] valueID     The referenced parameter.
     *
     *      @return The number of references made to the parameter on the selected row.
     */
    virtual int getAllReferencesToIdInItemOnRow(const int& row, QString valueID) const;

signals:

	//! \brief Emitted when the contents of the model change.
	void contentChanged();

	//! \brief Emitted when a new memory map item is added to the given index.
	void itemAdded(int index);

	//! \brief Emitted when a memory map item is removed from the given index.
	void itemRemoved(int index);

private:
	
	//! \brief No copying
	MemoryMapModel(const MemoryMapModel& other);

	//! \brief No assignment
	MemoryMapModel& operator=(const MemoryMapModel& other);

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of the target data.
     *
     *      @return The data in the given index.
     */
    QVariant valueForIndex(QModelIndex const& index) const;

	//! \brief Pointer to the memory map being edited.
	QSharedPointer<MemoryMap> memoryMap_;

    //! The choices available in the containing component;
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices_;

	//! \brief Contains the memory map items being edited.
	QList<QSharedPointer<MemoryMapItem> >& items_;

    QSharedPointer <ExpressionFormatter> expressionFormatter_;
};

#endif // MEMORYMAPMODEL_H
