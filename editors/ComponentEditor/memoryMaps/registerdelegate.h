//-----------------------------------------------------------------------------
// File: registerdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 25.08.2012
//
// Description:
// The delegate to provide editors to add/remove/edit the fields of register.
//-----------------------------------------------------------------------------

#ifndef REGISTERDELEGATE_H
#define REGISTERDELEGATE_H

#include <QStyledItemDelegate>
#include <QEvent>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionDelegate.h>

//-----------------------------------------------------------------------------
//! The delegate to provide editors to add/remove/edit the details of register.
//-----------------------------------------------------------------------------
class RegisterDelegate : public ExpressionDelegate
{
    Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterNameCompleter  The completer to use for parameter names in expression editor.
	 *      @param [in] parameterFinder         The parameter finder to use for expression editor.
	 *      @param [in] parent                  Pointer to the owner of the delegate.
	 */
	RegisterDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);
	
	//! The destructor.
	virtual ~RegisterDelegate();

	/*!
     *  Create a new editor for the given item.
	 *
	 *      @param [in] parent  Owner for the editor.
	 *      @param [in] option  Contains options for the editor.
	 *      @param [in] index   Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	 */
	virtual QWidget* createEditor(QWidget* parent, 	const QStyleOptionViewItem& option, QModelIndex const& index)
        const;

	/*!
     *  Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	 */
	virtual void setEditorData(QWidget* editor, const QModelIndex& index) const;

	/*!
     *  Save the data from the editor to the model.
	 *
	 *      @param [in] editor  Pointer to the editor that contains the data to store.
	 *      @param [in] model   Model that contains the data structure where data is to be saved to.
	 *      @param [in] index   Model index identifying the item that's data is to be saved.
	 */
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const;

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column  The column to check.
     *
     *      @return True if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    //! Gets the description column.
    virtual int descriptionColumn() const;

private slots:

	/*!
     *  Commit the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
	
	//! No copying.
	RegisterDelegate(const RegisterDelegate& other);

	//! No assignment.
	RegisterDelegate& operator=(const RegisterDelegate& other);
};

#endif // REGISTERDELEGATE_H
