/* 
 *  	Created on: 12.8.2011
 *      Author: Antti Kamppi
 * 		filename: configurableelementdelegate.h
 *		Project: Kactus 2
 */

#ifndef CONFIGURABLEELEMENTDELEGATE_H
#define CONFIGURABLEELEMENTDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <IPXACTmodels/choice.h>

#include <QWidget>

/*! \brief Delegate that provides editors to edit the configurable element values.
 *
 */
class ConfigurableElementDelegate : public ExpressionDelegate
 {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param component Pointer to the component that defines the component instance type.
	 * \param parent Pointer to the owner of this delegate.
	 *
	*/
	ConfigurableElementDelegate(QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> parameterFinder,
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

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column      The column to check.
     *
     *      @return True, if the cells in the column accept expressions, otherwise false.
     */
    bool columnAcceptsExpression(int column) const;

private:
	//! \brief No copying
	ConfigurableElementDelegate(const ConfigurableElementDelegate& other);

	//! \brief No assignment
	ConfigurableElementDelegate& operator=(const ConfigurableElementDelegate& other);

    /*!
     *  Checks if the index is used to select a parameter value using a choice.
     *
     *      @param [in] index   The index to check.
     *
     *      @return True, if the index is for selecting a value with a choice, otherwise false.
     */
    bool isIndexForValueUsingChoice(QModelIndex const& index) const;

    /*!
     *  Finds the name of the choice corresponding to the given index.
     *
     *      @param [in] index   The index of the row to get the choice name from.
     *
     *      @return The name of the choice on the row.
     */
    QString getChoiceNameAtIndex(QModelIndex const& index) const;

    /*!
     *  Create the enumeration selector for a value containing a choice.
     *
     *      @param [in] parent  The parent widget for the editor.
     *      @param [in] index   The index for which to create the editor.
     *
     *      @return An editor for selecting an enumeration.
     */
    QWidget* createEnumerationSelector(QWidget* parent, QModelIndex const& index) const;

    /*!
     *  Gets the choice used on the row identified by the given index.
     *
     *      @param [in] index   The index whose row to find the choice from.
     *
     *      @return The choice selected on the given row
     */
    QSharedPointer<Choice> findChoice(QModelIndex const& index) const;

    //! The list of the currently available choices.
    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;
};

#endif // CONFIGURABLEELEMENTDELEGATE_H
