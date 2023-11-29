//-----------------------------------------------------------------------------
// File: PortSummaryDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.11.2023
//
// Description:
// Delegate for port summary model.
//-----------------------------------------------------------------------------

#ifndef PORT_SUMMARY_DELEGATE_H
#define PORT_SUMMARY_DELEGATE_H

#include <editors/ComponentEditor/ports/portsdelegate.h>

#include <QCompleter>

class ParameterFinder;
class Component;
class PortTypeValidator;

//-----------------------------------------------------------------------------
//! Delegate for port summary model.
//-----------------------------------------------------------------------------
class PortSummaryDelegate : public PortsDelegate
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] component           Component containing the edited ports.
     *      @param [in] completionModel     Model containing the completions used in expression editor.
     *      @param [in] parameterFinder     The parameter finder to use for for expression editor.
     *      @param [in] typeValidator       Validator for port type definitions.
     *      @param [in] parent              The parent object.
     */
    using PortsDelegate::PortsDelegate;
	
	/*!
     *  The destructor.
     */
    ~PortSummaryDelegate() = default;

protected:
    
    /*!
     *  Checks if the given column supports expressions in the editor.
     *
     *      @param [in] column   The column to check.
     *
     *      @return True, if the cells in the column allow expressions, otherwise false.
     */
    bool columnAcceptsExpression(int column) const final;

    /*!
     *  Gets the description column.
     *
     *      @return Column number for description.
     */
    int descriptionColumn() const final;

private:

    /*!
     *  Gets the name column.
     *
     *      @return Column number for name.
     */
    int nameColumn() const final;

    /*!
     *  Gets the type definitions column.
     *
     *      @return Column number for type definitions.
     */
     int typeColumn() const final;

    /*!
     *  Gets the tag group column.
     *
     *      @return Column number for tag group.
     */
    int tagColumn() const final;

    /*!
     *  Gets the ad hoc column.
     *
     *      @return Column number for ad hoc.
     */
    int adHocColumn() const final;

    /*!
     *  Gets the array left column.
     *
     *      @return Column number for left value of the array.
     */
    int arrayLeftColumn() const final;

    /*!
     *  Gets the array right column.
     *
     *      @return Column number for right value of the array.
     */
    int arrayRightColumn() const final;

};

#endif // PORT_SUMMARY_DELEGATE_H
