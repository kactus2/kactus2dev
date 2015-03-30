/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementdelegate.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURABLEELEMENTDELEGATE_H
#define CONFIGURABLEELEMENTDELEGATE_H

#include <editors/ComponentEditor/parameters/ChoiceCreatorDelegate.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <IPXACTmodels/choice.h>

#include <QWidget>
#include <QSize>

//-----------------------------------------------------------------------------
//! Delegate that provides editors to edit the configurable element values.
//-----------------------------------------------------------------------------
class ConfigurableElementDelegate : public ChoiceCreatorDelegate
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterCompleter      Pointer to the completer, used in expression editor.
	 *      @param [in] parameterFinder         Pointer to the parameter finder, used in expression editor.
     *      @param [in] expressionFormatter     Expression formatter, used to change parameter ids to names.
	 *      @param [in] parent                  Pointer to the owner of this delegate.
	 */
	ConfigurableElementDelegate(QCompleter* parameterCompleter,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QObject *parent);
	
	//! \brief The destructor
	virtual ~ConfigurableElementDelegate();

    /*!
     *  Create a new editor for the given item.
     *
     *      @param [in] parent  Owner of the editor.
     *      @param [in] option  Contains options for the editor.
     *      @param [in] index   Model index identifying the item.
     *
     *      @return Pointer to the editor to be used to edit the item.
     */
    virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

    /*!
     *  Set the data for the editor.
     *
     *      @param [in] editor  Pointer to the editor where the data is to be set.
     *      @param [in] index   Model index identifying the item that's data is to be set.
     */
    virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

    /*!
     *  Save the data from the editor to the model.
     *
     *      @param [in] editor  Pointer to the editor that contains the data to store.
     *      @param [in] model   Model that contains the data structure where data is to be saved to.
     *      @param [in] index   Model index identifying the item that's data is to be saved.
     */
    virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

    /*!
     *  Set the choices for this delegate.
     *
     *      @param [in] choices     The choices of the current component.
     */
    void setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices);

    /*!
     *  Updates the editor geometry.
     *
     *      @param [in] editor  The editor being updated.
     *      @param [in] option  The options used to update the editor.
     *      @param [in] index   The model index being edited.
     */
    virtual void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
        const QModelIndex &index) const;

    /*!
     *  Paint the delegate.
     *
     *      @param [in] painter     The painter used.
     *      @param [in] option      The style option in use.
     *      @param [in] index       The current index of the model.
     */
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;

    /*!
     *  Returns the size needed by the delegate to display the item in index.
     *
     *      @param [in] option  The style information.
     *      @param [in] index   The index of the item.
     *
     *      @return The size needed by the delegate to display the item in index.
     */
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column      The column to check.
     *
     *      @return True, if the cells in the column accept expressions, otherwise false.
     */
    bool columnAcceptsExpression(int column) const;

    /*!
     *  Gets the name of the choice on the currently selected row.
     *
     *      @param [in] index   The index of the currently selected row.
     *
     *      @return The name of the choice used in the row on the given index.
     */
    virtual QString choiceNameOnRow(QModelIndex const& index) const;

    /*!
     *  Gets the column number of the value column.
     *
     *      @return The column number of the value column.
     */
    virtual int valueColumn() const;

private:
	//! \brief No copying
	ConfigurableElementDelegate(const ConfigurableElementDelegate& other);

	//! \brief No assignment
	ConfigurableElementDelegate& operator=(const ConfigurableElementDelegate& other);

    /*!
     *  Check if the value at the given index is an array.
     *
     *      @param [in] index   The index of the current value.
     *
     *      @return True, if the value is an array, false otherwise.
     */
    bool valueIsArray(QModelIndex const& index) const;

    /*!
     *  Create the editor for the array value.
     *
     *      @param [in] editor  The editor to be changed to array editor.
     *      @param [in] index   The index of the current value.
     */
    void createArrayEditor(QWidget* editor, QModelIndex const& index) const;

    /*!
     *  Repositions the editor to better use the available space. The editor is then resized to best cover the
     *  usable area.
     *
     *      @param [in] editor  The editor to reposition.
     *      @param [in] option  The style options for the editor.
     *      @param [in] index   The current index.
     */
    void repositionAndResizeEditor(QWidget* editor, QStyleOptionViewItem const& option, QModelIndex const& index)
        const;

    //! Expression formatter, used in expressions to change parameter ids into names.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;
};

#endif // CONFIGURABLEELEMENTDELEGATE_H
