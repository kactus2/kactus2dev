//-----------------------------------------------------------------------------
// File: ChoiceCreatorDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.03.2015
//
// Description:
// Delegate for providing functionality for choices inside parameters.
//-----------------------------------------------------------------------------

#ifndef CHOICECREATORDELEGATE_H
#define CHOICECREATORDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <IPXACTmodels/choice.h>

#include <QCompleter>

//-----------------------------------------------------------------------------
//! Delegate for providing functionality for choices inside parameters.
//-----------------------------------------------------------------------------
class ChoiceCreatorDelegate : public ExpressionDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parameterCompleter  Pointer to the completer, used in expression editor.
     *      @param [in] finder              Pointer to the parameter finder, used in expression editor.
     *      @param [in] parent              Pointer to the owner of this delegate.
     */
    ChoiceCreatorDelegate(QCompleter* parameterCompleter, QSharedPointer<ParameterFinder> finder,
        QObject* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~ChoiceCreatorDelegate();
    
protected:

    /*!
     *  Set the choices for this delegate.
     *
     *      @param [in] choices     The currently available choices.
     */
    void setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices);

    /*!
     *  Gets the currently available choices.
     *
     *      @return The currently available choices.
     */
    QSharedPointer<QList<QSharedPointer<Choice> > > getChoices() const;

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column  The column to check.
     *
     *      @return True, if cells in the column accept expressions, otherwise false.
     */
    bool columnAcceptsExpression(int column) const = 0;

    /*!
     *  Check if the index is used to select a parameter value using a choice.
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
    virtual QString choiceNameOnRow(QModelIndex const& index) const = 0;

    /*!
     *  Creates the enumeration selector for a value containing a choice.
     *
     *      @param [in] parent  The parent widget for the editor.
     *      @param [in] index   The index for which to create the editor.
     *
     *      @return An editor for selecting an enumeration.
     */
    virtual QWidget* createEnumerationSelector (QWidget* parent, QModelIndex const& index) const;

    /*!
     *  Gets the choice used on the row identified by the given index.
     *
     *      @param [in] index   The index of the row to find the choice from.
     *
     *      @return The choice of the given row.
     */
    virtual QSharedPointer<Choice> findChoice(QModelIndex const& index) const;

    /*!
     *  Get the column of the possible place for enumeration selector.
     *
     *      @return The column number for the column that's value can be given as an enumeration.
     */
    virtual int valueColumn() const = 0;

private:

	//! No copying
    ChoiceCreatorDelegate(const ChoiceCreatorDelegate& other);

	//! No assignment
    ChoiceCreatorDelegate& operator=(const ChoiceCreatorDelegate& other);

    //! The currently available choices.
    QSharedPointer<QList<QSharedPointer<Choice> > > choices_;
};

#endif // CHOICECREATORDELEGATE_H
