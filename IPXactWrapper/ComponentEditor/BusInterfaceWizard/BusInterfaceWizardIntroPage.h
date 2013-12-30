//-----------------------------------------------------------------------------
// File: BusInterfaceWizardIntroPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.12.2013
//
// Description:
// Bus interface wizard page for introduction.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEWIZARDINTROPAGE_H
#define BUSINTERFACEWIZARDINTROPAGE_H

#include <QWizardPage>

//-----------------------------------------------------------------------------
//! Introduction page for the interface wizard.
//-----------------------------------------------------------------------------
class BusInterfaceWizardIntroPage : public QWizardPage
{
    Q_OBJECT

public:
    /*!
     *  The constructor.
     */
    BusInterfaceWizardIntroPage(QWidget *parent);

    /*!
     *  The destructor.
     */
    ~BusInterfaceWizardIntroPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

private:
    // Disable copying.
    BusInterfaceWizardIntroPage(BusInterfaceWizardIntroPage const& rhs);
    BusInterfaceWizardIntroPage& operator=(BusInterfaceWizardIntroPage const& rhs);

    /*!
     *  Sets the page layout.
     */
    void setupLayout();
};

#endif // BUSINTERFACEWIZARDINTROPAGE_H

