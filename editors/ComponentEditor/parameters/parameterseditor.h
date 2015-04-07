/* 
 *
 *  Created on: 4.4.2011
 *      Author: Antti Kamppi
 * 		filename: parameterseditor.h
 */

#ifndef PARAMETERSEDITOR_H
#define PARAMETERSEDITOR_H

#include <editors/ComponentEditor/itemeditor.h>
#include <editors/ComponentEditor/parameters/parametersmodel.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ParameterFinder.h>

#include <IPXACTmodels/component.h>

#include <QSortFilterProxyModel>
#include <QSharedPointer>

class LibraryInterface;
class ColumnFreezableTable;

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
	 *      @param [in] parameterFinder         Pointer to the instance searching for parameters.
	 *      @param [in] expressionFormatter     Pointer to the instance formatting the expressions.
	 *      @param [in] parent                  Pointer to the owner of this widget.
	 */
	ParametersEditor(QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
		QWidget *parent = 0);
	
	//! The destructor
	virtual ~ParametersEditor();

	/*! Check for the validity of the edited parameters.
	*
	* \return True if all parameters are in valid state.
	*/
	virtual bool isValid() const;

	/*! Reload the information from the model to the editor.
	*/
	virtual void refresh();

    /*!
     *  Sets the component whose parameters to edit.
     *
     *      @param [in] component   The component to set.
     */
    void setComponent(QSharedPointer<Component> component);

protected:

	//! Handler for widget's show event
	virtual void showEvent(QShowEvent* event);

private:

	//! No copying
	ParametersEditor(const ParametersEditor& other);

	//! No assignment
	ParametersEditor& operator=(const ParametersEditor& other);

    //! The view that displays the parameters.
    ColumnFreezableTable* view_;

	//! The model that holds the data to be displayed to the user
	ParametersModel* model_;
};

#endif // PARAMETERSEDITOR_H
