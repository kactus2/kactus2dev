//-----------------------------------------------------------------------------
// File: memorymapsdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 22.08.2012
//
// Description:
// The delegate to provide editor for adding/removing/editing the memory maps of a component.
//-----------------------------------------------------------------------------

#ifndef MEMORYMAPSDELEGATE_H
#define MEMORYMAPSDELEGATE_H

#include <editors/ComponentEditor/common/MultilineDescriptionDelegate.h>
#include <editors/ComponentEditor/common/ExpressionDelegate.h>

//-----------------------------------------------------------------------------
//! The delegate to provide editor for adding/removing/editing the memory maps of a component.
//-----------------------------------------------------------------------------
class MemoryMapsDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:

    /*!
	 *  The constructor.
     *
     *    @param [in] completionModel         Model containing the completions used in expression editor.
     *    @param [in] parameterFinder         The parameter finder for expression editor.
	 *    @param [in] remapStateNames         A list of names of the component remap states.
	 *    @param [in] parent                  Pointer to the owner of the delegate.
	 */
    MemoryMapsDelegate(QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> parameterFinder,
        QStringList remapStateNames, QObject *parent);
	
	//! The destructor.
	virtual ~MemoryMapsDelegate();

    /*!
	 *  Create a new editor for the given item.
	 *
	 *    @param [in] parent  Owner for the editor.
	 *    @param [in] option  Contains options for the editor.
	 *    @param [in] index   Model index identifying the item.
     *
     *    @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
        const;

    /*!
	 *  Set the data for the editor.
	 *
	 *    @param [in] editor  Pointer to the editor where the data is to be set.
	 *    @param [in] index   The index of the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

    /*!
	 *  Save the data from the editor to the model.
	 *
	 *    @param [in] editor  Pointer to the editor that contains the data to store.
	 *    @param [in] model   Model that contains the data structure where data is to be saved to.
	 *    @param [in] index   The inex of the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

    /*!
     *  Update the list of remap state names.
     *
     *    @param [in] newRemapStateNames  The new list of remap state names.
     */
    void updateRemapStateNames(QStringList newRemapStateNames);

    /*!
     *  Paint the memory maps.
     *
     *    @param [in] painter     The used painter.
     *    @param [in] option      Available style options.
     *    @param [in] index       The current index.
     */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:

    //! Gets the description column.
    virtual int descriptionColumn() const;
    
    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *    @param [in] column  The column to check.
     *
     *    @return True if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

private slots:

    /*!
	 *  Commit the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
	
	//! No copying.
	MemoryMapsDelegate(const MemoryMapsDelegate& other);

	//! No assignment.
	MemoryMapsDelegate& operator=(const MemoryMapsDelegate& other);

    //! A list of the components remap state names.
    QStringList remapStateNames_;
};

#endif // MEMORYMAPSDELEGATE_H
