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

#include <editors/common/ComponentInstanceEditor/configurableelementeditor.h>
#include <editors/common/ExpressionSet.h>

#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <QAbstractItemModel>
#include <QSharedPointer>
#include <QWidget>

class Component;
class ComponentInstance;
class ComponentInstanceConfigurableElementsModel;

//-----------------------------------------------------------------------------
//! Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------
class ComponentInstanceConfigurableElementsEditor : public QWidget
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
     *      @param [in] parameterExpressions            The expressions for selecting parameter references.
     *      @param [in] moduleParameterExpressions      The expressions for selecting module parameter references.
     *      @param [in] defaultExpressions              The expressions for resolving default values.
     *      @param [in] completionModel                 The completion model for selecting parameter references.
	 *      @param [in] parent                          The parent widget.
	 */
    ComponentInstanceConfigurableElementsEditor(
        ExpressionSet parameterExpressions,
        ExpressionSet moduleParameterExpressions,
        ExpressionSet defaultExpressions,
        QAbstractItemModel* completionModel, QWidget *parent);
	
	/*!
     *  The destructor.
     */
    virtual ~ComponentInstanceConfigurableElementsEditor() = default;

    //! No copying.	No assignment.
    ComponentInstanceConfigurableElementsEditor(const ComponentInstanceConfigurableElementsEditor& other) = delete;
    ComponentInstanceConfigurableElementsEditor& operator=(const ComponentInstanceConfigurableElementsEditor& other) = delete;

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

signals:

    //! Emitted when the content of the editor has changed.
	void contentChanged();

    /*!
     *  Increase the amount of references to a parameter with the matching ID.
     *
     *      @param [in] id  ID of the parameter, whose references are being increased.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the amount of references to a parameter with a matching ID.
     *
     *      @param [in] id  ID of the parameter, whose references are being decreased.
     */
    void decreaseReferences(QString const& id);

private:

    //! The editor for component instance parameters.
	ConfigurableElementEditor parameterEditor_;

    //! The editor for component instance module parameters.
	ConfigurableElementEditor moduleParameterEditor_;

};

#endif // COMPONENTINSTANCECONFIGURABLEELEMENTSEDITOR_H
