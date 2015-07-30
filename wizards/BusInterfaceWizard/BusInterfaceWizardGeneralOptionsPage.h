//-----------------------------------------------------------------------------
// File: BusInterfaceWizardVLNVSelectionPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for bus interface general options.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEWIZARDGENERALOPTIONSPAGE_H
#define BUSINTERFACEWIZARDGENERALOPTIONSPAGE_H

#include <editors/ComponentEditor/busInterfaces/busifgeneraltab.h>
#include <library/LibraryManager/libraryinterface.h>

#include <editors/ComponentEditor/common/ParameterFinder.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <QWizardPage>

class BusInterfaceWizard;
class BusInterface;
class Component;
class LibraryInterface;
class ExpressionParser;
//-----------------------------------------------------------------------------
//! General options page for the interface wizard.
//-----------------------------------------------------------------------------
class BusInterfaceWizardGeneralOptionsPage : public QWizardPage
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] component               The component whose bus interface is being edited.
     *      @param [in] busIf                   The bus interface being edited.
     *      @param [in] lh                      The component library hunter.
     *      @param [in] absDefEditable          The editability of abstract definition.
     *      @param [in] parameterFinder         The parameter finder.
     *      @param [in] expressionFormatter     The expression formatter.
     *      @param [in] parent                  The parent wizard.
     */
    BusInterfaceWizardGeneralOptionsPage(QSharedPointer<Component> component,
        QSharedPointer<BusInterface> busIf, 
        LibraryInterface* lh,
        bool absDefEditable,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        BusInterfaceWizard* parent);

    /*!
    *  Destructor.
    */
    ~BusInterfaceWizardGeneralOptionsPage();

    /*!
    *  Returns the ID of the next page.
    */
    virtual int nextId() const;

    /*!
    *  Initializes the page.
    */
    void initializePage();

    /*!
    * Validates the page.
    */
    virtual bool validatePage();

    /*!
     * Checks if all the required fields on the page are valid.
     *
     *      @return True, if all fields are valid, otherwise false.
     */
    virtual bool isComplete();

signals:

    /*!
     *  Increase the number of references made to a specific parameter.
     *
     *      @param [in] id  The id of the parameter whose reference count is to be increased.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the number of references made to a specific parameter.
     *
     *      @param [in] id  The id of the parameter whose reference count is to be decreased.
     */
    void decreaseReferences(QString id);

private:
    // Disable copying.
    BusInterfaceWizardGeneralOptionsPage(BusInterfaceWizardGeneralOptionsPage const& rhs);
    BusInterfaceWizardGeneralOptionsPage& operator=(BusInterfaceWizardGeneralOptionsPage const& rhs);

   /*!
    * Sets the page layout.
    */
    void setupLayout();
   
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component whose bus interface is being edited.
    QSharedPointer<Component> component_;

    //! The bus interface being edited.
    QSharedPointer<BusInterface> busIf_;

    //! The library handler.
    LibraryInterface* handler_;

    //! Flag for indicating, if a new bus is created.
    bool newBus_;

    //! Editor for bus interface general options.
    BusIfGeneralTab* generalTab_;
};

#endif // BUSINTERFACEWIZARDGENERALOPTIONSPAGE_H
