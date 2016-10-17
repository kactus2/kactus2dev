//-----------------------------------------------------------------------------
// File: tst_DesignValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.01.2016
//
// Description:
// Unit test for class DesignValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Assertion.h>
#include <IPXACTmodels/common/PartSelect.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>
#include <IPXACTmodels/Component/Port.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>
#include <IPXACTmodels/Design/Interconnection.h>
#include <IPXACTmodels/Design/MonitorInterconnection.h>
#include <IPXACTmodels/Design/AdHocConnection.h>
#include <IPXACTmodels/Design/PortReference.h>

#include <IPXACTmodels/Design/validator/DesignValidator.h>
#include <IPXACTmodels/Design/validator/ComponentInstanceValidator.h>
#include <IPXACTmodels/Design/validator/InterconnectionValidator.h>
#include <IPXACTmodels/Design/validator/AdHocConnectionValidator.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <tests/MockObjects/LibraryMock.h>

#include <QtTest>

class tst_DesignValidator : public QObject
{
    Q_OBJECT

public:
    tst_DesignValidator();

private slots:

    void testHasValidVLNV();
    void testHasValidVLNV_data();

    void testComponentInstanceHasValidName();
    void testComponentInstanceHasValidName_data();

    void testComponentInstanceHasValidIsPresent();
    void testComponentInstanceHasValidIsPresent_data();

    void testComponentInstanceHasValidComponentReference();
    void testComponentInstanceHasValidComponentReference_data();

    void testHasValidComponentInstances();
    void testHasValidComponentInstances_data();

    void testInterconnectionHasValidName();
    void testInterconnectionHasValidName_data();

    void testInterconnectionHasValidIsPresent();
    void testInterconnectionHasValidIsPresent_data();

    void testInterconnectionActiveInterfaceIsValid();
    void testInterconnectionActiveInterfaceIsValid_data();

    void testInterconnectionActiveInterfaceHasValidIsPresent();
    void testInterconnectionActiveInterfaceHasValidIsPresent_data();

    void testInterconnectionActiveInterfaceHasValidExcludePorts();
    void testInterconnectionActiveInterfaceHasValidExcludePorts_data();

    void testInterconnectionInterfacesAreValid();
    void testInterconnectionInterfacesAreValid_data();

    void testHasValidInterconnections();
    void testHasValidInterconnections_data();

    void testMonitorInterconnectionHasValidName();
    void testMonitorInterconnectionHasValidName_data();

    void testMonitorInterconnectionHasValidIsPresent();
    void testMonitorInterconnectionHasValidIsPresent_data();

    void testMonitorInterconnectionHasValidMonitoredActiveInterface();
    void testMonitorInterconnectionHasValidMonitoredActiveInterface_data();

    void testMonitorInterconnectionHasValidMonitorInterfaces();
    void testMonitorInterconnectionHasValidMonitorInterfaces_data();

    void testHasValidMonitorInterconnections();
    void testHasValidMonitorInterconnections_data();

    void testAdHocConnectionHasValidName();
    void testAdHocConnectionHasValidName_data();

    void testAdHocConnectionHasValidIsPresent();
    void testAdHocConnectionHasValidIsPresent_data();

    void testAdHocConnectionHasValidTiedValue();
    void testAdHocConnectionHasValidTiedValue_data();

    void testAdHocConnectionHasValidInternalPortReference();
    void testAdHocConnectionHasValidInternalPortReference_data();

    void testAdHocConnectionHasValidExternalPortReference();
    void testAdHocConnectionHasValidExternalPortReference_data();

    void testAdHocConnectionPortReferenceHasValidPartSelect();
    void testAdHocConnectionPortReferenceHasValidPartSelect_data();

    void testHasValidAdHocConnections();
    void testHasValidAdHocConnections_data();

    void testHasValidParameters();
    void testHasValidParameters_data();

