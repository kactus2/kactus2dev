//-----------------------------------------------------------------------------
// File: addressspacesdelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 11.6.2012
//
// Description:
// The delegate that provides editors to edit the address spaces summary.
//-----------------------------------------------------------------------------

#ifndef ADDRESSSPACESDELEGATE_H
#define ADDRESSSPACESDELEGATE_H

#include <editors/ComponentEditor/common/ExpressionDelegate.h>

#include <QStyledItemDelegate>
#include <QEvent>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit the address spaces summary.
//-----------------------------------------------------------------------------
class AddressSpacesDelegate : public ExpressionDelegate 
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
	AddressSpacesDelegate(QCompleter* parameterNameCompleter, QSharedPointer<ParameterFinder> parameterFinder, 
        QObject *parent);
	
	//! The destructor
	virtual ~AddressSpacesDelegate();

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

private:
	//! No copying
	AddressSpacesDelegate(const AddressSpacesDelegate& other);
	AddressSpacesDelegate& operator=(const AddressSpacesDelegate& other);

};

#endif // ADDRESSSPACESDELEGATE_H
