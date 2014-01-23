//-----------------------------------------------------------------------------
// File: BusInterfaceWizardConclusionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.12.2013
//
// Description:
// Bus interface wizard page for summary.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEWIZARDCONCLUSIONSPAGE_H
#define BUSINTERFACEWIZARDCONCLUSIONSPAGE_H

#include "BusInterfaceWizard.h"

#include <models/businterface.h>

#include <QLabel>
#include <QWizardPage>

//-----------------------------------------------------------------------------
//! Summary page for the interface wizard.
//-----------------------------------------------------------------------------
class BusInterfaceWizardConclusionPage : public QWizardPage
{
    Q_OBJECT

public:

    /*!
     *  <Description>.
     *
     *      @param [in] busIf   The bus interface being edited.
     *      @param [in] ports   The ports selected to the wizard.
     *      @param [in] parent  The parent wizard.
     *
     *      @return 
     */
    BusInterfaceWizardConclusionPage(QSharedPointer<BusInterface> busIf, 
        QStringList ports,
        BusInterfaceWizard* parent);

    /*!
     *  The destructor.
     */
    ~BusInterfaceWizardConclusionPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

    /*!
     *  Initializes the page.
     */
    virtual void initializePage();

private:
    
    // Disable copying.
    BusInterfaceWizardConclusionPage(BusInterfaceWizardConclusionPage const& rhs);
    BusInterfaceWizardConclusionPage& operator=(BusInterfaceWizardConclusionPage const& rhs);

    /*!
     *  Sets the page layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    //! Data.
    //-----------------------------------------------------------------------------

    //! The bus interface being edited.
    QSharedPointer<BusInterface> busIf_;

    //! The ports selected to the wizard.
    QStringList ports_;

    //! Label for bus interface name.
    QLabel nameLabel_;

    //! Label for bus interface mode.
    QLabel modeLabel_;

    //! Label for bus interface bus definition.
    QLabel busDefLabel_;

    //! Label for bus interface abstraction definition.
    QLabel absDefLabel_;

    //! Label for bus interface port map count.
    QLabel portMapLabel_;

};

#endif // BUSINTERFACEWIZARDCONCLUSIONSPAGE_H
