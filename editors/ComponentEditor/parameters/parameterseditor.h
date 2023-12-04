//-----------------------------------------------------------------------------
// File: parameterseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 04.04.2011
//
// Description:
// The editor to add/remove/edit parameters.
//-----------------------------------------------------------------------------

#ifndef PARAMETERSEDITOR_H
#define PARAMETERSEDITOR_H

#include <editors/ComponentEditor/ParameterItemEditor.h>
#include <editors/ComponentEditor/parameters/parametersmodel.h>
#include <KactusAPI/include/ExpressionFormatter.h>
#include <KactusAPI/include/ParameterFinder.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class LibraryInterface;
class Component;
class ParametersView;
class ParameterValidator;

//-----------------------------------------------------------------------------
//! Editor to add/edit/remove parameters of a component.
//-----------------------------------------------------------------------------
class ParametersEditor : public ParameterItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] handler                 Pointer to the instance managing the library.
     *      @param [in] parameterFinder         Pointer to the instance searching for parameters.
     *      @param [in] expressionParser        Expression parser for configurable elements.
	 *      @param [in] expressionFormatter     Pointer to the instance formatting the expressions.
     *      @param [in] parameterInterface      Interface for accessing parameters.
     *      @param [in] parent                  Pointer to the owner of this widget.
	 */
    ParametersEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        ParametersInterface* parameterInterface,
        QWidget *parent = 0);

	/*!
     *  The destructor.
     */
	virtual ~ParametersEditor() = default;

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

protected:

	/*!
     *  Handler for widget's show event.
     */
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying.
	ParametersEditor(const ParametersEditor& other);

	//! No assignment.
	ParametersEditor& operator=(const ParametersEditor& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The view that displays the parameters.
    ParametersView* view_;

	//! The model that holds the data to be displayed to the user
	ParametersModel* model_;

    //! Interface for accessing parameters.
    ParametersInterface* parameterInterface_;
};

#endif // PARAMETERSEDITOR_H
