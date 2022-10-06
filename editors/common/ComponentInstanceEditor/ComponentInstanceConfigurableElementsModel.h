//-----------------------------------------------------------------------------
// File: ComponentInstanceConfigurableElementsModel.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 17.08.2017
//
// Description:
// Model class to manage the configurable element values of component instances.
//-----------------------------------------------------------------------------

#ifndef COMPONENTINSTANCECONFIGURABLEELEMENTSMODEL_H
#define COMPONENTINSTANCECONFIGURABLEELEMENTSMODEL_H

#include <KactusAPI/include/ExpressionFormatter.h>
#include <editors/ComponentEditor/common/ConfigurableElementFinder.h>
#include <editors/common/ComponentInstanceEditor/ConfigurableElementsModel.h>

#include <IPXACTmodels/common/validators/ParameterValidator.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>

#include <QAbstractTableModel>
#include <QList>
#include <QString>
#include <QSharedPointer>
#include <QSortFilterProxyModel>

class Parameter;
class ModuleParameter;
class Choice;
class ComponentInstance;
class Component;

//-----------------------------------------------------------------------------
//! Model class to manage the configurable element values of component instances.
//-----------------------------------------------------------------------------
class ComponentInstanceConfigurableElementsModel : public ConfigurableElementsModel
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterFinder             The parameter finder.
	 *      @param [in] elementFinder               Configurable elements finder.
	 *      @param [in] elementExpressionFormatter  Expression formatter for configurable element values.
	 *      @param [in] defaultValueFormatter       Expression formatter for default values.
	 *      @param [in] elementExpressionParser     Parser for configurable element values.
	 *      @param [in] defaultValueParser          Parser for default values.
	 *      @param [in] parent                      The parent object.
	 */
	ComponentInstanceConfigurableElementsModel(QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ConfigurableElementFinder> elementFinder,
        QSharedPointer<ExpressionFormatter> elementExpressionFormatter,
        QSharedPointer<ExpressionFormatter> defaultValueFormatter,
        QSharedPointer<ExpressionParser> elementExpressionParser,
        QSharedPointer<ExpressionParser> defaultValueParser, QObject* parent);

	/*!
	 *  The destructor.
	 */
    virtual ~ComponentInstanceConfigurableElementsModel() = default;

	/*!
	 *  Set the parameters for configurable element values.
	 *
	 *      @param [in] component           Pointer to the component referenced by the component instance.
     *      @param [in] instance            Pointer to the component instance being edited.
     *      @param [in] viewConfiguration   Pointer to the active view of the instanced component.
	 */
    void setParameters(QSharedPointer<Component> component, QSharedPointer<ComponentInstance> instance,
        QSharedPointer<ViewConfiguration> viewConfiguration);

signals:

    /*!
     *  Signal for connecting the root item models to the flag for displaying immediate values.
     *
     *      @param [in] show    Flag for displaying the immediate values.
     */
    void showImmediateValuesInModels(bool show);

    /*!
     *  Emitted to invalidate the assigned filter.
     */
    void invalidateFilter();

private:

    //! No copying. //! No assignment.
    ComponentInstanceConfigurableElementsModel(const ComponentInstanceConfigurableElementsModel& other);
    ComponentInstanceConfigurableElementsModel& operator=(const ComponentInstanceConfigurableElementsModel& other);

    /*!
     *  Get the list of contained module parameters.
     *
     *      @param [in] component           The component containing the module parameters.
     *      @param [in] viewConfiguration   View configuration assigned to the component instance.
     *
     *      @return Module parameters contained within the selected view configuration.
     */
    QSharedPointer<QList<QSharedPointer<Parameter> > > getModuleParameters(
        QSharedPointer<Component> component, QSharedPointer<ViewConfiguration> viewConfiguration);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The configurable element finder.
    QSharedPointer<ConfigurableElementFinder> elementFinder_;
};

#endif // COMPONENTINSTANCECONFIGURABLEELEMENTSMODEL_H
