//-----------------------------------------------------------------------------
// File: ImportWizardIntroPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.10.2014
//
// Description:
// Intro page for the import wizard.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDINTROPAGE_H
#define COMPONENTWIZARDINTROPAGE_H

#include <QWizardPage>
#include <QLabel>

class Component;

//-----------------------------------------------------------------------------
//! Intro page for the import wizard.
//-----------------------------------------------------------------------------
class ImportWizardIntroPage : public QWizardPage
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent          The parent widget.
     */
    ImportWizardIntroPage(QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ImportWizardIntroPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

private:
    // Disable copying.
    ImportWizardIntroPage(ImportWizardIntroPage const& rhs);
    ImportWizardIntroPage& operator=(ImportWizardIntroPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The label for information.
    QLabel infoLabel_;
};

#endif // COMPONENTWIZARDINTROPAGE_H
