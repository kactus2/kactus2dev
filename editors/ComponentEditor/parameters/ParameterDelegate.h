//-----------------------------------------------------------------------------
// File: ParameterDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.11.2014
//
// Description:
// Delegate that provides widgets for editing parameters.
//-----------------------------------------------------------------------------

#ifndef PARAMETERDELEGATE_H
#define PARAMETERDELEGATE_H

#include <QStyledItemDelegate>

class Choice;

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing parameters.
//-----------------------------------------------------------------------------
class ParameterDelegate : public QStyledItemDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *     @param [in] choices  The choices available for model parameter value.
	 *     @param [in] parent   The parent object
	*/
	ParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, QObject *parent = 0);

	//! The destructor
	virtual ~ParameterDelegate();

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

protected:

    /*!
     *  Gets the column for choices.
     *
     *      @return The column for editing choice selection.
     */
    virtual int choiceColumn() const;
    
    /*!
     *  Gets the column for values.
     *
     *      @return The column for editing value selection.
     */
    virtual int valueColumn() const;

private:

	//! No copying
	ParameterDelegate(const ParameterDelegate& other);

	//! No assignment
	ParameterDelegate& operator=(const ParameterDelegate& other);

    /*!
     *  Finds if the given index is used to select a model parameter value using a choice.
     *
     *      @param [in] index   The index to check.
     *
     *      @return True, if the index is for selecting value with a choice, otherwise false.
     */
    bool isIndexForValueUsingChoice(QModelIndex const& index) const;
    
    /*!
     *  Finds the name of the choice on the row identified by the given index.
     *
     *      @param [in] index   The index on whose row to find the choice name.
     *
     *      @return The name of the choice on the row.
     */
    QString choiceNameOnRow(QModelIndex const& index) const;

    /*!
     *  Finds the choice used on the row identified by the given index.
     *
     *      @param [in] index   The index whose row to find the choice from.
     *
     *      @return The choice selected on the given row.
     */
    QSharedPointer<Choice> findChoice(QModelIndex const& index) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The choices available for model parameter value.
    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;
};

#endif // ParameterDelegate_H
