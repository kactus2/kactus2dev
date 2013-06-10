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

#include <Vhdl2IPXact/vhdltoipxact.h>

#include <QWizardPage>

class ComponentWizard;

//-----------------------------------------------------------------------------
//! Intro page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardVhdlImportPage : public QWizardPage
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent wizard.
     */
    ComponentWizardVhdlImportPage(ComponentWizard* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardVhdlImportPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

private:
    // Disable copying.
    ComponentWizardVhdlImportPage(ComponentWizardVhdlImportPage const& rhs);
    ComponentWizardVhdlImportPage& operator=(ComponentWizardVhdlImportPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent wizard.
    ComponentWizard* parent_;

    //! The VHDL import editor.
    VHDLtoIPXACT editor_;
};

#endif // COMPONENTWIZARDVHDLIMPORTPAGE_H
