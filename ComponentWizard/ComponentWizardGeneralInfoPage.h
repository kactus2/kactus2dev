//-----------------------------------------------------------------------------
// File: ComponentWizardGeneralInfoPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.06.2013
//
// Description:
// General component information page for the component wizard.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDGENERALINFOPAGE_H
#define COMPONENTWIZARDGENERALINFOPAGE_H

#include <models/component.h>

#include <QWizardPage>
#include <QSharedPointer>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>

class ComponentWizard;
class LibraryInterface;
class VhdlParserWidget;
class FileSelector;
class FileViewer;
//-----------------------------------------------------------------------------
//! Intro page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardGeneralInfoPage : public QWizardPage
{
    Q_OBJECT

public:
 
   /*!
     *  Constructor.
     *
     *		@param [in, out] component Pointer to the component being edited.
     *      @param [in] parent The parent wizard.
     */
    ComponentWizardGeneralInfoPage(QSharedPointer<Component> component,
		 ComponentWizard* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardGeneralInfoPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

	 /*! \brief Initialize the page to contain correct files to select the top-vhdl.
	  * 
	  * The files saved in the component's file sets are used to select the top-vhdl.
	  * 
	  * Method: 		initializePage
	  * Full name:	ComponentWizardGeneralInfoPage::initializePage
	  * Access:		virtual public 
	  *
	  *
	 */
	 virtual void initializePage();

	 /*! \brief Check if the settings on the page are valid and user can move to next page.
	  *
	  * Method: 		isComplete
	  * Full name:	ComponentWizardGeneralInfoPage::isComplete
	  * Access:		virtual public 
	  *
	  *
	  * \return True if all settings are valid, otherwise false.
	 */
     virtual bool isComplete() const;


    /*!
     *   Assigns the input values to the component when next is clicked.
     */
     virtual bool validatePage();

private slots:
    
    //! Called when show/hide button is clicked.
    void onShowHide();

private:
	// Disable copying.
	ComponentWizardGeneralInfoPage(ComponentWizardGeneralInfoPage const& rhs);
	ComponentWizardGeneralInfoPage& operator=(ComponentWizardGeneralInfoPage const& rhs);

    /*!
     *   Creates layout for the page.
     */
    void setupLayout();

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

	//! The parent wizard.
	ComponentWizard* parent_;

    //! The component created in the wizard.
    QSharedPointer<Component> component_;

    //! Editor for inputting author of the component.
    QLineEdit authorEditor_;

    //! Editor for inputting a description for the component.
    QTextEdit descriptionEditor_;

    //! Button to show/hide file viewer.
    QPushButton* showHideButton_;

    //! Widget for browsing files in fileset.
    FileViewer* fileViewer_;
};

#endif // COMPONENTWIZARDGENERALINFOPAGE_H
