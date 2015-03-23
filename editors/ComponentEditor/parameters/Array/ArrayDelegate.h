//-----------------------------------------------------------------------------
// File: ArrayDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.03.2015
//
// Description:
// Delegate that provides widgets for editing arrays.
//-----------------------------------------------------------------------------

#ifndef ARRAYDELEGATE_H
#define ARRAYDELEGATE_H

#include <editors/ComponentEditor/parameters/ChoiceCreatorDelegate.h>

#include <IPXACTmodels/choice.h>

#include <QCompleter>

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing arrays.
//-----------------------------------------------------------------------------
class ArrayDelegate : public ChoiceCreatorDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parameterCompleter  The completer to use for expression editors.
     *      @param [in] finder              The parameter finder to search for the parameters.
     *      @param [in] selectedChoice      The currently active choice.
     *      @param [in] parent              The parent object.
     */
    ArrayDelegate(QCompleter* parameterCompleter,
        QSharedPointer<ParameterFinder> finder,
        QSharedPointer<Choice> selectedChoice,
        QObject* parent = 0);

	//! The destructor
    virtual ~ArrayDelegate();

	/*! Create a new editor for the given item
	 *
	 *      @param [in] parent   Owner for the editor.
	 *      @param [in] option   Contains options for the editor.
	 *      @param [in] index    Model index identifying the item.
	 *
	 *      @return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 *      @param [in] editor  Pointer to the editor where the data is to be set.
	 *      @param [in] index   Model index identifying the item that's data is to be set.
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 *     @param [in] editor   Pointer to the editor that contains the data to store.
	 *     @param [in] model    Model that contains the data structure where data is to be saved to.
	 *     @param [in] index    Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

signals:

    //! Emitted when the values of the array change.
    void contentChanged();

protected:

    /*!
     *  Checks if the cells in given column accept expression as an input value.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the cells accept expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    /*!
     *  Gets the name of the currently selected choice.
     *
     *      @param [in] index   The index of the currently selected row.
     *
     *      @return The name of the selected choice.
     */
    virtual QString choiceNameOnRow(QModelIndex const& index) const;

    /*!
     *  Gets the column number for the value column.
     *
     *      @return The number of the column for value.
     */
    virtual int valueColumn() const;

    /*!
     *  Gets the currently selected choice.
     *
     *      @param [in] index   The index of the currently selected row.
     */
    virtual QSharedPointer<Choice> findChoice(QModelIndex const& index) const;

private:

	//! No copying
    ArrayDelegate(const ArrayDelegate& other);

	//! No assignment
    ArrayDelegate& operator=(const ArrayDelegate& other);

    //! The currently selected choice.
    QSharedPointer<Choice> selectedChoice_;
};

#endif // ARRAYDELEGATE_H
