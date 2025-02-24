//-----------------------------------------------------------------------------
// File: BusInterfaceWizardGeneralOptionsPage.h
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

#include <editors/ComponentEditor/busInterfaces/general/busifgeneraltab.h>
#include <KactusAPI/include/LibraryInterface.h>

#include <KactusAPI/include/ParameterFinder.h>
#include <KactusAPI/include/ExpressionFormatter.h>

#include <QWizardPage>

class BusInterface;
class BusInterfaceWizard;
class Component;
class LibraryInterface;
class ExpressionParser;
class AbstractionTypeValidator;
class BusInterfaceInterface;

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
     *    @param [in] component               The component whose bus interface is being edited.
     *    @param [in] busIf                   The bus interface being edited.
     *    @param [in] lh                      The component library handler.
     *    @param [in] absDefEditable          The editability of abstract definition.
     *    @param [in] parameterFinder         The parameter finder.
     *    @param [in] expressionFormatter     The expression formatter.
     *    @param [in] busValidator            Validator for bus interfaces.
     *    @param [in] busInterface            Interface for accessing bus interfaces.
     *    @param [in] parent                  The parent wizard.
     */
    BusInterfaceWizardGeneralOptionsPage(QSharedPointer<Component> component,
        QSharedPointer<BusInterface> busIf,
        LibraryInterface* lh,
        bool absDefEditable,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionFormatter> expressionFormatter,
        QSharedPointer<ExpressionParser> expressionParser,
        BusInterfaceInterface* busInterface,
        BusInterfaceWizard* parent);

    /*!
     *  Destructor.
     */
    ~BusInterfaceWizardGeneralOptionsPage() final = default;

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

    /*!
     *  Initializes the page.
     */
    void initializePage();

    /*!
     * Checks if all the required fields on the page are valid.
     *
     *    @return True, if all fields are valid, otherwise false.
     */
    virtual bool isComplete() const;    

signals:

    /*!
     *  Increase the number of references made to a specific parameter.
     *
     *    @param [in] id  The id of the parameter whose reference count is to be increased.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the number of references made to a specific parameter.
     *
     *    @param [in] id  The id of the parameter whose reference count is to be decreased.
     */
    void decreaseReferences(QString id);

    /*!
     *  Inform of a change in the name of the bus interface.
     *	
     *    @param [in] busName     The new name of the bus interface.
     */
    void busNameChanged(std::string const& busName);

private:
    // Disable copying.
    BusInterfaceWizardGeneralOptionsPage(BusInterfaceWizardGeneralOptionsPage const& rhs);
    BusInterfaceWizardGeneralOptionsPage& operator=(BusInterfaceWizardGeneralOptionsPage const& rhs);

    /*!
     *  Checks if all the mandatory fields are filled in.
     *
     *    @return True, if mandatory fields are filled, otherwise false.
     */
    bool mandatoryFieldsAreFilledIn() const;

    /*!
     * Sets the page layout.
     */
    void setupLayout();
   
    /*!
     *  Check if a valid reference to an existing abstraction definition is found.
     *
     *    @return True, if a valid abstraction definition is found, false otherwise.
     */
    bool abstractionReferenceIsFound() const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component whose bus interface is being edited.
    QSharedPointer<Component> component_;

    //! The bus interface being edited.
    QSharedPointer<BusInterface> busIf_;

    //! The library handler.
    LibraryInterface* handler_;

    //! Editor for bus interface general options.
    BusIfGeneralTab* generalTab_;

    //! Interface for accessing bus interfaces.
    BusInterfaceInterface* busInterface_;
};

#endif // BUSINTERFACEWIZARDGENERALOPTIONSPAGE_H