    void testHasValidAssertions();
    void testHasValidAssertions_data();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);

    QSharedPointer<DesignValidator> createDesignValidator(LibraryInterface* library);

    QSharedPointer<ComponentInstanceValidator> createComponentInstanceValidator(LibraryInterface* library);

    QSharedPointer<InterconnectionValidator> createInterconnectionValidator(LibraryInterface* library);

    QSharedPointer<AdHocConnectionValidator> createAdHocConnectionValidator(LibraryInterface* library);
};

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::tst_DesignValidator()
//-----------------------------------------------------------------------------
tst_DesignValidator::tst_DesignValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidVLNV()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidVLNV()
{
    QFETCH(QString, vendor);
    QFETCH(QString, library);
    QFETCH(QString, name);
    QFETCH(QString, version);
    QFETCH(bool, isValid);

    VLNV designVLNV(VLNV::DESIGN, vendor, library, name, version);
    QSharedPointer<Design> testDesign (new Design(designVLNV));

    QSharedPointer<DesignValidator> validator = createDesignValidator(0);

    QCOMPARE(validator->hasValidVLNV(testDesign), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testDesign);

        QString expectedError = QObject::tr("The type of the vlnv is invalid within design");

        if (vendor.isEmpty())
        {
            expectedError = QObject::tr("No vendor specified for vlnv within design");
        }
        else if (library.isEmpty())
        {
            expectedError = QObject::tr("No library specified for vlnv within design");
        }
        else if (name.isEmpty())
        {
            expectedError = QObject::tr("No name specified for vlnv within design");
        }
        else if (version.isEmpty())
        {
            expectedError = QObject::tr("No version specified for vlnv within design");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidVLNV_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidVLNV_data()
{
    QTest::addColumn<QString>("vendor");
    QTest::addColumn<QString>("library");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("version");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design with all VLNVs is valid") << "Tengen" << "Toppa" << "Gurren" << "Lagann" << true;
    QTest::newRow("Design without vendor is not valid") << "" << "Toppa" << "Gurren" << "Lagann" << false;
    QTest::newRow("Design without library is not valid") << "Tengen" << "" << "Gurren" << "Lagann" << false;
    QTest::newRow("Design without name is not valid") << "Tengen" << "Toppa" << "" << "Lagann" << false;
    QTest::newRow("Design without version is not valid") << "Tengen" << "Toppa" << "Gurren" << "" << false;
    QTest::newRow("Empty VLNV is not valid") << "" << "" << "" << "" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testComponentInstanceHasValidName()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testComponentInstanceHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<ComponentInstance> testInstance (new ComponentInstance());
    testInstance->setInstanceName(name);

    QSharedPointer<ComponentInstanceValidator> validator = createComponentInstanceValidator(0);

    QCOMPARE(validator->hasValidName(testInstance), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testInstance, "test");

        QString expectedError = QObject::tr("Invalid instance name '%1' set for component instance within %2")
            .arg(name).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testComponentInstanceHasValidName_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testComponentInstanceHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testComponentInstanceHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testComponentInstanceHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<ComponentInstance> testInstance (new ComponentInstance());
    testInstance->setInstanceName("Yatagarasu");
    testInstance->setIsPresent(isPresent);

    QSharedPointer<ComponentInstanceValidator> validator = createComponentInstanceValidator(0);

    QCOMPARE(validator->hasValidIsPresent(testInstance), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testInstance, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for component instance %1 within %2")
            .arg(testInstance->getInstanceName()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testComponentInstanceHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testComponentInstanceHasValidIsPresent_data()
{
    QTest::addColumn<QString>("isPresent");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("IsPresent 1 is valid") << "1" << true;
    QTest::newRow("IsPresent 1*3-3 is valid") << "1*3-3" << true;
    QTest::newRow("IsPresent 2*100 is invalid") << "2*100" << false;
    QTest::newRow("IsPresent -14 is invalid") << "-14" << false;
    QTest::newRow("Real number isPresent  0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as isPresent is invalid") << "test" << false;
    QTest::newRow("String as isPresent is invalid") << "\"test\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testComponentInstanceHasValidComponentReference()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testComponentInstanceHasValidComponentReference()
{
    QFETCH(bool, referenceExists);
    QFETCH(QString, vendor);
    QFETCH(QString, library);
    QFETCH(QString, name);
    QFETCH(QString, version);
    QFETCH(bool, addToLibrary);
    QFETCH(bool, isValid);

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<ComponentInstance> testInstance (new ComponentInstance());
    testInstance->setInstanceName("Saitama");

    if (referenceExists)
    {
        QSharedPointer<ConfigurableVLNVReference> componentVLNV (new ConfigurableVLNVReference(
            VLNV(VLNV::COMPONENT, vendor, library, name, version)));

        testInstance->setComponentRef(componentVLNV);

        if (addToLibrary)
        {
            QSharedPointer<Component> referencedComponent (new Component(*componentVLNV.data()));
            mockLibrary->addComponent(referencedComponent);
        }
    }

    QSharedPointer<ComponentInstanceValidator> validator = createComponentInstanceValidator(mockLibrary);

    QCOMPARE(validator->hasValidComponentReference(testInstance), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testInstance, "test");

        QString expectedError = QObject::tr("The type of the vlnv is invalid within component reference in "
            "component instance %1").arg(testInstance->getInstanceName());

        if (!referenceExists)
        {
            expectedError = QObject::tr("No component reference given in component instance %1 within %2")
                .arg(testInstance->getInstanceName()).arg("test");
        }
        else if (vendor.isEmpty())
        {
            expectedError = QObject::tr("No vendor specified for vlnv within component reference in component "
                "instance %1").arg(testInstance->getInstanceName());
        }
        else if (library.isEmpty())
        {
            expectedError = QObject::tr("No library specified for vlnv within component reference in component "
                "instance %1").arg(testInstance->getInstanceName());
        }
        else if (name.isEmpty())
        {
            expectedError = QObject::tr("No name specified for vlnv within component reference in component "
                "instance %1").arg(testInstance->getInstanceName());
        }
        else if (version.isEmpty())
        {
            expectedError = QObject::tr("No version specified for vlnv within component reference in component "
                "instance %1").arg(testInstance->getInstanceName());
        }
        else if (!addToLibrary)
        {
            expectedError = QObject::tr("Component reference %1 in component instance %2 within %3 was not found "
                "in the library")
                .arg(testInstance->getComponentRef()->toString()).arg(testInstance->getInstanceName()).arg("test");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testComponentInstanceHasValidComponentReference_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testComponentInstanceHasValidComponentReference_data()
{
    QTest::addColumn<bool>("referenceExists");
    QTest::addColumn<QString>("vendor");
    QTest::addColumn<QString>("library");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("version");
    QTest::addColumn<bool>("addToLibrary");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Component instance with VLNV and referencing a component contained in library is valid") <<
        true << "One" << "Punch" << "Man" << "2" << true << true;

    QTest::newRow("Component reference without vendor is not valid") <<
        true << "" << "Punch" << "Man" << "2" << true << false;
    QTest::newRow("Component reference without library is not valid") <<
        true << "One" << "" << "Man" << "2" << true << false;
    QTest::newRow("Component reference without name is not valid") <<
        true << "One" << "Punch" << "" << "2" << true << false;
    QTest::newRow("Component reference without version is not valid") <<
        true << "One" << "Punch" << "Man" << "" << true << false;

    QTest::newRow("Component instance without a component reference is not valid") <<
        false << "" << "" << "" << "" << false << false;
    QTest::newRow("Component reference referencing design not found in library is not valid") <<
        true << "One" << "Punch" << "Man" << "2" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidComponentInstances()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidComponentInstances()
{
    QFETCH(bool, instanceExists);
    QFETCH(QString, name);
    QFETCH(bool, componentReferenceExists);
    QFETCH(bool, copyInstance);
    QFETCH(bool, isValid);

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<Design> testDesign (new Design(VLNV(VLNV::DESIGN, "Samurai", "Champloo", "MugenJinFuu", "3")));

    QSharedPointer<ConfigurableVLNVReference> componentVLNV (
        new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Man", "Saitama")));

    QSharedPointer<ComponentInstance> testInstance (new ComponentInstance());
    testInstance->setInstanceName(name);

    if (instanceExists)
    {
        testDesign->getComponentInstances()->append(testInstance);

        if (componentReferenceExists)
        {
            testInstance->setComponentRef(componentVLNV);
            QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));
            mockLibrary->addComponent(testComponent);
        }

        if (copyInstance)
        {
            QSharedPointer<ComponentInstance> otherInstance (new ComponentInstance(*testInstance.data()));
            testDesign->getComponentInstances()->append(otherInstance);
        }
    }

    QSharedPointer<DesignValidator> validator = createDesignValidator(mockLibrary);

    QCOMPARE(validator->hasValidComponentInstances(testDesign), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testDesign);

        QString expectedError = QObject::tr("Component instance name '%1' within design %2 is not unique.")
            .arg(testInstance->getInstanceName()).arg(testDesign->getVlnv().toString());

        if (name.isEmpty())
        {
            expectedError = QObject::tr("Invalid instance name '%1' set for component instance within design %2")
                .arg(testInstance->getInstanceName()).arg(testDesign->getVlnv().toString());
        }
        else if (!componentReferenceExists)
        {
            expectedError = QObject::tr("No component reference given in component instance %1 within design %2")
                .arg(testInstance->getInstanceName()).arg(testDesign->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidComponentInstances_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidComponentInstances_data()
{
    QTest::addColumn<bool>("instanceExists");
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("componentReferenceExists");
    QTest::addColumn<bool>("copyInstance");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design without component instances is valid") << false << "" << false << false << true;
    QTest::newRow("Component instance with name and component reference is valid") <<
        true << "Genos" << true << false << true;

    QTest::newRow("Component instance without name is not valid") <<
        true << "" << true << false << false; 
    QTest::newRow("Component instance without component reference is not valid") <<
        true << "Genos" << false << false << false;
    QTest::newRow("Component instances with non-unique name is not valid") <<
        true << "Genos" << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionHasValidName()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<Interconnection> testInterconnection (new Interconnection());
    testInterconnection->setName(name);

    QSharedPointer<InterconnectionValidator> validator = createInterconnectionValidator(0);

    QCOMPARE(validator->hasValidName(testInterconnection->name()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsInInterconnection(foundErrors, testInterconnection, "test");

        QString expectedError = QObject::tr("Invalid name '%1' set for interconnection within %2")
            .arg(name).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionHasValidName_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<Interconnection> testConnection (new Interconnection());
    testConnection->setName("Yatagarasu");
    testConnection->setIsPresent(isPresent);

    QSharedPointer<InterconnectionValidator> validator = createInterconnectionValidator(0);

    QCOMPARE(validator->hasValidIsPresent(testConnection->getIsPresent()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsInInterconnection(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for interconnection '%1' within %2")
            .arg(testConnection->name()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionHasValidIsPresent_data()
{
    QTest::addColumn<QString>("isPresent");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("IsPresent 1 is valid") << "1" << true;
    QTest::newRow("IsPresent 1*3-3 is valid") << "1*3-3" << true;
    QTest::newRow("IsPresent 2*100 is invalid") << "2*100" << false;
    QTest::newRow("IsPresent -14 is invalid") << "-14" << false;
    QTest::newRow("Real number isPresent  0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as isPresent is invalid") << "test" << false;
    QTest::newRow("String as isPresent is invalid") << "\"test\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionActiveInterfaceIsValid()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionActiveInterfaceIsValid()
{
    QFETCH(bool, startInterfaceExists);
    QFETCH(QString, componentReference);
    QFETCH(bool, componentExists);
    QFETCH(QString, busReference);
    QFETCH(bool, busInterfaceExists);
    QFETCH(bool, isValid);

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > containedInstances (
        new QList<QSharedPointer<ComponentInstance> > ());

    QSharedPointer<Interconnection> testConnection (new Interconnection());
    testConnection->setName("Balduran");

    if (startInterfaceExists)
    {
        QSharedPointer<ActiveInterface> testInterface (new ActiveInterface(componentReference, busReference));
        testConnection->setStartInterface(testInterface);

        if (componentExists)
        {
            QSharedPointer<ConfigurableVLNVReference> componentVLNV (
                new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Man", "Saitama")));

            QSharedPointer<ComponentInstance> componentInstance (
                new ComponentInstance(componentReference, componentVLNV));
            if (componentReference.isEmpty())
            {
                componentInstance->setInstanceName("Baiken");
            }
            containedInstances->append(componentInstance);

            QSharedPointer<Component> referencedComponent (new Component(*componentVLNV.data()));
            mockLibrary->addComponent(referencedComponent);

            if (busInterfaceExists)
            {
                QSharedPointer<BusInterface> testBus (new BusInterface());
                testBus->setName(busReference);
                referencedComponent->getBusInterfaces()->append(testBus);
            }
        }
    }

    QSharedPointer<InterconnectionValidator> validator = createInterconnectionValidator(mockLibrary);
    validator->changeComponentInstances(containedInstances);

    QCOMPARE(validator->hasValidStartInterface(testConnection), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsInInterconnection(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("No component reference set for active interface in interconnection "
            "'%1' within %2").arg(testConnection->name()).arg("test");

        if (!startInterfaceExists)
        {
            expectedError = QObject::tr("No active interface set for interconnection '%1' within %2")
                .arg(testConnection->name()).arg("test");
        }
        else if (!componentExists)
        {
            expectedError = QObject::tr("Component instance '%1' referenced by the active interface in "
                "interconnection '%2' within %3 was not found")
                .arg(componentReference).arg(testConnection->name()).arg("test");
        }
        else if (busReference.isEmpty())
        {
            expectedError = QObject::tr("No bus reference set for active interface in interconnection '%1' within "
                "%2").arg(testConnection->name()).arg("test");
        }
        else if (!busInterfaceExists)
        {
            expectedError = QObject::tr("Bus interface '%1' referenced by the active interface in interconnection "
                "'%2' within %3 was not found")
                .arg(busReference).arg(testConnection->name()).arg("test");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionActiveInterfaceIsValid_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionActiveInterfaceIsValid_data()
{
    QTest::addColumn<bool>("startInterfaceExists");
    QTest::addColumn<QString>("componentReference");
    QTest::addColumn<bool>("componentExists");
    QTest::addColumn<QString>("busReference");
    QTest::addColumn<bool>("busInterfaceExists");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Active interface with component reference and bus reference is valid") <<
        true << "Saitama" << true << "Genos" << true << true;

    QTest::newRow("Active interface without component reference is not valid") <<
        true << "" << true << "Genos" << true << false;
    QTest::newRow("Active interface with component reference to non-existing component is not valid") <<
        true << "Saitama" << false << "Genos" << true << false;
    QTest::newRow("Active interface without bus reference is not valid") <<
        true << "Saitama" << true << "" << true << false;
    QTest::newRow("Active interface with bus reference to non-existing bus interface is not valid") <<
        true << "Saitama" << true << "Genos" << false << false;
    QTest::newRow("Interconnection without active interface is not valid") <<
        false << "" << false << "" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionActiveInterfaceHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionActiveInterfaceHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<ActiveInterface> testInterface (new ActiveInterface("Saitama", "Genos"));
    testInterface->setIsPresent(isPresent);

    QSharedPointer<Interconnection> testConnection (new Interconnection("OnePunchMan", testInterface));

    QSharedPointer<InterconnectionValidator> validator = createInterconnectionValidator(0);

    QCOMPARE(validator->hasValidIsPresent(testInterface->getIsPresent()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsInInterconnection(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for %1 within interconnection '%2'")
            .arg("active interface").arg(testConnection->name());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionActiveInterfaceHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionActiveInterfaceHasValidIsPresent_data()
{
    QTest::addColumn<QString>("isPresent");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("IsPresent 1 is valid") << "1" << true;
    QTest::newRow("IsPresent 1*3-3 is valid") << "1*3-3" << true;
    QTest::newRow("IsPresent 2*100 is invalid") << "2*100" << false;
    QTest::newRow("IsPresent -14 is invalid") << "-14" << false;
    QTest::newRow("Real number isPresent  0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as isPresent is invalid") << "test" << false;
    QTest::newRow("String as isPresent is invalid") << "\"test\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionActiveInterfaceHasValidExcludePorts()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionActiveInterfaceHasValidExcludePorts()
{
    QFETCH(QString, portName);
    QFETCH(bool, portExists);
    QFETCH(bool, isValid);

    QSharedPointer<ActiveInterface> testInterface (new ActiveInterface("Genos", "Sonic"));

    QSharedPointer<BusInterface> testBus (new BusInterface());
    testBus->setName(testInterface->getBusReference());

    QSharedPointer<ConfigurableVLNVReference> componentVLNV (
        new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Man", "Saitama")));
    QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));

    testComponent->getBusInterfaces()->append(testBus);

    QSharedPointer<ComponentInstance> testInstance (
        new ComponentInstance(testInterface->getComponentReference(), componentVLNV));

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > instances (
        new QList<QSharedPointer<ComponentInstance> > ());
    instances->append(testInstance);

    LibraryMock mockLibrary (this);
    mockLibrary.addComponent(testComponent);

    if (!portName.isEmpty())
    {
        testInterface->getExcludePorts()->append(portName);

        if (portExists)
        {
            QSharedPointer<PortMap::LogicalPort> testPort (new PortMap::LogicalPort());
            testPort->name_ = portName;

            QSharedPointer<PortMap> testMap (new PortMap());
            testMap->setLogicalPort(testPort);

            QSharedPointer<AbstractionType> testAbstraction (new AbstractionType());
            testAbstraction->getPortMaps()->append(testMap);

            testBus->getAbstractionTypes()->append(testAbstraction);
        }
    }

    QSharedPointer<Interconnection> testConnection (new Interconnection("OnePunchMan", testInterface));

    QSharedPointer<InterconnectionValidator> validator = createInterconnectionValidator(&mockLibrary);
    validator->changeComponentInstances(instances);

    QCOMPARE(validator->hasValidStartInterface(testConnection), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsInInterconnection(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("Logical port referenced in active interface in interconnection '%1' "
            "was not found in the port maps of the referenced bus interface %2")
            .arg(testConnection->name()).arg(testBus->name());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionActiveInterfaceHasValidExcludePorts_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionActiveInterfaceHasValidExcludePorts_data()
{
    QTest::addColumn<QString>("portName");
    QTest::addColumn<bool>("portExists");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Active interface without an exclude port is valid") << "" << false << true;
    QTest::newRow("Active interface exclude port referencing existing logical port is valid") <<
        "One" << true << true;
    QTest::newRow("Active interface exclude port referencing non-existing logical port is not valid") <<
        "One" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionInterfacesAreValid()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionInterfacesAreValid()
{
    QFETCH(bool, createActiveInterface);
    QFETCH(bool, createHierInterface);
    QFETCH(bool, copyInterface);
    QFETCH(bool, isValid);

    QSharedPointer<Interconnection> testConnection (new Interconnection());
    testConnection->setName("Gygax");

    LibraryMock mockLibrary (this);

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > instances (
        new QList<QSharedPointer<ComponentInstance> > ());

    QSharedPointer<ActiveInterface> startInterface(new ActiveInterface("StartComponent", "StartBus"));

    QSharedPointer<ConfigurableVLNVReference> startVLNV (
        new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Start", "1.0")));
    QSharedPointer<Component> startComponent (new Component(*startVLNV));
    mockLibrary.addComponent(startComponent);

    QSharedPointer<ComponentInstance> startInstance (
        new ComponentInstance(startInterface->getComponentReference(), startVLNV));

    QSharedPointer<BusInterface> startBus(new BusInterface());
    startBus->setName(startInterface->getBusReference());
    startComponent->getBusInterfaces()->append(startBus);

    testConnection->setStartInterface(startInterface);

    QSharedPointer<ActiveInterface> testInterface (new ActiveInterface("Demi", "Lich"));

    if (createActiveInterface)
    {
        QSharedPointer<ConfigurableVLNVReference> componentVLNV (
            new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Man", "Saitama")));
        QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));

        QSharedPointer<ComponentInstance> testInstance (
            new ComponentInstance(testInterface->getComponentReference(), componentVLNV));
        instances->append(testInstance);

        QSharedPointer<BusInterface> testBus(new BusInterface());
        testBus->setName(testInterface->getBusReference());

        testComponent->getBusInterfaces()->append(testBus);

        testConnection->getActiveInterfaces()->append(testInterface);

        mockLibrary.addComponent(testComponent);

        if (copyInterface)
        {
            QSharedPointer<ActiveInterface> otherInterface (new ActiveInterface(*testInterface.data()));
            testConnection->getActiveInterfaces()->append(otherInterface);
        }
    }

    QSharedPointer<HierInterface> hierInterface (new HierInterface("Wight"));
    if (createHierInterface)
    {
        testConnection->getHierInterfaces()->append(hierInterface);

        if (copyInterface)
        {
            QSharedPointer<HierInterface> otherInterface (new HierInterface(*hierInterface.data()));
            testConnection->getHierInterfaces()->append(otherInterface);
        }
    }

    QSharedPointer<InterconnectionValidator> validator = createInterconnectionValidator(&mockLibrary);
    validator->changeComponentInstances(instances);

    QCOMPARE(validator->hasValidInterfaces(testConnection), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsInInterconnection(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("No active interfaces or hierarchical interfaces set for "
            "interconnection '%1' within %2").arg(testConnection->name()).arg("test");

        if (copyInterface && !createHierInterface)
        {
            expectedError = QObject::tr("Component reference '%1' and bus reference '%2' pair is not unique in "
                "active interfaces of interconnection '%3' within %4.")
                .arg(testInterface->getComponentReference()).arg(testInterface->getBusReference())
                .arg(testConnection->name()).arg("test");
        }
        else if (copyInterface && createHierInterface)
        {
            expectedError = QObject::tr("Bus reference '%1' is not unique in hierarchical interfaces of "
                "interconnection '%2' within %3.")
                .arg(hierInterface->getBusReference()).arg(testConnection->name()).arg("test");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testInterconnectionInterfacesAreValid_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testInterconnectionInterfacesAreValid_data()
{
    QTest::addColumn<bool>("createActiveInterface");
    QTest::addColumn<bool>("createHierInterface");
    QTest::addColumn<bool>("copyInterface");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Interconnection with both an active interface and a hier interface is valid") <<
        true << true << false << true;
    QTest::newRow("Interconnection with active interface is valid") << true << false << false << true;
    QTest::newRow("Interconnection with hier interface is valid") << false << true << false << true;

    QTest::newRow("Interconnection without active interface or hier interface is not valid") <<
        false << false << false << false;
    QTest::newRow("Interconnection with interfaces referencing same component/bus interface pair is not valid") <<
        true << false << true << false;
    QTest::newRow("Hierarchical interfaces referencing the same bus interface is not valid") <<
        false << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidInterconnections()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidInterconnections()
{
    QFETCH(bool, createInterconnection);
    QFETCH(bool, startInterfaceExists);
    QFETCH(bool, hierInterfaceExists);
    QFETCH(bool, copyInterconnection);
    QFETCH(bool, isValid);

    QSharedPointer<Design> testDesign (new Design(VLNV(VLNV::DESIGN, "Samurai", "Champloo", "MugenJinFuu", "3")));

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<Interconnection> testConnection (new Interconnection());
    testConnection->setName("testConnection");

    if (createInterconnection)
    {
        if (startInterfaceExists)
        {
            QSharedPointer<ActiveInterface> activeInterface (new ActiveInterface("Space", "Dandy"));

            QSharedPointer<ConfigurableVLNVReference> componentVLNV (
                new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Man", "Saitama")));
            QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));
            mockLibrary->addComponent(testComponent);

            QSharedPointer<ComponentInstance> testInstance (
                new ComponentInstance(activeInterface->getComponentReference(), componentVLNV));
            testDesign->getComponentInstances()->append(testInstance);

            QSharedPointer<BusInterface> testBus(new BusInterface());
            testBus->setName(activeInterface->getBusReference());
            testComponent->getBusInterfaces()->append(testBus);

            testConnection->setStartInterface(activeInterface);
        }

        if (hierInterfaceExists)
        {
            QSharedPointer<HierInterface> hierarchical (new HierInterface("Bebop"));
            testConnection->getHierInterfaces()->append(hierarchical);
        }

        testDesign->getInterconnections()->append(testConnection);

        if (copyInterconnection)
        {
            QSharedPointer<Interconnection> otherConnection (new Interconnection(*testConnection.data()));
            testDesign->getInterconnections()->append(otherConnection);
        }
    }

    QSharedPointer<DesignValidator> validator = createDesignValidator(mockLibrary);

    QCOMPARE(validator->hasValidInterconnections(testDesign), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testDesign);

        QString expectedError = QObject::tr("Interconnection name %1 within design %2 is not unique")
            .arg(testConnection->name()).arg(testDesign->getVlnv().toString());

        if (!startInterfaceExists)
        {
            expectedError = QObject::tr("No active interface set for interconnection '%1' within design %2")
                .arg(testConnection->name()).arg(testDesign->getVlnv().toString());
        }
        else if (!hierInterfaceExists)
        {
            expectedError = QObject::tr("No active interfaces or hierarchical interfaces set for interconnection "
                "'%1' within design %2" )
                .arg(testConnection->name()).arg(testDesign->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidInterconnections_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidInterconnections_data()
{
    QTest::addColumn<bool>("createInterconnection");
    QTest::addColumn<bool>("startInterfaceExists");
    QTest::addColumn<bool>("hierInterfaceExists");
    QTest::addColumn<bool>("copyInterconnection");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design without interconnections is valid") << false << false << false << false << true;
    QTest::newRow("Interconnection with start interface and a hierarchical interface is valid") <<
        true << true << true << false << true;

    QTest::newRow("Interconnection without a start interface is not valid") <<
        true << false << true << false << false;
    QTest::newRow("Interconnection without a second interface is not valid") <<
        true << true << false << false << false;
    QTest::newRow("Interconnections with non-unique names are not valid") << true << true << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testMonitorInterconnectionHasValidName()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testMonitorInterconnectionHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<MonitorInterconnection> testConnection (new MonitorInterconnection());
    testConnection->setName(name);

    QSharedPointer<InterconnectionValidator> validator = createInterconnectionValidator(0);

    QCOMPARE(validator->hasValidName(testConnection->name()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsInMonitorInterconnection(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("Invalid name '%1' set for monitor interconnection within %2")
            .arg(name).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testMonitorInterconnectionHasValidName_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testMonitorInterconnectionHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testMonitorInterconnectionHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testMonitorInterconnectionHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<MonitorInterconnection> testConnection (new MonitorInterconnection());
    testConnection->setName("Yatagarasu");
    testConnection->setIsPresent(isPresent);

    QSharedPointer<InterconnectionValidator> validator = createInterconnectionValidator(0);

    QCOMPARE(validator->hasValidIsPresent(testConnection->getIsPresent()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsInMonitorInterconnection(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for monitor interconnection '%1' within %2")
            .arg(testConnection->name()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testMonitorInterconnectionHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testMonitorInterconnectionHasValidIsPresent_data()
{
    QTest::addColumn<QString>("isPresent");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("IsPresent 1 is valid") << "1" << true;
    QTest::newRow("IsPresent 1*3-3 is valid") << "1*3-3" << true;
    QTest::newRow("IsPresent 2*100 is invalid") << "2*100" << false;
    QTest::newRow("IsPresent -14 is invalid") << "-14" << false;
    QTest::newRow("Real number isPresent  0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as isPresent is invalid") << "test" << false;
    QTest::newRow("String as isPresent is invalid") << "\"test\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testMonitorInterconnectionHasValidMonitoredActiveInterface()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testMonitorInterconnectionHasValidMonitoredActiveInterface()
{
    QFETCH(bool, startInterfaceExists);
    QFETCH(QString, componentReference);
    QFETCH(bool, instanceExists);
    QFETCH(QString, busReference);
    QFETCH(bool, busInterfaceExists);
    QFETCH(bool, isValid);

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > containedInstances (
        new QList<QSharedPointer<ComponentInstance> > ());

    QSharedPointer<MonitorInterconnection> testConnection (new MonitorInterconnection());
    testConnection->setName("Balduran");

    if (startInterfaceExists)
    {
        QSharedPointer<MonitorInterface> testInterface (new MonitorInterface(componentReference, busReference));
        testConnection->setMonitoredctiveInterface(testInterface);

        if (instanceExists)
        {
            QSharedPointer<ConfigurableVLNVReference> componentVLNV (
                new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Man", "Saitama")));

            QSharedPointer<ComponentInstance> componentInstance (
                new ComponentInstance(componentReference, componentVLNV));
            if (componentReference.isEmpty())
            {
                componentInstance->setInstanceName("Baiken");
            }
            containedInstances->append(componentInstance);

            QSharedPointer<Component> referencedComponent (new Component(*componentVLNV.data()));
            mockLibrary->addComponent(referencedComponent);

            if (busInterfaceExists)
            {
                QSharedPointer<BusInterface> testBus (new BusInterface());
                testBus->setName(busReference);
                if (busReference.isEmpty())
                {
                    testBus->setName("TKL");
                }
                referencedComponent->getBusInterfaces()->append(testBus);
            }
        }
    }

    QSharedPointer<InterconnectionValidator> validator = createInterconnectionValidator(mockLibrary);
    validator->changeComponentInstances(containedInstances);

    QCOMPARE(validator->hasValidMonitoredActiveInterface(testConnection), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsInMonitorInterconnection(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("No component reference set for monitored active interface in monitor "
            "interconnection '%1' within %2").arg(testConnection->name()).arg("test");

        if (!startInterfaceExists)
        {
            expectedError = QObject::tr("No monitored active interface set for monitor interconnection '%1' "
                "within %2").arg(testConnection->name()).arg("test");
        }
        else if (!instanceExists)
        {
            expectedError = QObject::tr("Component instance '%1' referenced by the monitored active interface in "
                "monitor interconnection '%2' within %3 was not found")
                .arg(componentReference).arg(testConnection->name()).arg("test");
        }
        else if (busReference.isEmpty())
        {
            expectedError = QObject::tr("No bus reference set for monitored active interface in monitor "
                "interconnection '%1' within %2").arg(testConnection->name()).arg("test");
        }
        else if (!busInterfaceExists)
        {
            expectedError = QObject::tr("Bus interface '%1' referenced by the monitored active interface in "
                "monitor interconnection '%2' within %3 was not found")
                .arg(busReference).arg(testConnection->name()).arg("test");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testMonitorInterconnectionHasValidMonitoredActiveInterface_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testMonitorInterconnectionHasValidMonitoredActiveInterface_data()
{
    QTest::addColumn<bool>("startInterfaceExists");
    QTest::addColumn<QString>("componentReference");
    QTest::addColumn<bool>("instanceExists");
    QTest::addColumn<QString>("busReference");
    QTest::addColumn<bool>("busInterfaceExists");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Monitored active interface with component and bus references to existing elements is valid") <<
        true << "Space" << true << "Dandy" << true << true;

    QTest::newRow("Monitor interconnection without monitored active interface is not valid") <<
        false << "" << false << "" << false << false;
    QTest::newRow("Monitored active interface without component instance reference is not valid") <<
        true << "" << true << "Dandy" << true << false;
    QTest::newRow("Monitored active interface referencing non-existing component instance is not valid") <<
        true << "Space" << false << "Dandy" << true << false;
    QTest::newRow("Monitored active interface without bus reference is not valid") <<
        true << "Space" << true << "" << true << false;
    QTest::newRow("Monitored active interface referencing non-existing bus interface is not valid") <<
        true << "Space" << true << "Dandy" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testMonitorInterconnectionHasValidMonitorInterfaces()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testMonitorInterconnectionHasValidMonitorInterfaces()
{
    QFETCH(bool, monitorInterfaceExists);
    QFETCH(QString, componentReference);
    QFETCH(bool, instanceExists);
    QFETCH(QString, busReference);
    QFETCH(bool, busInterfaceExists);
    QFETCH(bool, copyInterface);
    QFETCH(bool, isValid);

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > containedInstances (
        new QList<QSharedPointer<ComponentInstance> > ());

    QSharedPointer<MonitorInterconnection> testConnection (new MonitorInterconnection());
    testConnection->setName("Balduran");

    QSharedPointer<MonitorInterface> testInterface (new MonitorInterface(componentReference, busReference));
    if (monitorInterfaceExists)
    {
        testConnection->getMonitorInterfaces()->append(testInterface);

        if (instanceExists)
        {
            QSharedPointer<ConfigurableVLNVReference> componentVLNV (
                new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Man", "Saitama")));

            QSharedPointer<ComponentInstance> componentInstance (
                new ComponentInstance(componentReference, componentVLNV));
            if (componentReference.isEmpty())
            {
                componentInstance->setInstanceName("Baiken");
            }
            containedInstances->append(componentInstance);

            QSharedPointer<Component> referencedComponent (new Component(*componentVLNV.data()));
            mockLibrary->addComponent(referencedComponent);

            if (busInterfaceExists)
            {
                QSharedPointer<BusInterface> testBus (new BusInterface());
                testBus->setName(busReference);
                if (busReference.isEmpty())
                {
                    testBus->setName("TKL");
                }
                referencedComponent->getBusInterfaces()->append(testBus);
            }
        }

        if (copyInterface)
        {
            QSharedPointer<MonitorInterface> otherInterface (new MonitorInterface(*testInterface.data()));
            testConnection->getMonitorInterfaces()->append(otherInterface);
        }
    }

    QSharedPointer<InterconnectionValidator> validator = createInterconnectionValidator(mockLibrary);
    validator->changeComponentInstances(containedInstances);

    QCOMPARE(validator->hasValidMonitorInterfaces(testConnection), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsInMonitorInterconnection(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("No component reference set for monitor interface in monitor "
            "interconnection '%1' within %2").arg(testConnection->name()).arg("test");

        if (!monitorInterfaceExists)
        {
            expectedError = QObject::tr("No monitor interfaces set for monitor interconnection '%1' within %2")
                .arg(testConnection->name()).arg("test");
        }
        else if (!instanceExists)
        {
            expectedError = QObject::tr("Component instance '%1' referenced by the monitor interface in monitor "
                "interconnection '%2' within %3 was not found")
                .arg(componentReference).arg(testConnection->name()).arg("test");
        }
        else if (busReference.isEmpty())
        {
            expectedError = QObject::tr("No bus reference set for monitor interface in monitor interconnection "
                "'%1' within %2").arg(testConnection->name()).arg("test");
        }
        else if (!busInterfaceExists)
        {
            expectedError = QObject::tr("Bus interface '%1' referenced by the monitor interface in monitor "
                "interconnection '%2' within %3 was not found")
                .arg(busReference).arg(testConnection->name()).arg("test");
        }
        else if (copyInterface)
        {
            expectedError = QObject::tr("Component reference '%1' and bus reference '%2' pair is not unique in "
                "monitor interfaces of monitor interconnection '%3' within %4.")
                .arg(testInterface->getComponentReference()).arg(testInterface->getBusReference())
                .arg(testConnection->name()).arg("test");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testMonitorInterconnectionHasValidMonitorInterfaces_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testMonitorInterconnectionHasValidMonitorInterfaces_data()
{
    QTest::addColumn<bool>("monitorInterfaceExists");
    QTest::addColumn<QString>("componentReference");
    QTest::addColumn<bool>("instanceExists");
    QTest::addColumn<QString>("busReference");
    QTest::addColumn<bool>("busInterfaceExists");
    QTest::addColumn<bool>("copyInterface");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Monitor interface with component and bus references to existing elements is valid") <<
        true << "Space" << true << "Dandy" << true << false << true;

    QTest::newRow("Monitor interconnection without monitor interface is not valid") <<
        false << "" << false << "" << false << false << false;
    QTest::newRow("Monitor interface without component instance reference is not valid") <<
        true << "" << true << "Dandy" << true << false << false;
    QTest::newRow("Monitor interface referencing non-existing component instance is not valid") <<
        true << "Space" << false << "Dandy" << true << false << false;
    QTest::newRow("Monitor interface without bus reference is not valid") <<
        true << "Space" << true << "" << true << false << false;
    QTest::newRow("Monitor interface referencing non-existing bus interface is not valid") <<
        true << "Space" << true << "Dandy" << false << false << false;

    QTest::newRow("Monitor interface referencing non-unique component / bus reference pair is not valid") <<
        true << "Space" << true << "Dandy" << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidMonitorInterconnections()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidMonitorInterconnections()
{
    QFETCH(bool, createInterconnection);
    QFETCH(bool, monitoredActiveInterfaceExists);
    QFETCH(bool, monitorInterfaceExists);
    QFETCH(bool, copyInterconnection);
    QFETCH(bool, isValid);

    QSharedPointer<Design> testDesign (new Design(VLNV(VLNV::DESIGN, "Samurai", "Champloo", "MugenJinFuu", "3")));

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<MonitorInterconnection> testConnection (new MonitorInterconnection());
    testConnection->setName("testConnection");

    if (createInterconnection)
    {
        if (monitoredActiveInterfaceExists)
        {
            QSharedPointer<MonitorInterface> activeInterface (new MonitorInterface("Space", "Dandy"));

            QSharedPointer<ConfigurableVLNVReference> componentVLNV (
                new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Man", "Saitama")));
            QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));
            mockLibrary->addComponent(testComponent);

            QSharedPointer<ComponentInstance> testInstance (
                new ComponentInstance(activeInterface->getComponentReference(), componentVLNV));
            testDesign->getComponentInstances()->append(testInstance);

            QSharedPointer<BusInterface> testBus(new BusInterface());
            testBus->setName(activeInterface->getBusReference());
            testComponent->getBusInterfaces()->append(testBus);

            testConnection->setMonitoredctiveInterface(activeInterface);
        }

        if (monitorInterfaceExists)
        {
            QSharedPointer<MonitorInterface> monitorInterface (new MonitorInterface("Cowboy", "Bebop"));

            QSharedPointer<ConfigurableVLNVReference> componentVLNV(
                new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "Spike", "Jet", "Faye", "Edward")));
            QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));
            mockLibrary->addComponent(testComponent);

            QSharedPointer<ComponentInstance> testInstance (
                new ComponentInstance(monitorInterface->getComponentReference(), componentVLNV));
            testDesign->getComponentInstances()->append(testInstance);

            QSharedPointer<BusInterface> testBus(new BusInterface());
            testBus->setName(monitorInterface->getBusReference());
            testComponent->getBusInterfaces()->append(testBus);

            testConnection->getMonitorInterfaces()->append(monitorInterface);
        }

        testDesign->getMonitorInterconnecions()->append(testConnection);

        if (copyInterconnection)
        {
            QSharedPointer<MonitorInterconnection> otherConnection (
                new MonitorInterconnection(*testConnection.data()));
            testDesign->getMonitorInterconnecions()->append(otherConnection);
        }
    }

    QSharedPointer<DesignValidator> validator = createDesignValidator(mockLibrary);

    QCOMPARE(validator->hasValidMonitorInterconnections(testDesign), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testDesign);

        QString expectedError = QObject::tr("Monitor interconnection name %1 within design %2 is not unique")
            .arg(testConnection->name()).arg(testDesign->getVlnv().toString());

        if (!monitoredActiveInterfaceExists)
        {
            expectedError = QObject::tr("No monitored active interface set for monitor interconnection '%1' "
                "within design %2").arg(testConnection->name()).arg(testDesign->getVlnv().toString());
        }
        else if (!monitorInterfaceExists)
        {
            expectedError = QObject::tr("No monitor interfaces set for monitor interconnection '%1' within design "
                "%2" ).arg(testConnection->name()).arg(testDesign->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidMonitorInterconnections_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidMonitorInterconnections_data()
{
    QTest::addColumn<bool>("createInterconnection");
    QTest::addColumn<bool>("monitoredActiveInterfaceExists");
    QTest::addColumn<bool>("monitorInterfaceExists");
    QTest::addColumn<bool>("copyInterconnection");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design without monitor interconnections is valid") << false << false << false << false << true;
    QTest::newRow("Monitor interconnection with monitored active interface and a monitor interface is valid") <<
        true << true << true << false << true;

    QTest::newRow("Monitor interconnection without a monitored active interface is not valid") <<
        true << false << true << false << false;
    QTest::newRow("Monitor interconnection without a monitor interface is not valid") <<
        true << true << false << false << false;
    QTest::newRow("Monitor interconnections with non-unique names are not valid") <<
        true << true << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionHasValidName()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<AdHocConnection> testConnection (new AdHocConnection(name));

    QSharedPointer<AdHocConnectionValidator> validator = createAdHocConnectionValidator(0);

    QCOMPARE(validator->hasValidName(testConnection), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("Invalid name '%1' set for ad hoc connection within %2")
            .arg(name).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionHasValidName_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<AdHocConnection> testConnection (new AdHocConnection("Yatagarasu"));
    testConnection->setIsPresent(isPresent);

    QSharedPointer<AdHocConnectionValidator> validator = createAdHocConnectionValidator(0);

    QCOMPARE(validator->hasValidIsPresent(testConnection->getIsPresent()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for ad hoc connection %1 within %2")
            .arg(testConnection->name()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionHasValidIsPresent_data()
{
    QTest::addColumn<QString>("isPresent");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("IsPresent 1 is valid") << "1" << true;
    QTest::newRow("IsPresent 1*3-3 is valid") << "1*3-3" << true;
    QTest::newRow("IsPresent 2*100 is invalid") << "2*100" << false;
    QTest::newRow("IsPresent -14 is invalid") << "-14" << false;
    QTest::newRow("Real number isPresent  0.12 is invalid") << "0.12" << false;
    QTest::newRow("Text as isPresent is invalid") << "test" << false;
    QTest::newRow("String as isPresent is invalid") << "\"test\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionHasValidTiedValue()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionHasValidTiedValue()
{
    QFETCH(QString, tiedValue);
    QFETCH(bool, isValid);

    QSharedPointer<AdHocConnection> testConnection (new AdHocConnection("Yatagarasu"));
    testConnection->setTiedValue(tiedValue);

    QSharedPointer<AdHocConnectionValidator> validator = createAdHocConnectionValidator(0);

    QCOMPARE(validator->hasValidTiedValue(testConnection), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("Invalid tied value set for ad hoc connection %1 within %2")
            .arg(testConnection->name()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionHasValidTiedValue_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionHasValidTiedValue_data()
{
    QTest::addColumn<QString>("tiedValue");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Tied value default is valid") << "default" << true;
    QTest::newRow("Tied value open is valid") << "open" << true;
    QTest::newRow("Numerical tied value is valid") << "1" << true;
    QTest::newRow("Hexadecimal tied value is valid") << "'h01" << true;
    QTest::newRow("Empty tied value is valid") << "" << true;

    QTest::newRow("Tied value of text is not valid") << "text" << false;
    QTest::newRow("Tied value of only white spaces is not valid") << "    " << false;
    QTest::newRow("Tied value consisting of characters and white spaces is not valid") << "  test  " << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionHasValidInternalPortReference()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionHasValidInternalPortReference()
{
    QFETCH(QString, tiedValue);
    QFETCH(QString, componentReference);
    QFETCH(bool, instanceExists);
    QFETCH(QString, portReference);
    QFETCH(bool, portExists);
    QFETCH(bool, portHasDefault);
    QFETCH(bool, isValid);

    QSharedPointer<AdHocConnection> testConnection (new AdHocConnection("Yatagarasu"));
    testConnection->setTiedValue(tiedValue);

    QSharedPointer<PortReference> internalReference (new PortReference(portReference, componentReference));
    testConnection->getInternalPortReferences()->append(internalReference);

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > instances (
        new QList<QSharedPointer<ComponentInstance> > ());

    LibraryMock* mockLibrary (new LibraryMock(this));

    if (instanceExists)
    {
        QSharedPointer<ConfigurableVLNVReference> componentVLNV (
            new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Man", "Saitama")));
        QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));
        mockLibrary->addComponent(testComponent);

        QSharedPointer<ComponentInstance> testInstance(new ComponentInstance(componentReference, componentVLNV));
        if (componentReference.isEmpty())
        {
            testInstance->setInstanceName("testInstance");
        }
        instances->append(testInstance);

        if (portExists)
        {
            QSharedPointer<Port> testPort (new Port(portReference));
            testComponent->getPorts()->append(testPort);

            if (portHasDefault)
            {
                testPort->setDefaultValue("27");
            }
        }
    }

    QSharedPointer<AdHocConnectionValidator> validator = createAdHocConnectionValidator(mockLibrary);
    validator->changeComponentInstances(instances);

    QCOMPARE(validator->hasValidPortReferences(testConnection), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("No component reference set for internal port reference in ad hoc "
            "connection %1 within %2").arg(testConnection->name()).arg("test");

        if (!instanceExists)
        {
            expectedError = QObject::tr("Component instance %1 referenced by internal port reference in ad hoc "
                "connection %2 within %3 was not found")
                .arg(componentReference).arg(testConnection->name()).arg("test");
        }
        else if (portReference.isEmpty())
        {
            expectedError = QObject::tr("No port reference set for internal port reference in ad hoc connection "
                "%1 within %2").arg(testConnection->name()).arg("test");
        }
        else if (!portExists)
        {
            expectedError = QObject::tr("Port '%1' referenced by the internal port reference  in ad hoc "
                "connection %2 within %3 was not found")
                .arg(portReference).arg(testConnection->name()).arg("test");
        }
        else if (!portHasDefault && instanceExists && !componentReference.isEmpty() && !portReference.isEmpty() &&
            portExists)
        {
            expectedError = QObject::tr("No default value found for port '%1' referenced by internal port "
                "reference in ad hoc connection %2 within %3")
                .arg(portReference).arg(testConnection->name()).arg("test");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionHasValidInternalPortReference_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionHasValidInternalPortReference_data()
{
    QTest::addColumn<QString>("tiedValue");
    QTest::addColumn<QString>("componentReference");
    QTest::addColumn<bool>("instanceExists");
    QTest::addColumn<QString>("portReference");
    QTest::addColumn<bool>("portExists");
    QTest::addColumn<bool>("portHasDefault");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Internal port reference referencing existing component and port is valid") <<
        "" << "Space" << true << "Dandy" << true << false << true;
    QTest::newRow("Tied value default and internal port referenced port with default value is valid") << 
        "default" << "Space" << true << "Dandy" << true << true << true;

    QTest::newRow("Tied value default and internal port referenced port without default value is not valid") << 
        "default" << "Space" << true << "Dandy" << true << false << false;
    QTest::newRow("Internal port reference without component reference is not valid") <<
        "" << "" << true << "Dandy" << true << false << false;
    QTest::newRow("Internal port reference referencing non-existing component instance is not valid") <<
        "" << "Space" << false << "Dandy" << true << false << false;
    QTest::newRow("Internal port reference without port reference is not valid") <<
        "open" << "Space" << true << "" << true << false << false;
    QTest::newRow("Internal port reference referencing non-existing port is not valid") <<
        "1" << "Space" << true << "Dandy" << false << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionHasValidExternalPortReference()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionHasValidExternalPortReference()
{
    QFETCH(QString, portReference);
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<AdHocConnection> testConnection (new AdHocConnection("Yatagarasu"));

    QSharedPointer<PortReference> externalReference (new PortReference(portReference));
    externalReference->setIsPresent(isPresent);
    testConnection->getExternalPortReferences()->append(externalReference);

    QSharedPointer<AdHocConnectionValidator> validator = createAdHocConnectionValidator(0);

    QCOMPARE(validator->hasValidPortReferences(testConnection), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConnection, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for external port reference within ad hoc "
            "connection %1").arg(testConnection->name());

        if (portReference.isEmpty())
        {
            expectedError = QObject::tr("No port reference set for external port reference in ad hoc "
                "connection %1 within %2").arg(testConnection->name()).arg("test");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionHasValidExternalPortReference_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionHasValidExternalPortReference_data()
{
    QTest::addColumn<QString>("portReference");
    QTest::addColumn<QString>("isPresent");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("External port reference with port reference is valid") << "Baldurs" << "" << true;
    QTest::newRow("External port without port reference is not valid") << "" << "" << false;

    QTest::newRow("Port reference with isPresent 1 is valid") << "Baldurs" << "1" << true;
    QTest::newRow("Port reference with isPresent 1*3-3 is valid") << "Baldurs" <<  "1*3-3" << true;
    QTest::newRow("Port reference with isPresent 2*100 is invalid") << "Baldurs" << "2*100" << false;
    QTest::newRow("Port reference with isPresent -14 is invalid") << "Baldurs" << "-14" << false;
    QTest::newRow("Port reference with real number isPresent  0.12 is invalid") << "Baldurs" << "0.12" << false;
    QTest::newRow("Port reference with text as isPresent is invalid") << "Baldurs" << "test" << false;
    QTest::newRow("Port reference with string as isPresent is invalid") << "Baldurs" << "\"test\"" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionPortReferenceHasValidPartSelect()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionPortReferenceHasValidPartSelect()
{
    QFETCH(bool, partSelectExists);
    QFETCH(QString, rangeLeft);
    QFETCH(QString, rangeRight);
    QFETCH(QString, index);
    QFETCH(bool, isValid);

    QSharedPointer<PortReference> externalReference (new PortReference("Namida"));

    if (partSelectExists)
    {
        QSharedPointer<PartSelect> testPart (new PartSelect(rangeLeft, rangeRight));
        if (!index.isEmpty())
        {
            testPart->getIndices()->append(index);
        }

        externalReference->setPartSelect(testPart);
    }

    QSharedPointer<AdHocConnection> testConnection (new AdHocConnection("Yatagarasu"));
    testConnection->getExternalPortReferences()->append(externalReference);

    QSharedPointer<AdHocConnectionValidator> validator = createAdHocConnectionValidator(0);

    QCOMPARE(validator->hasValidPortReferences(testConnection), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConnection, "test");

        QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
        bool indexToIntOk = true;
        bool leftToIntOk = true;

        parser->parseExpression(index).toInt(&indexToIntOk);
        int intLeft = parser->parseExpression(rangeLeft).toInt(&leftToIntOk);

        QString expectedError ("");

        if (rangeLeft.isEmpty() && rangeRight.isEmpty() && index.isEmpty())
        {
            expectedError = QObject::tr("No range or index set for part select in external port reference in ad "
                "hoc connection %1 within %2")
                .arg(testConnection->name()).arg("test");
        }
        else if (intLeft < 0 || !leftToIntOk)
        {
            expectedError = QObject::tr("Invalid left value '%1' set for external port reference part select "
                "in ad hoc connection %2 within %3")
                .arg(rangeLeft).arg(testConnection->name()).arg("test");
        }

        else if (rangeRight.isEmpty())
        {
            expectedError = QObject::tr("Invalid right value '%1' set for external port reference part select in "
                "ad hoc connection %2 within %3")
                .arg(rangeRight).arg(testConnection->name()).arg("test");
        }
        else if (!indexToIntOk)
        {
            expectedError = QObject::tr("Invalid index value '%1' set for external port reference part select in "
                "ad hoc connection %2 within %3")
                .arg(index).arg(testConnection->name()).arg("test");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testAdHocConnectionPortReferenceHasValidPartSelect_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testAdHocConnectionPortReferenceHasValidPartSelect_data()
{
    QTest::addColumn<bool>("partSelectExists");
    QTest::addColumn<QString>("rangeLeft");
    QTest::addColumn<QString>("rangeRight");
    QTest::addColumn<QString>("index");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Port reference without part select is valid") << false << "" << "" << "" << true;
    QTest::newRow("Part select with range is valid") << true << "4" << "10" << "" << true;
    QTest::newRow("Part select with index is valid") << true << "" << "" << "8" << true;
    QTest::newRow("Part select with range and index is valid") << true << "4" << "10" << "8" << true;
    
    QTest::newRow("Part select with negative values is not valid") << true << "-2" << "4" << "" << false;
    QTest::newRow("Part select with text as values is not valid") << true << "4" << "10" << "text" << false;
    QTest::newRow("Part select without range and index is not valid") << true << "" << "" << "" << false;
    QTest::newRow("Part select range without left value is not valid") << true << "" << "10" << "" << false;
    QTest::newRow("Part select range without right value is not valid") << true << "4" << "" << "" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidAdHocConnections()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidAdHocConnections()
{
    QFETCH(bool, createAdHocConnection);
    QFETCH(bool, internalPortReferenceExists);
    QFETCH(bool, copyAdHocConenction);
    QFETCH(bool, isValid);

    QSharedPointer<Design> testDesign (new Design(VLNV(VLNV::DESIGN, "Samurai", "Champloo", "MugenJinFuu", "3")));

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<AdHocConnection> testConnection (new AdHocConnection("AdHoc"));

    if (createAdHocConnection)
    {
        if (internalPortReferenceExists)
        {
            QSharedPointer<PortReference> testReference (new PortReference("Saitama", "Genos"));

            QSharedPointer<ConfigurableVLNVReference> componentVLNV (
                new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "One", "Punch", "Man", "Saitama")));
            QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));
            mockLibrary->addComponent(testComponent);

            QSharedPointer<ComponentInstance> testInstance(
                new ComponentInstance(testReference->getComponentRef(), componentVLNV));
            testDesign->getComponentInstances()->append(testInstance);

            QSharedPointer<Port> referencedPort (new Port(testReference->getPortRef()));
            testComponent->getPorts()->append(referencedPort);

            testConnection->getInternalPortReferences()->append(testReference);
        }

        testDesign->getAdHocConnections()->append(testConnection);

        if (copyAdHocConenction)
        {
            QSharedPointer<AdHocConnection> otherConnection (new AdHocConnection(*testConnection.data()));
            testDesign->getAdHocConnections()->append(otherConnection);
        }
    }

    QSharedPointer<DesignValidator> validator = createDesignValidator(mockLibrary);

    QCOMPARE(validator->hasValidAdHocConnections(testDesign), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testDesign);

        QString expectedError = QObject::tr("Ad hoc connection name %1 within design %2 is not unique")
            .arg(testConnection->name()).arg(testDesign->getVlnv().toString());

        if (!internalPortReferenceExists)
        {
            expectedError = QObject::tr("No port references set for ad hoc connection %1 within design %2")
                .arg(testConnection->name()).arg(testDesign->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidAdHocConnections_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidAdHocConnections_data()
{
    QTest::addColumn<bool>("createAdHocConnection");
    QTest::addColumn<bool>("internalPortReferenceExists");
    QTest::addColumn<bool>("copyAdHocConenction");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design without ad hoc connections is valid") << false << false << false << true;
    QTest::newRow("Ad hoc connection with port reference is valid") << true << true << false << true;

    QTest::newRow("Ad hoc connection without port reference is not valid") << true << false << false << false;
    QTest::newRow("Ad hoc connections with non-unique names are not valid") << true << true << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidParameters()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidParameters()
{
    QFETCH(QString, name);
    QFETCH(QString, value);
    QFETCH(bool, copyParameter);
    QFETCH(bool, isValid);

    QSharedPointer<Parameter> testParameter (new Parameter());
    testParameter->setName(name);
    testParameter->setValue(value);

    QSharedPointer<Design> testDesign (new Design(VLNV(VLNV::DESIGN, "Samurai", "Champloo", "MugenJinFuu", "3")));

    testDesign->getParameters()->append(testParameter);

    if (copyParameter)
    {
        QSharedPointer<Parameter> otherParameter (new Parameter(*testParameter.data()));
        testDesign->getParameters()->append(otherParameter);
    }

    QSharedPointer<DesignValidator> validator = createDesignValidator(0);

    QCOMPARE(validator->hasValidParameters(testDesign), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testDesign);

        QString expectedError =
            QObject::tr("No valid name specified for parameter %1 within design %2")
            .arg(name).arg(testDesign->getVlnv().toString());

        if (value.isEmpty())
        {
            expectedError = QObject::tr("No value specified for parameter %1 within design %2")
                .arg(name).arg(testDesign->getVlnv().toString());
        }
        else if (copyParameter)
        {
            expectedError = QObject::tr("Parameter name %1 within design %2 is not unique.")
                .arg(name).arg(testDesign->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidParameters_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidParameters_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("value");
    QTest::addColumn<bool>("copyParameter");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Parameter with name and value is valid") << "Gunmen" << "8" << false << true;
    QTest::newRow("Parameter without name is not valid") << "" << "8" << false << false;
    QTest::newRow("Parameter without value is not valid") << "Gunmen" << "" << false << false;
    QTest::newRow("Parameters with the same name is not valid") << "Gunmen" << "8" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidAssertions()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidAssertions()
{
    QFETCH(QString, name);
    QFETCH(QString, assertValue);
    QFETCH(bool, copyAssertion);
    QFETCH(bool, isValid);

    QSharedPointer<Assertion> testAssertion (new Assertion());
    testAssertion->setName(name);
    testAssertion->setAssert(assertValue);

    QSharedPointer<Design> testDesign (new Design(VLNV(VLNV::DESIGN, "Samurai", "Champloo", "MugenJinFuu", "3")));
    testDesign->getAssertions()->append(testAssertion);

    if (copyAssertion)
    {
        QSharedPointer<Assertion> otherAssertion (new Assertion(*testAssertion.data()));
        testDesign->getAssertions()->append(otherAssertion);
    }

    QSharedPointer<DesignValidator> validator = createDesignValidator(0);

    QCOMPARE(validator->hasValidAssertions(testDesign), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testDesign);

        QString expectedError = QObject::tr("Invalid name set for assertion %1 within design %2")
            .arg(name).arg(testDesign->getVlnv().toString());

        QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
        bool assertOk = true;
        int parsedAssert = parser->parseExpression(assertValue).toInt(&assertOk);

        if (!assertOk || parsedAssert != 1)
        {
            expectedError = QObject::tr("Invalid assert set for assertion %1 within design %2")
                .arg(name).arg(testDesign->getVlnv().toString());
        }
        else if (copyAssertion)
        {
            expectedError = QObject::tr("Assertion name %1 within design %2 is not unique.")
                .arg(name).arg(testDesign->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::testHasValidAssertions_data()
//-----------------------------------------------------------------------------
void tst_DesignValidator::testHasValidAssertions_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("assertValue");
    QTest::addColumn<bool>("copyAssertion");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Assertion with name and assert value of 1 is valid") << "Onibocho" << "1" << false << true;
    QTest::newRow("Assertion without name is not valid") << "" << "4-3" << false << false;
    QTest::newRow("Assertion with incorrect assert value is not valid") << "Onibocho" << "4*2" << false << false;
    QTest::newRow("Assertion with assert value 0 is not valid") << "Onibocho" << "1*2-2" << false << false;
    QTest::newRow("Assertion without assert value is not valid") << "Onibocho" << "" << false << false;
    QTest::newRow("Assertions with the same name is not valid") << "Onibocho" << "1" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_DesignValidator::errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList)
{
    if (!errorList.contains(expectedError))
    {
        qDebug() << "The following error:" << endl << expectedError << endl << "was not found in error list:";
        foreach(QString error, errorList)
        {
            qDebug() << error;
        }
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::createDesignValidator()
//-----------------------------------------------------------------------------
QSharedPointer<DesignValidator> tst_DesignValidator::createDesignValidator(LibraryInterface* library)
{
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());

    QSharedPointer<DesignValidator> validator (new DesignValidator(parser, library));

    return validator;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::createComponentInstanceValidator()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstanceValidator> tst_DesignValidator::createComponentInstanceValidator(
    LibraryInterface* library)
{
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());

    QSharedPointer<ComponentInstanceValidator> validator (new ComponentInstanceValidator(parser, library));

    return validator;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::createInterconnectionValidator()
//-----------------------------------------------------------------------------
QSharedPointer<InterconnectionValidator> tst_DesignValidator::createInterconnectionValidator(
    LibraryInterface* library)
{
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());

    QSharedPointer<InterconnectionValidator> validator (new InterconnectionValidator(parser, library));

    return validator;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignValidator::createAdHocConnectionValidator()
//-----------------------------------------------------------------------------
QSharedPointer<AdHocConnectionValidator> tst_DesignValidator::createAdHocConnectionValidator(
    LibraryInterface* library)
{
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());

    QSharedPointer<AdHocConnectionValidator> validator (new AdHocConnectionValidator(parser, library));

    return validator;
}

QTEST_APPLESS_MAIN(tst_DesignValidator)

#include "tst_DesignValidator.moc"
