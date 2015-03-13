//-----------------------------------------------------------------------------
// File: ExpressionDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.01.2015
//
// Description:
// Base class for delegates providing editors for expressions.
//-----------------------------------------------------------------------------

#ifndef EXPRESSIONDELEGATE_H
#define EXPRESSIONDELEGATE_H

#include <QStyledItemDelegate>
#include <QCompleter>

class ParameterFinder;

//-----------------------------------------------------------------------------
//! Base class for delegates providing editors for expressions.
//-----------------------------------------------------------------------------
class ExpressionDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parameterNameCompleter      The completer to use for parameter names in expression editor.
     *      @param [in] parameterFinder             The parameter finder to use for for expression editor.
     *      @param [in] parent                      The parent object.
     */
    ExpressionDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder, 
        QObject* parent);
    
    //! The destructor.
    virtual ~ExpressionDelegate();

    /*! Create a new editor for the given item
	 *
	 * @param [in] parent   Owner for the editor.
	 * @param [in] option   Contains options for the editor.
	 * @param [in] index    Model index identifying the item.
	 *
	 * @return Pointer to the editor to be used to edit the item.
	*/
	virtual QWidget* createEditor(QWidget* parent, QStyleOptionViewItem const& option, 
        QModelIndex const& index) const;

	/*! Set the data for the editor.
	 *
	 * @param [in] editor   Pointer to the editor where the data is to be set.
	 * @param [in] index    Model index identifying the item that's data is to be set.
	 *
	*/
	virtual void setEditorData(QWidget* editor, QModelIndex const& index) const;

	/*! Save the data from the editor to the model.
	 *
	 * @param [in] editor   Pointer to the editor that contains the data to store.
	 * @param [in] model    Model that contains the data structure where data is to be saved to.
	 * @param [in] index    Model index identifying the item that's data is to be saved.
	 *
	*/
	virtual void setModelData(QWidget* editor, QAbstractItemModel* model, QModelIndex const& index) const;

signals: 

    /*!
    *  Increase the amount of references to a parameter corresponding to the id.
    *
    *      @param [in] id      The id of the parameter being searched for.
    */
    void increaseReferences(QString id);

    /*!
    *  Decrease the amount of references to a parameter corresponding to the id.
    *
    *      @param [in] id      The id of the parameter being searched for.
    */
    void decreaseReferences(QString id);

protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const = 0;

    //! Finder for parameters in the containing component.
    QSharedPointer<ParameterFinder> parameterFinder_;

    //! The completer for parameter names in expressions.
    QCompleter* parameterNameCompleter_;

private:
    // Disable copying.
    ExpressionDelegate(ExpressionDelegate const& rhs);
    ExpressionDelegate& operator=(ExpressionDelegate const& rhs);
   
    /*!
     *  Creates an editor for expressions.
     *
     *      @param [in] parent   The parent widget for the editor.
     *
     *      @return An editor for expressions.
     */
    QWidget* createExpressionEditor(QWidget* parent) const;
};

#endif // EXPRESSIONDELEGATE_H
