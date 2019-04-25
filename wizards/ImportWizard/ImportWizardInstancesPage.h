//-----------------------------------------------------------------------------
// File: ImportWizardInstancesPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 02.04.2019
//
// Description:
// Import wizard page for importing instance elements from a file.
//-----------------------------------------------------------------------------

#ifndef IMPORTWIZARDINSTANCESPAGE_H
#define IMPORTWIZARDINSTANCESPAGE_H

#include <QWizardPage>

class Component;
class ImportInstancesEditor;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Import wizard page for importing instance elements from a file.
//-----------------------------------------------------------------------------
class ImportWizardInstancesPage : public QWizardPage
{
    Q_OBJECT
public:

    /*!
     *  The Constructor.
     *
     *      @param [in] workingComponent    Pointer to the component to import to.
     *      @param [in] libraryAccess       The library handle.
     *      @param [in] parent              The parent wizard.
     */
    ImportWizardInstancesPage(QSharedPointer<Component> workingComponent, LibraryInterface* libraryAccess,
        QWidget* parent);

    /*!
     *  The destructor.
     */
    ~ImportWizardInstancesPage() = default;

    /*!
     *  Initialize the page.
     */
    virtual void initializePage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

    // Disable copying.
    ImportWizardInstancesPage(ImportWizardInstancesPage const& rhs) = delete;
    ImportWizardInstancesPage& operator=(ImportWizardInstancesPage const& rhs) = delete;

private:

    //! Editor for import instances.
    ImportInstancesEditor* editor_;
};

#endif // IMPORTWIZARDINSTANCESPAGE_H
