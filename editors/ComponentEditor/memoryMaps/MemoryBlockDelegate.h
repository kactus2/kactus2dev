//-----------------------------------------------------------------------------
// File: MemoryBlockDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 14.02.2022
//
// Description:
// The delegate that provides editors to add/remove/edit details of memory blocks.
//-----------------------------------------------------------------------------

#ifndef MEMORYBLOCKDELEGATE_H
#define MEMORYBLOCKDELEGATE_H

#include <KactusAPI/include/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionDelegate.h>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to add/remove/edit details of memory blocks.
//-----------------------------------------------------------------------------
class MemoryBlockDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:
	
	/*!
	 *  The constructor.
	 *
     *      @param [in] completionModel         Model containing the completions used in expression editor.
	 *      @param [in] parameterFinder         The parameter finder for expression editor.
	 *      @param [in] parent                  Pointer to the owner of the delegate.
	 */
	MemoryBlockDelegate(QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~MemoryBlockDelegate() = default;

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

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column  The column to check.
     *
     *      @return True if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

private slots:

	/*!
     *  Commit the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
	
    //! No copying.	No assignment.
	MemoryBlockDelegate(const MemoryBlockDelegate& other);
    MemoryBlockDelegate& operator=(const MemoryBlockDelegate& other);

    /*!
     *  Get the column for block name.
     *
     *      @return Index of the name column.
     */
    virtual int nameColumn() const = 0;

    /*!
     *  Get the column for block base address.
     *
     *      @return Index of the base address column.
     */
    virtual int baseAddressColumn() const = 0;

    /*!
     *  Get the column for block is present.
     *
     *      @return Index of the is present column.
     */
    virtual int isPresentColumn() const = 0;

    /*!
     *  Creates an editor for name.
     *
     *      @param [in] parent   The parent widget for the editor.
     *      @param [in] option   The style options for the editor.
     *      @param [in] index    The index for which the editor is created.
     *
     *      @return Editor for editing name.
     */
    QWidget* createNameEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index) const;
};

#endif // MEMORYBLOCKDELEGATE_H
