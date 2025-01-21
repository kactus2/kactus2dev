//-----------------------------------------------------------------------------
// File: cpusdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 14.06.2012
//
// Description:
// The delegate to provide editors to add/remove/edit the cpus of a component.
//-----------------------------------------------------------------------------

#ifndef CPUSDELEGATE_H
#define CPUSDELEGATE_H

#include <editors/ComponentEditor/common/EnumerationEditorConstructorDelegate.h>

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <QStyledItemDelegate>
#include <QSharedPointer>
#include <QCompleter>

class Component;

//-----------------------------------------------------------------------------
//! The delegate to provide editors to add/remove/edit the cpus of a component.
//-----------------------------------------------------------------------------
class CpusDelegate : public EnumerationEditorConstructorDelegate
{
	Q_OBJECT

public:

	/*! The constructor
	 *
     *    @param [in] component               The component being edited.
     *    @param [in] parameterNameCompleter  The completer to use for parameter names in expression editor.
     *    @param [in] parameterFinder         The parameter finder to use for for expression editor.
	 *    @param [in] parent                  The owner of the delegate.
	*/
	CpusDelegate(QSharedPointer<Component> component, QCompleter* parameterNameCompleter, 
		QSharedPointer<ParameterFinder> parameterFinder, QObject *parent);
	
	//! The destructor.
	virtual ~CpusDelegate() = default;

    //! No copying
    CpusDelegate(const CpusDelegate& other) = delete;
    CpusDelegate& operator=(const CpusDelegate& other) = delete;


	/*! Create a new editor for the given item
	 *
	 *    @param [in] parent  Owner for the editor.
	 *    @param [in] option  Contains options for the editor.
	 *    @param [in] index   Model index identifying the item.
	 *
	 *    @return The editor to be used to edit the item.
	*/
	QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const final;

	/*! Set the data for the editor.
	 *
	 *    @param [in] editor The editor where the data is to be set.
	 *    @param [in] index Model index identifying the item that's data is to be set.
	*/
	void setEditorData(QWidget* editor, QModelIndex const& index) const final;

	/*! Save the data from the editor to the model.
	 *
	 *    @param [in] editor  The editor that contains the data to store.
	 *    @param [in] model   Model that contains the data structure where data is to be saved to.
	 *    @param [in] index   Model index identifying the item that's data is to be saved.
	*/
	void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const final;

signals:

    /*!
     *  Increase the amount of references to a parameter corresponding to the id.
     *
     *    @param [in] id      The id of the parameter being searched for.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the amount of references to a parameter corresponding to the id.
     *
     *    @param [in] id      The id of the parameter being searched for.
     */
    void decreaseReferences(QString const& id);

protected:

    //! Filters events for editors.
    bool eventFilter(QObject* editor, QEvent* event) final;

private:
	
    /*!
     *  Check if the column is used for enumerations.
     *
     *    @param [in] index   The selected index.
     *
     *    @return True, if the column is used for editing enumerations, false otherwise.
     */
    bool isEnumerationEditorColumn(QModelIndex const& index) const final;

    /*!
     *  The list of currently selected enumerations in the selected item.
     *
     *    @param [in] index   Index of the selected item.
     *
     *    @return List of currently selected enumerations.
     */
    QStringList getCurrentSelection(QModelIndex const& index) const final;

    /*!
     *  Get the list of the available enumerations.
     *
     *    @return List of the available enumerations.
     */
    QStringList getAvailableItems() const final;

    /*!
     *  Set the selected enumerations to the selected item.
     *
     *    @param [in] index           Model index identifying the item that's data is to be saved.
     *    @param [in] model           Model that contains the data structure where data is to be saved to.
     *    @param [in] selectedItems   List of the selected enumerations.
     */
    void setEnumerationDataToModel(QModelIndex const& index, QAbstractItemModel* model, 
        QStringList const& selectedItems) const final;

	/*!
	 *  Checks if given column accepts expressions as value.
	 *
	 *    @param [in] column The column number to check.
	 *
	 *    @return True, if column cells accept expressions, otherwise false.
	 */
	bool columnAcceptsExpression(int column) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    // 
	//! The component being edited.
	QSharedPointer<Component> component_;

    //!  The completer to use for parameter names in expression editor.
	QCompleter* parameterNameCompleter_;

    //! The parameter finder to use for for expression editor.
	QSharedPointer<ParameterFinder> parameterFinder_;

};

#endif // CPUSDELEGATE_H
