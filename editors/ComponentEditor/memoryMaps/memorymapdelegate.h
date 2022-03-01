//-----------------------------------------------------------------------------
// File: memorymapdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.08.2012
//
// Description:
// The delegate that provides editors to add/remove/edit details of a single memory map.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPDELEGATE_H
#define MEMORYMAPDELEGATE_H

#include <editors/ComponentEditor/memoryMaps/MemoryBlockDelegate.h>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to add/remove/edit details of a single memory map.
//-----------------------------------------------------------------------------
class MemoryMapDelegate : public MemoryBlockDelegate
{
	Q_OBJECT

public:
	
	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterNameCompleter  The completer to use for parameter names in expression editor.
	 *      @param [in] parameterFinder         The parameter finder for expression editor.
	 *      @param [in] parent                  Pointer to the owner of the delegate.
	 */
	MemoryMapDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);
	
	/*!
     *  The destructor.
     */
	virtual ~MemoryMapDelegate() = default;

	/*!
     *  Create a new editor for the given item
	 *
	 *      @param [in] parent Owner for the editor.
	 *      @param [in] option Contains options for the editor.
	 *      @param [in] index Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor Pointer to the editor where the data is to be set.
	 *      @param [in] index Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor Pointer to the editor that contains the data to store.
	 *      @param [in] model Model that contains the data structure where data is to be saved to.
	 *      @param [in] index Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column  The column to check.
     *
     *      @return True if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    /*!
     *  Gets the description column.
     */
    virtual int descriptionColumn() const;

private:
	
    //! No copying.	No assignment.
	MemoryMapDelegate(const MemoryMapDelegate& other);
	MemoryMapDelegate& operator=(const MemoryMapDelegate& other);

    /*!
     *  Get the column for block name.
     *
     *      @return Index of the name column.
     */
    virtual int nameColumn() const override final;

    /*!
     *  Get the column for block base address.
     *
     *      @return Index of the base address column.
     */
    virtual int baseAddressColumn() const override final;

    /*!
     *  Get the column for block is present.
     *
     *      @return Index of the is present column.
     */
    virtual int isPresentColumn() const override final;
};

#endif // MEMORYMAPDELEGATE_H
