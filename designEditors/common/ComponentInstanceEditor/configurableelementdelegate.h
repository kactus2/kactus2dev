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

#include <IPXACTmodels/choice.h>

#include <QWidget>

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
	 *      @param [in] parameterCompleter  Pointer to the completer, used in expression editor.
	 *      @param [in] parameterFinder     Pointer to the parameter finder, used in expression editor.
	 *      @param [in] parent              Pointer to the owner of this delegate.
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

};

#endif // CONFIGURABLEELEMENTDELEGATE_H
