//-----------------------------------------------------------------------------
// File: ComponentWizard.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Component wizard dialog.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARD_H
#define COMPONENTWIZARD_H

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>
#include <editors/ComponentEditor/referenceCounter/ParameterReferenceCounter.h>

#include <QWizard>

class Component;
class PluginManager;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Component wizard dialog.
//-----------------------------------------------------------------------------
class ComponentWizard : public QWizard
{
    Q_OBJECT

public:
 
	 /*!
	 *  Constructor.
	 *
	 *      @param [in] component  The component for which the wizard is run.
	 *      @param [in] basePath   The base path where the component XML will be located.
	 *      @param [in] pluginMgr  The plugin manager.
	 *      @param [in] handler	   Pointer to the instance which manages the library.
	 *      @param [in] parent     The parent widget.
	 */
	 ComponentWizard(QSharedPointer<Component> component, QString const& basePath,
		             PluginManager const& pluginMgr, LibraryInterface* handler, 
		             QWidget* parent);

	 /*!
	 *  Destructor.
	 */
	 ~ComponentWizard();

    /*!
     *  Returns the component.
     */
    QSharedPointer<Component> getComponent();

protected:

    /*!
     *  Called when back has been selected on a wizard page.
     *
     *      @param [in] id   The id of the page.
     */
    virtual void cleanupPage(int id);

private slots:

    /*!
     *  Called when a new component has been imported.
     *
     *      @param [in] component   The newly imported component.
     */
    virtual void onComponentChanged(QSharedPointer<Component> component);

private:
    // Disable copying.
    ComponentWizard(ComponentWizard const& rhs);
    ComponentWizard& operator=(ComponentWizard const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The original component before import.
    QSharedPointer<Component> originalComponent_;

    //! The working copy of a component after import.
    QSharedPointer<Component> workingComponent_;

    //! The parameter finder used for finding parameters with parameter ids.
    QSharedPointer <ComponentParameterFinder> parameterFinder_;

    //! The expression formatter, used for showing the expressions as names instead of ids.
    QSharedPointer<ExpressionFormatter> expressionFormatter_;

    //! The counter that changes the amount of references to parameters.
    QSharedPointer<ReferenceCounter> referenceCounter_;
};

#endif // COMPONENTWIZARD_H
