//-----------------------------------------------------------------------------
// File: ComponentInstanceConfigurableElementsEditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.09.2017
//
// Description:
// Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCECONFIGURABLEELEMENTSEDITOR_H
#define COMPONENTINSTANCECONFIGURABLEELEMENTSEDITOR_H

#include <designEditors/common/ComponentInstanceEditor/configurableelementeditor.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QAbstractItemModel>
#include <QSharedPointer>

class Component;
class ComponentInstance;
class ComponentInstanceConfigurableElementsModel;

//-----------------------------------------------------------------------------
//! Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------
class ComponentInstanceConfigurableElementsEditor : public ConfigurableElementEditor
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] elementFinder                   The finder for the configurable element values.
	 *      @param [in] parameterFinder                 The finder for configurable elements and top parameters.
	 *      @param [in] configurableElementFormatter    Formats referencing expressions in configurable elements.
	 *      @param [in] instanceExpressionFormatter     Formats referencing expressions in component instance.
	 *      @param [in] expressionParser                Solves expressions in configurable elements.
	 *      @param [in] instanceParser                  Solves expressions in default values (component instance).
	 *      @param [in] completionModel                 The completion model for selecting parameter references.
	 *      @param [in] parent                          The parent widget.
	 */
    ComponentInstanceConfigurableElementsEditor(QSharedPointer<ConfigurableElementFinder> elementFinder,
        QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionFormatter> configurableElementFormatter,
        QSharedPointer<ExpressionFormatter> instanceExpressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionParser> instanceParser,
        QAbstractItemModel* completionModel, QWidget *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~ComponentInstanceConfigurableElementsEditor();

	/*!
     *  Set the component instance to be edited.
	 *
	 *      @param [in] component           Pointer to the component referenced by the component instance.
     *      @param [in] instance            Pointer to the component instance to edit.
     *      @param [in] viewConfiguration   Pointer to the view configuration of the component instance.
     *      @param [in] editProvider        Pointer to the editing capabilities.
	 */
    void setComponent(QSharedPointer<Component> component, QSharedPointer<ComponentInstance> instance,
        QSharedPointer<ViewConfiguration> viewConfiguration, QSharedPointer<IEditProvider> editProvider);

	/*!
     *  Clear the editor from all data.
	 */
	virtual void clear();

private:
    //! No copying.	No assignment.
	ComponentInstanceConfigurableElementsEditor(const ComponentInstanceConfigurableElementsEditor& other);
    ComponentInstanceConfigurableElementsEditor& operator=(const ComponentInstanceConfigurableElementsEditor& other);

	//! The model to edit the configurable elements of a component instance.
    ComponentInstanceConfigurableElementsModel* model_;
};

#endif // COMPONENTINSTANCECONFIGURABLEELEMENTSEDITOR_H
