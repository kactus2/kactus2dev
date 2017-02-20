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

#include <QCompleter>

class Choice;
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

    //! Gets the description column.
    virtual int descriptionColumn() const;

private:

	//! No copying.
    ArrayDelegate(const ArrayDelegate& other);

	//! No assignment.
    ArrayDelegate& operator=(const ArrayDelegate& other);


    //! The currently selected choice.
    QSharedPointer<Choice> selectedChoice_;
};

#endif // ARRAYDELEGATE_H
