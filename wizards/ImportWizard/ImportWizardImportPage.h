//-----------------------------------------------------------------------------
// File: ImportWizardImportPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.10.2014
//
// Description:
// Import wizard page for importing elements from a file.
//-----------------------------------------------------------------------------

#ifndef ImportWizardVHDLIMPORTPAGE_H
#define ImportWizardVHDLIMPORTPAGE_H

#include <wizards/ComponentWizard/ComponentWizardImportPage.h>

#include <QWizardPage>
#include <QSharedPointer>

class Component;
class LibraryInterface;
class ImportEditor;
class PluginManager;

//-----------------------------------------------------------------------------
//! Import wizard page for importing elements from a file.
//-----------------------------------------------------------------------------
class ImportWizardImportPage : public ComponentWizardImportPage
{
    Q_OBJECT
public:
    /*!
     *  Constructor.
     *
     *		@param [in]      component  Pointer to the component to import to.
     *		@param [in]      handler    The library handle.
     *		@param [in]      pluginMgr  Manager for all plugins.
     *      @param [in]      parent     The parent wizard.
     */
    ImportWizardImportPage(QSharedPointer<Component> component,
		 LibraryInterface* handler,
         PluginManager const& pluginMgr,
		 QWidget* parent);

    //! The destructor.
    ~ImportWizardImportPage();

    //!  Returns the ID of the next page.
    virtual int nextId() const;

private:
	// Disable copying.
	ImportWizardImportPage(ImportWizardImportPage const& rhs);
	ImportWizardImportPage& operator=(ImportWizardImportPage const& rhs);

};

#endif // ImportWizardVHDLIMPORTPAGE_H
