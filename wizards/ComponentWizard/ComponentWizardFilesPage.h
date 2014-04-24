//-----------------------------------------------------------------------------
// File: ComponentWizardFilesPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 06.06.2013
//
// Description:
// Files page for the component wizard.
//-----------------------------------------------------------------------------

#ifndef COMPONENTWIZARDFILESPAGE_H
#define COMPONENTWIZARDFILESPAGE_H

#include <editors/ComponentEditor/fileSet/dependencyAnalysis/FileDependencySourceEditor.h>

#include <QWizardPage>

class ComponentWizard;

//-----------------------------------------------------------------------------
//! Files page for the component wizard.
//-----------------------------------------------------------------------------
class ComponentWizardFilesPage : public QWizardPage
{
public:
    /*!
     *  Constructor.
     *
     *      @param [in] parent The parent wizard.
     */
    ComponentWizardFilesPage(ComponentWizard* parent);

    /*!
     *  Destructor.
     */
    ~ComponentWizardFilesPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

    /*!
     *  Validates the contents of the page.
     */
    virtual bool validatePage();

private:
    // Disable copying.
    ComponentWizardFilesPage(ComponentWizardFilesPage const& rhs);
    ComponentWizardFilesPage& operator=(ComponentWizardFilesPage const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The parent wizard.
    ComponentWizard* parent_;

    //! The source directories editor.
    FileDependencySourceEditor editor_;
};

#endif // COMPONENTWIZARDFILESPAGE_H
