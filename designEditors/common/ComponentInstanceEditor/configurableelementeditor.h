//-----------------------------------------------------------------------------
// File: configurableelementeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 12.08.2011
//
// Description:
// Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------

#ifndef CONFIGURABLEELEMENTEDITOR_H
#define CONFIGURABLEELEMENTEDITOR_H

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <editors/ComponentEditor/common/ParameterCompleter.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ConfigurableElementFinder.h>

#include <designEditors/common/ComponentInstanceEditor/ComponentInstanceConfigurableElementsModel.h>
#include <designEditors/common/ComponentInstanceEditor/configurableelementdelegate.h>
#include <designEditors/common/ComponentInstanceEditor/ConfigurableElementsView.h>

#include <QAbstractItemModel>
#include <QGroupBox>
#include <QSharedPointer>
#include <QCheckBox>

class ExpressionParser;
class ParameterFinder;
class Component;
class ComponentInstance;
//-----------------------------------------------------------------------------
//! Editor for configurable elements of a component instance.
//-----------------------------------------------------------------------------
class ConfigurableElementEditor : public QGroupBox
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
    ConfigurableElementEditor(QSharedPointer<ConfigurableElementFinder> elementFinder,
        QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionFormatter> configurableElementFormatter,
        QSharedPointer<ExpressionFormatter> instanceExpressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionParser> instanceParser,
        QAbstractItemModel* completionModel, QWidget *parent);
	
	//! The destructor.
	virtual ~ConfigurableElementEditor();

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
	void clear();

signals:

	//! Emitted when contents of the editor changes.
	void contentChanged();

private:
	//! No copying.
	ConfigurableElementEditor(const ConfigurableElementEditor& other);

	//! No assignment.
	ConfigurableElementEditor& operator=(const ConfigurableElementEditor& other);

	//! The widget to display the contents of the model.
    ConfigurableElementsView view_;

	//! The model to edit the configurable elements of a component instance.
    ComponentInstanceConfigurableElementsModel model_;

    //! The delegate used in the display widget.
    ConfigurableElementDelegate* delegate_;

    QCheckBox* filterSelection_;
};

#endif // CONFIGURABLEELEMENTEDITOR_H
