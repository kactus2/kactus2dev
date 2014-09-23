//-----------------------------------------------------------------------------
// File: ComponentWizardIntroPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Intro page for the component wizard.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDINTROPAGE_H
#define COMPONENTWIZARDINTROPAGE_H

#include <QWizardPage>
#include <QLabel>

class Component;

//-----------------------------------------------------------------------------
//! Intro page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardIntroPage : public QWizardPage
{
public:
    /*!
     *  Constructor.
     *
     *		@param [in, out] component  The component being edited.
     *      @param [in] parent          The parent widget.
     */
    ComponentWizardIntroPage(QSharedPointer<Component> component, QWidget* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardIntroPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

private:
    // Disable copying.
    ComponentWizardIntroPage(ComponentWizardIntroPage const& rhs);
    ComponentWizardIntroPage& operator=(ComponentWizardIntroPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The label for information.
    QLabel infoLabel_;
};

#endif // COMPONENTWIZARDINTROPAGE_H
