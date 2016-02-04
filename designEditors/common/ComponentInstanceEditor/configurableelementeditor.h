//-----------------------------------------------------------------------------
// File: configurableelementeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
#include <editors/ComponentEditor/common/ListParameterFinder.h>

#include "ConfigurableElementsModel.h"
#include "configurableelementdelegate.h"
#include "ConfigurableElementsView.h"

#include <QAbstractItemModel>
#include <QGroupBox>
#include <QSharedPointer>

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
	 *      @param [in] listFinder                      The finder for the configurable element values.
	 *      @param [in] parameterFinder                 The finder for configurable elements and top parameters.
	 *      @param [in] configurableElementFormatter    Formats referencing expressions in configurable elements.
	 *      @param [in] instanceExpressionFormatter     Formats referencing expressions in component instance.
	 *      @param [in] expressionParser                Solves expressions in configurable elements.
	 *      @param [in] instanceParser                  Solves expressions in default values (component instance).
	 *      @param [in] completionModel                 The completion model for selecting parameter references.
	 *      @param [in] parent                          The parent widget.
	 */
	ConfigurableElementEditor(QSharedPointer<ListParameterFinder> listFinder,
        QSharedPointer<ParameterFinder> parameterFinder, 
        QSharedPointer<ExpressionFormatter> configurableElementFormatter,
        QSharedPointer<ExpressionFormatter> instanceExpressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionParser> instanceParser,
        QAbstractItemModel* completionModel,
        QWidget *parent);
	
	//! The destructor.
	virtual ~ConfigurableElementEditor();

	/*!
     *  Set the component instance to be edited.
	 *
	 *      @param [in] component       Pointer to the component referenced by the component instance.
     *      @param [in] instance        Pointer to the component instance to edit.
     *      @param [in] editProvider    Pointer to the editing capabilities.
	 */
    void setComponent(QSharedPointer<Component> component, QSharedPointer<ComponentInstance> instance,
        QSharedPointer<IEditProvider> editProvider);

	/*!
     *  Clear the editor from all data.
	 */
	void clear();

    /*!
     *  Set the design configuration model for the configurable element model.
     *
     *      @param [in] designConfiguration     The design configuration to be set.
     */
    void setDesignConfigurationToModel(QSharedPointer<DesignConfiguration> designConfiguration);

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
	ConfigurableElementsModel model_;

    //! The delegate used in the display widget.
    QSharedPointer<ConfigurableElementDelegate> delegate_;
};

#endif // CONFIGURABLEELEMENTEDITOR_H
