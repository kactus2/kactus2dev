//-----------------------------------------------------------------------------
// File: ChoiceCreatorDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 20.03.2015
//
// Description:
// Delegate for providing functionality for choices inside parameters.
//-----------------------------------------------------------------------------

#ifndef CHOICECREATORDELEGATE_H
#define CHOICECREATORDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <QCompleter>

class Choice;

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
     *    @param [in] completionModel     Model containing the completions used in expression editor.
     *    @param [in] finder              Pointer to the parameter finder, used in expression editor.
     *    @param [in] parent              Pointer to the owner of this delegate.
     */
    ChoiceCreatorDelegate(QAbstractItemModel* completionModel, QSharedPointer<ParameterFinder> finder,
        QObject* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~ChoiceCreatorDelegate();
    
protected:

    /*!
     *  Set the choices for this delegate.
     *
     *    @param [in] choices     The currently available choices.
     */
    void setChoices(QSharedPointer<QList<QSharedPointer<Choice> > > choices);

    /*!
     *  Gets the currently available choices.
     *
     *    @return The currently available choices.
     */
    QSharedPointer<QList<QSharedPointer<Choice> > > getChoices() const;

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *    @param [in] column  The column to check.
     *
     *    @return True, if cells in the column accept expressions, otherwise false.
     */
    bool columnAcceptsExpression(int column) const = 0;

    /*!
     *  Finds the name of the choice corresponding to the given index.
     *
     *    @param [in] index   The index of the row to get the choice name from.
     *
     *    @return The name of the choice on the row.
     */
    virtual QString choiceNameOnRow(QModelIndex const& index) const = 0;

    /*!
     *  Gets the choice used on the row identified by the given index.
     *
     *    @param [in] index   The index of the row to find the choice from.
     *
     *    @return The choice of the given row.
     */
    virtual QSharedPointer<Choice> findChoice(QModelIndex const& index) const;

    /*!
     *  Get the choice items for the combo editor.
     *
     *    @param [in] index   Index of the edited item.
     *
     *    @return List of choice items for the combo editor.
     */
    QStringList getChoiceComboItems(QModelIndex const& index) const;

    /*!
     *  Get the enumeration text for the enumeration value.
     *
     *    @param [in] index               Index of the edited item.
     *    @param [in] enumerationValue    The selected enumeration value.
     *
     *    @return The selected enumeration text.
     */
    QString findEnumerationText(QModelIndex const& index, QString const& enumerationValue) const;

    /*!
     *  Get the enumeration value for the enumeration text.
     *
     *    @param [in] index               Index of the edited item.
     *    @param [in] enumerationText     The selected enumeration text.
     *
     *    @return The selected enumeration value.
     */
    QString findEnumerationValue(QModelIndex const& index, QString const& enumerationText) const;

    /*!
     *  Get the column of the possible place for enumeration selector.
     *
     *    @return The column number for the column that's value can be given as an enumeration.
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
