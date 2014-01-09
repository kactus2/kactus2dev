//-----------------------------------------------------------------------------
// File: BusInterfaceWizardPortMapPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for defining port maps.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEWIZARDPORTMAPPAGE_H
#define BUSINTERFACEWIZARDPORTMAPPAGE_H

#include <IPXactWrapper/ComponentEditor/busInterfaces/busifportmaptab.h>

#include <QWizardPage>
#include <QLabel>
#include <QTextEdit>

class BusInterfaceWizard;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Port map page for the bus interface wizard.
//-----------------------------------------------------------------------------
class BusInterfaceWizardPortMapPage : public QWizardPage
{
    Q_OBJECT
public:


    /*!
     *  The constructor.
     *
     *      @param [in] component   The component whose bus interface is being edited.
     *      @param [in] busIf       The bus interface being edited.
     *      @param [in] lh          The component library handler.
     *      @param [in] parent      The parent wizard.
     */
    BusInterfaceWizardPortMapPage(QSharedPointer<Component> component, QSharedPointer<BusInterface> busIf, 
        LibraryInterface* lh, BusInterfaceWizard* parent);

    /*!
    *  Destructor.
    */
    ~BusInterfaceWizardPortMapPage();

    /*!
    *  Returns the ID of the next page.
    */
    virtual int nextId() const;

    /*!
    *  Initializes the page.
    */
    virtual void initializePage();

    /*!
    *  Validates the page.
    */
    virtual bool validatePage();

private slots:
    
    //! Handler for error messages.
    virtual void showErrorMessage(const QString& msg);

private:
    // Disable copying.
    BusInterfaceWizardPortMapPage(BusInterfaceWizardPortMapPage const& rhs);
    BusInterfaceWizardPortMapPage& operator=(BusInterfaceWizardPortMapPage const& rhs);

    /*!
    *  Sets the page layout.
    */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The parent wizard.
    BusInterfaceWizard* parent_;

    //! The library interface.
    LibraryInterface* handler_;

    //! The widget for editing port maps.
    BusIfPortmapTab portMapTab_;

    //! The target bus interface.
    QSharedPointer<BusInterface> busIf_;

    //! The component whose interface is being edited.
    QSharedPointer<Component> component_;
};

#endif // BUSINTERFACEWIZARDPORTMAPPAGE_H
