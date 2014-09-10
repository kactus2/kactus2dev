//-----------------------------------------------------------------------------
// File: ComponentWizardVhdlImportPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// VHDL import page for the component wizard.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDVHDLIMPORTPAGE_H
#define COMPONENTWIZARDVHDLIMPORTPAGE_H


#include <QWizardPage>
#include <QSharedPointer>

class Component;
class LibraryInterface;
class VhdlImportEditor;
//-----------------------------------------------------------------------------
//! Intro page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardVhdlImportPage : public QWizardPage
{
public:
    /*!
     *  Constructor.
     *
     *		@param [in, out] component  Pointer to the component being edited.
     *		@param [in]      handler    Pointer to the instance which manages the library.
     *      @param [in]      parent     The parent wizard.
     */
    ComponentWizardVhdlImportPage(QSharedPointer<Component> component,
		 LibraryInterface* handler,
		 QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardVhdlImportPage();

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


private:
	// Disable copying.
	ComponentWizardVhdlImportPage(ComponentWizardVhdlImportPage const& rhs);
	ComponentWizardVhdlImportPage& operator=(ComponentWizardVhdlImportPage const& rhs);

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

	//! \brief Editor to set the generics and ports.
	VhdlImportEditor* editor_;
};

#endif // COMPONENTWIZARDVHDLIMPORTPAGE_H
