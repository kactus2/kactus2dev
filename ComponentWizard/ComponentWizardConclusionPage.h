//-----------------------------------------------------------------------------
// File: ComponentWizardConclusionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Conclusion page for the component wizard.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDCONCLUSIONPAGE_H
#define COMPONENTWIZARDCONCLUSIONPAGE_H

#include <QWizardPage>

class ComponentWizard;

//-----------------------------------------------------------------------------
//! Conclusion page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardConclusionPage : public QWizardPage
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent wizard.
     */
    ComponentWizardConclusionPage(ComponentWizard* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardConclusionPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

private:
    // Disable copying.
    ComponentWizardConclusionPage(ComponentWizardConclusionPage const& rhs);
    ComponentWizardConclusionPage& operator=(ComponentWizardConclusionPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent wizard.
    ComponentWizard* parent_;
};

#endif // COMPONENTWIZARDCONCLUSIONPAGE_H
