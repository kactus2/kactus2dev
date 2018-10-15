//-----------------------------------------------------------------------------
// File: registerfiledelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Dan Chianucci
// Date: 19.06.2018
//
// Description:
// The delegate that provides editors to add/remove/edit the details of register file
//-----------------------------------------------------------------------------
#ifndef REGISTERFILEDELEGATE_H
#define REGISTERFILEDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to add/remove/edit the details of register file
//-----------------------------------------------------------------------------
class RegisterFileDelegate: public ExpressionDelegate
{
  Q_OBJECT

public:

  /*!
   *  The constructor
   *
   *      @param [in] parameterNameCompleter      The completer to use for parameter names in expression editor.
   *      @param [in] parameterFinder             The parameter finder to use for for expression editor.
   *      @param [in] parent                      Pointer to the owner of this delegate.
   */
  RegisterFileDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder,
        QObject *parent);

  //! The destructor.
  virtual ~RegisterFileDelegate() = default;

  //! No copying
  RegisterFileDelegate(const RegisterFileDelegate& other) = delete;
  RegisterFileDelegate& operator=(const RegisterFileDelegate& other) = delete;


protected:

    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the cells in the column allow expressions, otherwise false.
     */
    virtual bool columnAcceptsExpression(int column) const;

    //! Gets the description column.
    virtual int descriptionColumn() const;

};

#endif // REGISTERFILEDELEGATE_H
