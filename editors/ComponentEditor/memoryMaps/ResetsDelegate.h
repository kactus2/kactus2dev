//-----------------------------------------------------------------------------
// File: ResetsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 11.01.2019
//
// Description:
// The delegate to provide editors to add/remove/edit the resets of a field.
//-----------------------------------------------------------------------------

#ifndef RESETSDELEGATE_H
#define RESETSDELEGATE_H

#include <QStyledItemDelegate>
#include <QEvent>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionDelegate.h>

class ResetType;
class QComboBox;

//-----------------------------------------------------------------------------
//! The delegate to provide editors to add/remove/edit the resets of a field.
//-----------------------------------------------------------------------------
class ResetsDelegate : public ExpressionDelegate
{
    Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterNameCompleter  The completer to use for parameter names in expression editor.
	 *      @param [in] parameterFinder         The parameter finder to use for expression editor.
     *      @param [in] resetTypes              The reset types of the containing component.
	 *      @param [in] parent                  Pointer to the owner of the delegate.
	 */
    ResetsDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<QList<QSharedPointer<ResetType> > > resetTypes, QObject *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~ResetsDelegate();

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

    /*!
     *  Gets the description column (Mandatory for expression delegate, returns columncount).
     */
    virtual int descriptionColumn() const;

private slots:

	/*!
     *  Commit the data from the sending editor and close the editor.
	 */
	void commitAndCloseEditor();

private:
	
	//! No copying. No assignment.
	ResetsDelegate(const ResetsDelegate& other);
    ResetsDelegate& operator=(const ResetsDelegate& other);

    /*!
     *  Create the combo box for reset type reference editor.
     *
     *      @param [in] parent  Parent for the editor.
     *
     *      @return Pointer to the created reset type reference editor.
     */
    QComboBox* createResetTypeCombo(QWidget* parent) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The reset types of the containing component.
    QSharedPointer<QList<QSharedPointer<ResetType> > > resetTypes_;
};

#endif // RESETSDELEGATE_H
