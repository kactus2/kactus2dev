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

#include <editors/ComponentEditor/parameters/ChoiceCreatorDelegate.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <QCompleter>

class ParameterFinder;
class Choice;

//-----------------------------------------------------------------------------
//! Delegate that provides widgets for editing parameters.
//-----------------------------------------------------------------------------
class ParameterDelegate : public ChoiceCreatorDelegate
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] choices                 The choices available for model parameter value.
	 *      @param [in] parameterCompleter      The completer to use for expression editors.
	 *      @param [in] finder                  The parameter finder to user for expression editors.
	 *      @param [in] expressionFormatter     The expression formatter for changing ids to parameter names.
	 *      @param [in] parent                  The parent of the object.
	 */
	ParameterDelegate(QSharedPointer<QList<QSharedPointer<Choice> > > choices, 
        QCompleter* parameterCompleter,
        QSharedPointer<ParameterFinder> finder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QObject* parent = 0);

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

    /*!
     *  Paint the delegate.
     *
     *      @param [in] painter     The painter used.
     *      @param [in] option      Style option in use.
     *      @param [in] index       Index of model.
     */
    virtual void paint(QPainter *painter, QStyleOptionViewItem const& option, QModelIndex const& index) const;

    /*!
     *  Updates the editor geometry.
     *
     *      @param [in] editor  The editor being updated.
     *      @param [in] option  The options used to update the editor.
     *      @param [in] index   The model index being edited.
     */
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

signals:

    /*!
     *  Open the reference tree of the selected parameter.
     *
     *      @param [in] id      The id of the parameter.
     */
    void openReferenceTree(QString const& id) const;

    //! Emitted when the values of the array change.
    void contentChanged();

protected:

    /*!
     *  Gets the column for name.
     *
     *      @return The column index for editing name.
     */
    virtual int nameColumn() const;

    /*!
     *  Gets the column for choices.
     *
     *      @return The column for editing choice selection.
     */
    virtual int choiceColumn() const;

    /*!
     *  Gets the column for value format.
     *
     *      @return The column for editing format selection.
     */
    virtual int formatColumn() const;
    
    /*!
    *  Gets the column for the left value of bit width vector.
    *
    *      @return The column for editing the left value of the bit width vector.
    */
    virtual int bitWidthLeftColumn() const;

    /*!
    *  Gets the column for the right value of bit width vector.
    *
    *      @return The column for editing the right value of the bit width vector.
    */
    virtual int bitWidthRightColumn() const;

    /*!
     *  Gets the column for minimum value.
     *
     *      @return The column for editing the minimum value.
     */
    virtual int minimumColumn() const;
    
    /*!
     *  Gets the column for maximum value.
     *
     *      @return The column for editing the maximum value.
     */
    virtual int maximumColumn() const;

    /*!
     *  Gets the column for values.
     *
     *      @return The column for editing value selection.
     */
    virtual int valueColumn() const;

    /*!
     *  Gets the column for resolve.
     *
     *      @return The column for editing value selection.
     */
    virtual int resolveColumn() const;

    /*!
     *  Gets the column for description.
     *
     *      @return     The column for description.
     */
    virtual int descriptionColumn() const;

    /*!
     *  Gets the column for array left.
     *
     *      @return     The column index for array left.
     */
    virtual int arrayLeftColumn() const;

    /*!
     *  Gets the column for array right.
     *
     *      @return     The column index for array right.
     */
    virtual int arrayRightColumn() const;

    /*!
     *  Gets the column for usage count.
     *
     *      @return     The column index for usage count.
     */
    virtual int usageCountColumn() const;

    /*!
     *  Gets the column index for the id of the parameter.
     *
     *      @return     The column index for the parameter id.
     */
    virtual int idColumn() const;

    /*!
     *  Checks if the cells in given column accept expression as an input value.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the cells accept expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    /*!
     *  Finds the name of the choice on the row identified by the given index.
     *
     *      @param [in] index   The index on whose row to find the choice name.
     *
     *      @return The name of the choice on the row.
     */
    QString choiceNameOnRow(QModelIndex const& index) const;

    /*!
     *  Creates an editor for selecting a choice.
     *
     *      @param [in] parent   The parent widget for the editor.
     *
     *      @return An editor for selecting a choice.
     */
    QWidget* createChoiceSelector(QWidget* parent) const;
    
    /*!
     *  Creates an editor for selecting a format for parameter value.
     *
     *      @param [in] parent   The parent widget for the editor.
     *
     *      @return An editor for selecting a format.
     */
    QWidget* createFormatSelector(QWidget* parent) const;
        
    /*!
     *  Creates an editor for selecting resolve attribute of a parameter.
     *
     *      @param [in] parent   The parent widget for the editor.
     *
     *      @return An editor for selecting resolve attribute.
     */
    QWidget* createResolveSelector(QWidget* parent) const;

private:

	//! No copying
	ParameterDelegate(const ParameterDelegate& other);

	//! No assignment
	ParameterDelegate& operator=(const ParameterDelegate& other);

    /*!
     *  Check if the value should be an array.
     *
     *      @param [in] index   The index of the current value.
     *
     *      @return True, if the value is an array, otherwise false.
     */
    bool valueIsArray(QModelIndex const& index) const;

    /*!
     *  Repositions the editor if there is not enough space under the default position. The editor is then resized
     *  to use the available space.
     *
     *      @param [in] editor  The editor to reposition.
     *      @param [in] option  The style options for the editor.
     *      @param [in] index   The current index.
     */
    void repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

    /*!
     *  Gets the size of the array.
     *
     *      @param [in] arrayLeftValue      The left value of the array.
     *      @param [in] arrayRightValue     The right value of the array.
     */
    int getArraySize(int arrayLeftValue, int arrayRightValue) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression formatter used to change parameter uuids in references to parameter names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // ParameterDelegate_H
