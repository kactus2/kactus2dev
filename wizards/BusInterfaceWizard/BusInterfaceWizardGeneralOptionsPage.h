//-----------------------------------------------------------------------------
// File: BusInterfaceWizardVLNVSelectionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for bus interface general options.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEWIZARDGENERALOPTIONSPAGE_H
#define BUSINTERFACEWIZARDGENERALOPTIONSPAGE_H

#include <editors/ComponentEditor/busInterfaces/busifgeneraltab.h>
#include <library/LibraryManager/libraryinterface.h>

#include <QWizardPage>

class BusInterfaceWizard;
class BusInterface;
class Component;
class LibraryInterface;
//-----------------------------------------------------------------------------
//! General options page for the interface wizard.
//-----------------------------------------------------------------------------
class BusInterfaceWizardGeneralOptionsPage : public QWizardPage
{

public:

    /*!
     *  The constructor.
     *
     *      @param [in] component   The component whose bus interface is being edited.
     *      @param [in] busIf       The bus interface being edited.
     *      @param [in] lh          The component library handler.
     *      @param [in] parent      The parent wizard.
     */
    BusInterfaceWizardGeneralOptionsPage(QSharedPointer<Component> component, QSharedPointer<BusInterface> busIf, 
        LibraryInterface* lh, bool absDefEditable, BusInterfaceWizard* parent);

    /*!
    *  Destructor.
    */
    ~BusInterfaceWizardGeneralOptionsPage();

    /*!
    *  Returns the ID of the next page.
    */
    virtual int nextId() const;

    /*!
    *  Initializes the page.
    */
    void initializePage();

    /*!
    * Validates the page.
    */
    virtual bool validatePage();

    /*!
     * Checks if all the required fields on the page are valid.
     *
     *      @return True, if all fields are valid, otherwise false.
     */
    virtual bool isComplete();


private:
    // Disable copying.
    BusInterfaceWizardGeneralOptionsPage(BusInterfaceWizardGeneralOptionsPage const& rhs);
    BusInterfaceWizardGeneralOptionsPage& operator=(BusInterfaceWizardGeneralOptionsPage const& rhs);

   /*!
    * Sets the page layout.
    */
    void setupLayout();
   

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Flag for indicating, if a new bus is created.
    bool newBus_;

    //! Editor for bus interface general options.
    BusIfGeneralTab* generalTab_;

    //! The library handler.
    LibraryInterface* handler_;

    //! The component whose bus interface is being edited.
    QSharedPointer<Component> component_;

    //! The bus interface being edited.
    QSharedPointer<BusInterface> busIf_;

};

#endif // BUSINTERFACEWIZARDGENERALOPTIONSPAGE_H
