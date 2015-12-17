//-----------------------------------------------------------------------------
// File: parameterseditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 04.04.2011
//
// Description:
// The editor to add/remove/edit parameters.
//-----------------------------------------------------------------------------

#ifndef PARAMETERSEDITOR_H
#define PARAMETERSEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/parameters/parametersmodel.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class LibraryInterface;
class ColumnFreezableTable;
class Component;
//-----------------------------------------------------------------------------
//! Editor to add/edit/remove parameters of a component.
//-----------------------------------------------------------------------------
class ParametersEditor : public ItemEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] component               Pointer to the component being edited.
	 *      @param [in] handler                 Pointer to the instance managing the library.
     *      @param [in] validator               The validator to use for checking parameter validity.
     *      @param [in] expressionParser        Expression parser for configurable elements.
	 *      @param [in] parameterFinder         Pointer to the instance searching for parameters.
	 *      @param [in] expressionFormatter     Pointer to the instance formatting the expressions.
	 *      @param [in] parent                  Pointer to the owner of this widget.
	 */
	ParametersEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterValidator2014> validator,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		QWidget *parent = 0);
	
	//! The destructor.
	virtual ~ParametersEditor();

	/*!
     *  Reload the information from the model to the editor.
	 */
	virtual void refresh();

protected:

	//! Handler for widget's show event.
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying.
	ParametersEditor(const ParametersEditor& other);

	//! No assignment.
	ParametersEditor& operator=(const ParametersEditor& other);

    //! The view that displays the parameters.
    ColumnFreezableTable* view_;

	//! The model that holds the data to be displayed to the user
	ParametersModel* model_;
};

#endif // PARAMETERSEDITOR_H
