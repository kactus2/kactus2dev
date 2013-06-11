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

#include <common/widgets/componentPreviewBox/ComponentPreviewBox.h>

#include <QWizardPage>
#include <QLabel>

class ComponentWizard;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Conclusion page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardConclusionPage : public QWizardPage
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] lh     The library interface.
     *      @param [in] parent The parent wizard.
     */
    ComponentWizardConclusionPage(LibraryInterface* lh, ComponentWizard* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardConclusionPage();

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
    ComponentWizardConclusionPage(ComponentWizardConclusionPage const& rhs);
    ComponentWizardConclusionPage& operator=(ComponentWizardConclusionPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent wizard.
    ComponentWizard* parent_;

    //! The summary text.
    QLabel summaryLabel_;

    //! The component preview box.
    ComponentPreviewBox previewBox_;
};

#endif // COMPONENTWIZARDCONCLUSIONPAGE_H
