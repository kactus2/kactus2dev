//-----------------------------------------------------------------------------
// File: ComponentWizardViewsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 09.10.2014
//
// Description:
// Component wizard page for setting up views.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDVIEWSPAGE_H
#define COMPONENTWIZARDVIEWSPAGE_H

#include <QWizardPage>
#include <QSharedPointer>
#include <QTabWidget>
#include <QMap>

class Component;
class ComponentWizard;
class LibraryInterface;
class View;
class ViewEditor;

//-----------------------------------------------------------------------------
//! Component wizard page for setting up views.
//-----------------------------------------------------------------------------
class ComponentWizardViewsPage : public QWizardPage
{
    Q_OBJECT
public:

    /*!
     *  The Constructor.
     *
     *      @param [in] lh     The library interface.
     *      @param [in] parent The parent wizard.
     */
    ComponentWizardViewsPage(LibraryInterface* lh, ComponentWizard* parent);

    //! The destructor.     
    ~ComponentWizardViewsPage();

    /*!
    *  Returns the ID of the next page.
    */
    virtual int nextId() const;

    /*!
    *  Initializes the page.
    */
    virtual void initializePage();
    
    /*!
     *  Returns true if the page is complete and next can be selected.
     */
    virtual bool isComplete() const;

private slots:

    //! Called when a view has been edited.
    virtual void onViewEdited();

private:
    // Disable copying.
    ComponentWizardViewsPage(ComponentWizardViewsPage const& rhs);
    ComponentWizardViewsPage& operator=(ComponentWizardViewsPage const& rhs);

    //! Sets the page layout.
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library interface.
    LibraryInterface* library_;

    //! The parent wizard.
    ComponentWizard* parent_;

    //! The child widget for holding tabs for different views.
    QTabWidget* viewTabs_;

    //! The tab indexes and the views in them.
    QMap<int, QSharedPointer<View> > views_;
};

#endif // COMPONENTWIZARDVIEWSPAGE_H
