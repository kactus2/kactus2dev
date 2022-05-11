//-----------------------------------------------------------------------------
// File: ImportWizard.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.10.2014
//
// Description:
// Wizard for component import.
//-----------------------------------------------------------------------------

#ifndef IMPORTWIZARD_H
#define IMPORTWIZARD_H

#include <KactusAPI/include/ComponentParameterFinder.h>
#include <editors/ComponentEditor/referenceCounter/ParameterReferenceCounter.h>

#include <QWizard>
#include <QSharedPointer>

class Component;
class LibraryInterface;

//-----------------------------------------------------------------------------
//! Component wizard dialog.
//-----------------------------------------------------------------------------
class ImportWizard : public QWizard
{
    Q_OBJECT

public:
 
    /*!
     *  Constructor.
     *
     *      @param [in] component  The component for which the wizard is run.
     *      @param [in] handler	   The library handle.
     *      @param [in] parent     The parent widget.
     */
    ImportWizard(QSharedPointer<Component> component, LibraryInterface* handler, QWidget* parent);

    /*!
     *  The destructor.
     */
    ~ImportWizard();

    /*!
     *  Returns the component after import modifications.
     */
    QSharedPointer<Component> getComponent();

private slots:

    /*!
     *  Called when a new component has been imported.
     *
     *      @param [in] component   The newly imported component.
     */
    virtual void onComponentChanged(QSharedPointer<Component> component);

private:
    // Disable copying.
    ImportWizard(ImportWizard const& rhs);
    ImportWizard& operator=(ImportWizard const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The working copy of a component after import.
    QSharedPointer<Component> workingComponent_;

    //! Parameter finder, finds the desired parameters.
    QSharedPointer <ComponentParameterFinder> updatingFinder_;

    //! The counter that changes the amount of references to parameters.
    QSharedPointer<ReferenceCounter> referenceCounter_;
};

#endif // ImportWizard_H
