//-----------------------------------------------------------------------------
// File: ComponentWizardImportPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Import page for the component wizard.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDVHDLIMPORTPAGE_H
#define COMPONENTWIZARDVHDLIMPORTPAGE_H

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <QWizardPage>
#include <QSharedPointer>

class Component;
class LibraryInterface;
class ImportEditor;
class PluginManager;

//-----------------------------------------------------------------------------
//! Import page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardImportPage : public QWizardPage
{
    Q_OBJECT
public:
    /*!
     *  The constructor.
     *
     *      @param [in] component               Pointer to the component being edited.
     *      @param [in] handler                 Pointer to the instance which manages the library.
     *      @param [in] pluginMgr               Manager for all plugins.
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] parent                  The parent wizard.
     */
    ComponentWizardImportPage(QSharedPointer<Component> component,
		 LibraryInterface* handler,
         PluginManager const& pluginMgr,
         QSharedPointer <ComponentParameterFinder> parameterFinder,
         QSharedPointer<ExpressionFormatter> expressionFormatter,
		 QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardImportPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

	 /*! \brief Initialize the page to contain correct files to select the top-vhdl.
	  * 
	  * The files saved in the component's file sets are used to select the top-vhdl.
	  * 
	  * Method: 		initializePage
	  * Full name:	ComponentWizardVhdlImportPage::initializePage
	  * Access:		virtual public 
	  *
	  *
	 */
	 virtual void initializePage();


	 /*! \brief Check if the settings on the page are valid and user can move to next page.
	  *
	  * Method: 		isComplete
	  * Full name:	ComponentWizardVhdlImportPage::isComplete
	  * Access:		virtual public 
	  *
	  *
	  * \return True if all settings are valid, otherwise false.
	 */
	 virtual bool isComplete() const;

signals:

     //! Emitted when a new component has been imported.
    void componentChanged(QSharedPointer<Component>);

    /*!
     *  Increase the number of references to the selected parameter.
     *
     *      @param [in] id      The selected parameter.
     */
    void increaseReferences(QString const& id);

    /*!
     *  Decrease the number of references to the selected parameter.
     *
     *      @param [in] id      The selected parameter.
     */
    void decreaseReferences(QString const& id);

private:
	// Disable copying.
	ComponentWizardImportPage(ComponentWizardImportPage const& rhs);
	ComponentWizardImportPage& operator=(ComponentWizardImportPage const& rhs);
    
	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

	//! \brief Editor to set the generics and ports.
	ImportEditor* editor_;
};

#endif // COMPONENTWIZARDVHDLIMPORTPAGE_H
