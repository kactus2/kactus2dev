//-----------------------------------------------------------------------------
// File: memorymapmodel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.08.2012
//
// Description:
// The model to manage the address blocks of a single memory map.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPMODEL_H
#define MEMORYMAPMODEL_H

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/memoryMaps/MemoryBlockModel.h>

#include <QAbstractTableModel>
#include <QSharedPointer>
#include <QList>

class AddressBlockInterface;

//-----------------------------------------------------------------------------
//! The model to manage the details of a single memory map.
//-----------------------------------------------------------------------------
class MemoryMapModel : public MemoryBlockModel
{
	Q_OBJECT

public:
	
	/*!
	 *  The constructor.
	 *
     *      @param [in] blockInterface      Interface for address blocks;
	 *      @param [in] expressionParser    Pointer to the expression parser.
	 *      @param [in] parameterFinder     Pointer to the parameter finder.
	 *      @param [in] parent              Pointer to the owner of this model.
	 */
    MemoryMapModel(AddressBlockInterface* blockInterface,
        QSharedPointer <ExpressionParser> expressionParser,
        QSharedPointer <ParameterFinder> parameterFinder,
        QObject *parent);

	/*!
     *  The destructor.
     */
	virtual ~MemoryMapModel() = default;

	/*!
     *  Get the number of columns the item has to be displayed.
	 *
	 *      @param [in] parent  Identifies the parent that's column count is requested.
	 *
	 *      @return The number of columns to be displayed.
	 */
	virtual int columnCount(QModelIndex const& parent = QModelIndex()) const;

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
	virtual QVariant data(QModelIndex const& index, int role = Qt::DisplayRole) const;

	/*!
     *  Save the data to the model for specified item.
	 *
	 *      @param [in] index   The model index of the item that's data is to be saved.
	 *      @param [in] value   The data that is to be saved.
	 *      @param [in] role    The role specifies what kind of data should be saved.
	 *
	 *      @return True if saving happened successfully.
	 */
	bool setData(QModelIndex const& index, const QVariant& value, int role = Qt::EditRole);
    
    /*!
     *  Get the list of acceptable mime types.
     *
     *      @return The list of acceptable mime types.
     */
    virtual QStringList mimeTypes() const override final;

protected:

    /*!
     *  Checks if the column index is valid for expressions.
     *
     *      @param [in] index   The index being evaluated.
     *
     *      @return True, if the column can have expressions, otherwise false.
     */
    virtual bool isValidExpressionColumn(QModelIndex const& index) const override final;

    /*!
     *  validates the data in the column.
     *
     *      @param [in] index   The index being validated.
     *
     *      @return True, if the data is valid, otherwise false.
     */
    virtual bool validateIndex(QModelIndex const& index) const override final;

    /*!
     *  Get the formatted value of an expression in the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The formatted value of an expression in the selected index.
     */
    virtual QVariant formattedExpressionForIndex(QModelIndex const& index) const override final;

    /*!
     *  Get the expression of the selected index.
     *
     *      @param [in] index   The selected index.
     *
     *      @return The expression of the selected index.
     */
    virtual QVariant expressionForIndex(QModelIndex const& index) const override final;

    /*!
     *  Gets the value for the given index.
     *
     *      @param [in] index   The index of the target data.
     *
     *      @return The data in the given index.
     */
    virtual QVariant valueForIndex(QModelIndex const& index) const override final;

signals:

    /*!
     *  Informs of addressing changes in child items.
     *
     *      @param [in]     Index of the selected address block.
     */
    void childAddressingChanged(int);

private:
	
    //! No copying.	No assignment.
	MemoryMapModel(const MemoryMapModel& other);
	MemoryMapModel& operator=(const MemoryMapModel& other);

    /*!
     *  Get the index of the name column.
     *
     *      @return Index of the name column.
     */
    virtual int nameColumn() const override final;

    /*!
     *  Get the index of the base address column.
     *
     *      @return Index of the base address column.
     */
    virtual int baseAddressColumn() const override final;

    /*!
     *  Get the index of the is present column.
     *
     *      @return Index of the is present column.
     */
    virtual int isPresentColumn() const override final;

    /*!
     *  Get the index of the description column.
     *
     *      @return Index of the description column.
     */
    virtual int descriptionColumn() const override final;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for address blocks.
    AddressBlockInterface* localBlockInterface_;
};

#endif // MEMORYMAPMODEL_H
