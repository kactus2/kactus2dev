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

#include <QWizard>

class Component;
class PluginManager;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Component wizard dialog.
//-----------------------------------------------------------------------------
class ComponentWizard : public QWizard
{
public:
    enum Page
    {
        PAGE_INTRO = 0,
        PAGE_FILES,
        PAGE_DEPENDENCY,
        PAGE_VHDL_IMPORT,
        PAGE_CONCLUSION
	 };

	 /*!
	 *  Constructor.
	 *
	 *      @param [in] component  The component for which the wizard is run.
	 *      @param [in] basePath   The base path where the component XML will be located.
	 *      @param [in] pluginMgr  The plugin manager.
	 *      @param [in] handler	 Pointer to the instance which manages the library.
	 *      @param [in] parent     The parent widget.
	 */
	 ComponentWizard(QSharedPointer<Component> component,
		 QString const& basePath,
		 PluginManager& pluginMgr,
		 LibraryInterface* handler, 
		 QWidget* parent);

	 /*!
	 *  Destructor.
	 */
	 ~ComponentWizard();

    /*!
     *  Returns the component.
     */
    QSharedPointer<Component> getComponent();

    /*!
     *  Returns the base path.
     */
    QString const& getBasePath() const;

private:
    // Disable copying.
    ComponentWizard(ComponentWizard const& rhs);
    ComponentWizard& operator=(ComponentWizard const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component.
    QSharedPointer<Component> component_;

    //! The component base path.
    QString basePath_;
};

#endif // COMPONENTWIZARD_H
