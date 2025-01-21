//-----------------------------------------------------------------------------
// File: BusInterfaceWizardBusDefinitionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for editing bus and abstraction definitions.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEWIZARDBUSDEFINITIONPAGE_H
#define BUSINTERFACEWIZARDBUSDEFINITIONPAGE_H

#include <editors/BusDefinitionEditor/BusDefinitionEditor.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/DirectionTypes.h>

#include <QWizardPage>
#include <QMap>
#include <QPushButton>

class AbstractionDefinition;
class BusDefinition;
class BusInterface;
class BusInterfaceWizard;
class Component;
class ExpressionParser;
class LibraryInterface;
class Port;
class PortAbstraction;
class VLNV;

//-----------------------------------------------------------------------------
//! Bus editor page for the interface wizard.
//-----------------------------------------------------------------------------
class BusInterfaceWizardBusDefinitionEditorPage : public QWizardPage
{
    Q_OBJECT
public:

    /*!
     *  The constructor.
     *
     *    @param [in] component       The component whose bus interface is being edited.
     *    @param [in] busIf           The bus interface being edited.
     *    @param [in] lh              The component library handler.
     *    @param [in] parent          The parent wizard.
     */
    BusInterfaceWizardBusDefinitionEditorPage(QSharedPointer<Component> component,
        QSharedPointer<BusInterface> busIf, LibraryInterface* lh, bool editable,
        BusInterfaceWizard* parent);

    /*!
     *  Destructor.
     */
    ~BusInterfaceWizardBusDefinitionEditorPage() final = default;

    // Disable copying.
    BusInterfaceWizardBusDefinitionEditorPage(BusInterfaceWizardBusDefinitionEditorPage const& rhs) = delete;
    BusInterfaceWizardBusDefinitionEditorPage& operator=(BusInterfaceWizardBusDefinitionEditorPage const& rhs) = delete;

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

    /*!
     *  Initializes the page.
     */
    virtual void initializePage();

    /*!
     *  Validates the page.
     */
    virtual bool validatePage();

private:

    /*!
    *  Sets the page layout.
    */
    void setupLayout();


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The library handler.
    LibraryInterface* handler_;

    //! The component whose bus interface is being edited.
    QSharedPointer<Component> component_;

    //! The bus interface being edited.
    QSharedPointer<BusInterface> busIf_;

    //! The editor for bus and abstraction definition.
    BusDefinitionEditor editor_;

    //! Flag for indicating changes on the page. 
    bool editable_ = false;
};

#endif // BUSINTERFACEWIZARDBUSDEFINITIONPAGE_H
