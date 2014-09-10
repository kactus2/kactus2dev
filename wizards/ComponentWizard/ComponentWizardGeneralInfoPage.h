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

#include <IPXACTmodels/component.h>

#include <QWizardPage>
#include <QSharedPointer>
#include <QLineEdit>
#include <QTextEdit>

class ComponentWizard;
class LibraryInterface;
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
     *      @param [in] parent The parent widget.
     */
    ComponentWizardGeneralInfoPage(QSharedPointer<Component> component, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardGeneralInfoPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

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

    //! The component created in the wizard.
    QSharedPointer<Component> component_;

    //! Editor for inputting author of the component.
    QLineEdit authorEditor_;

    //! Editor for inputting a description for the component.
    QTextEdit descriptionEditor_;
};

#endif // COMPONENTWIZARDGENERALINFOPAGE_H
