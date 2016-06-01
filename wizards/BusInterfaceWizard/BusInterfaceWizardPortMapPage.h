//-----------------------------------------------------------------------------
// File: BusInterfaceWizardPortMapPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 25.11.2013
//
// Description:
// Bus interface wizard page for defining port maps.
//-----------------------------------------------------------------------------

#ifndef BUSINTERFACEWIZARDPORTMAPPAGE_H
#define BUSINTERFACEWIZARDPORTMAPPAGE_H

#include <editors/ComponentEditor/busInterfaces/BusInterfacePortMapTab.h>

#include <QWizardPage>
#include <QLabel>
#include <QTextEdit>

class BusInterfaceWizard;
class BusInterfaceValidator;
class LibraryInterface;
class ExpressionParser;
class ParameterFinder;
class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! Port map page for the bus interface wizard.
//-----------------------------------------------------------------------------
class BusInterfaceWizardPortMapPage : public QWizardPage
{
    Q_OBJECT
public:


    /*!
     *  The constructor.
     *
     *      @param [in] component           The component whose bus interface is being edited.
     *      @param [in] busIf               The bus interface being edited.
     *      @param [in] lh                  The component library handler.
     *      @param [in] physicalPorts       The physical ports selected for mapping.
     *      @param [in] expressionParser    The used expression parser.
     *      @param [in] formatter           The used expression formatter.
     *      @param [in] parameterFinder     The used parameter finder.
     *      @param [in] validator           The validator for bus interface.
     *      @param [in] parent              The parent wizard.
     */
    BusInterfaceWizardPortMapPage(QSharedPointer<Component> component, QSharedPointer<BusInterface> busIf, 
        LibraryInterface* lh, QStringList physicalPorts, QSharedPointer<ExpressionParser> expressionParser,
        QSharedPointer<ExpressionFormatter> formatter, QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<BusInterfaceValidator> validator, BusInterfaceWizard* parent);

    /*!
     *  The destructor.
     */
    ~BusInterfaceWizardPortMapPage();

    /*!
     *  Returns the ID of the next page.
     */
    virtual int nextId() const;

    /*!
     *  Initializes the page.
     */
    virtual void initializePage();
    
    /*!
     * Checks if all the required fields on the page are valid.
     *
     *      @return True, if all fields are valid, otherwise false.
     */
    virtual bool isComplete() const;

private slots:
    
    //! Handler for error messages.
    virtual void showErrorMessage(QString const& msg);

private:
    // Disable copying.
    BusInterfaceWizardPortMapPage(BusInterfaceWizardPortMapPage const& rhs);
    BusInterfaceWizardPortMapPage& operator=(BusInterfaceWizardPortMapPage const& rhs);

    /*!
    *  Sets the page layout.
    */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The component whose interface is being edited.
    QSharedPointer<Component> component_;

    //! The target bus interface.
    QSharedPointer<BusInterface> busIf_;

    //! The library interface.
    LibraryInterface* handler_;

    //! The widget for editing port maps.
    BusInterfacePortMapTab portMapTab_;

    //! The bus interface validator.
    QSharedPointer<BusInterfaceValidator> validator_;
};

#endif // BUSINTERFACEWIZARDPORTMAPPAGE_H
