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

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>

#include <QWizardPage>
#include <QSharedPointer>

class Component;
class LibraryInterface;
class ImportEditor;
class BusInterfaceInterface;

//-----------------------------------------------------------------------------
//! Import wizard page for importing elements from a file.
//-----------------------------------------------------------------------------
class ImportWizardImportPage : public ComponentWizardImportPage
{
    Q_OBJECT
public:
    /*!
     *  The Constructor.
     *
     *    @param [in] component               Pointer to the component to import to.
     *    @param [in] handler                 The library handle.
     *    @param [in] parameterFinder         The parameter finder.
     *    @param [in] expressionFormatter     The expression formatter.
     *    @param [in] busInterface            Interface for accessing bus interfaces.
     *    @param [in] parent                  The parent wizard.
     */
    ImportWizardImportPage(QSharedPointer<Component> component,
        LibraryInterface* handler,
        QSharedPointer <ComponentParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        BusInterfaceInterface* busInterface,
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
