//-----------------------------------------------------------------------------
// File: tst_DesignConfigurationValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.01.2016
//
// Description:
// Unit test for class DesignConfigurationValidator.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/common/VLNV.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/Assertion.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/View.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/designConfiguration/DesignConfiguration.h>
#include <IPXACTmodels/designConfiguration/InterconnectionConfiguration.h>
#include <IPXACTmodels/designConfiguration/MultipleAbstractorInstances.h>
#include <IPXACTmodels/designConfiguration/AbstractorInstance.h>
#include <IPXACTmodels/designConfiguration/InterfaceRef.h>
#include <IPXACTmodels/designConfiguration/ViewConfiguration.h>

#include <IPXACTmodels/designConfiguration/validators/DesignConfigurationValidator.h>
#include <IPXACTmodels/designConfiguration/validators/InterconnectionConfigurationValidator.h>
#include <IPXACTmodels/designConfiguration/validators/ViewConfigurationValidator.h>

#include <editors/ComponentEditor/common/SystemVerilogExpressionParser.h>

#include <tests/MockObjects/LibraryMock.h>

#include <QtTest>
#include "IPXACTmodels/kactusExtensions/SWView.h"

class tst_DesignConfigurationValidator : public QObject
{
    Q_OBJECT

public:
    tst_DesignConfigurationValidator();

private slots:

    void testHasValidVLNV();
    void testHasValidVLNV_data();

    void testHasValidDesignReference();
    void testHasValidDesignReference_data();

    void testHasValidGeneratorChainConfigurations();
    void testHasValidGeneratorChainConfigurations_data();

    void testInterconnectionConfigurationHasValidInterconnectionReference();
    void testInterconnectionConfigurationHasValidInterconnectionReference_data();

    void testInterconnectionConfigurationHasValidIsPresent();
    void testInterconnectionConfigurationHasValidIsPresent_data();

    void testInterconnectionConfigurationHasValidAbstractorInstance();
    void testInterconnectionConfigurationHasValidAbstractorInstance_data();

    void testInterconnectionConfigurationHasValidAbstractorIsPresent();
    void testInterconnectionConfigurationHasValidAbstractorIsPresent_data();

    void testInterconnectionConfigurationHasValidInterfaceReference();
    void testInterconnectionConfigurationHasValidInterfaceReference_data();

    void testInterconnectionConfigurationInterfaceHasValidIsPresent();
    void testInterconnectionConfigurationInterfaceHasValidIsPresent_data();

    void testHasValidInterconnectionConfigurations();
    void testHasValidInterconnectionConfigurations_data();

    void testViewConfigurationHasValidName();
    void testViewConfigurationHasValidName_data();

    void testViewConfigurationHasValidIsPresent();
    void testViewConfigurationHasValidIsPresent_data();

    void testViewConfigurationHasValidViewReference();
	void testViewConfigurationHasValidViewReference_data();

	void testSWViewConfigurationHasValidViewReference();
	void testSWViewConfigurationHasValidViewReference_data();

    void testHasValidViewConfigurations();
    void testHasValidViewConfigurations_data();

    void testHasValidParameters();
    void testHasValidParameters_data();

    void testHasValidAssertions();
    void testHasValidAssertions_data();

private:

    bool errorIsNotFoundInErrorList(QString const& expectedError, QVector<QString> errorList);

    QSharedPointer<DesignConfigurationValidator> createDesignConfigurationValidator(LibraryInterface* library);

    QSharedPointer<InterconnectionConfigurationValidator> createInterconnectionConfigurationValidator(
        LibraryInterface* library);

    QSharedPointer<ViewConfigurationValidator> createViewConfigurationValidator(LibraryInterface* library);
};

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::tst_DesignConfigurationValidator()
//-----------------------------------------------------------------------------
tst_DesignConfigurationValidator::tst_DesignConfigurationValidator()
{
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidVLNV()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidVLNV()
{
    QFETCH(QString, vendor);
    QFETCH(QString, library);
    QFETCH(QString, name);
    QFETCH(QString, version);
    QFETCH(bool, isValid);

    VLNV designConfigurationVLNV(VLNV::DESIGNCONFIGURATION, vendor, library, name, version);
    QSharedPointer<DesignConfiguration> testConfiguration (new DesignConfiguration(designConfigurationVLNV));

    QSharedPointer<DesignConfigurationValidator> validator = createDesignConfigurationValidator(0);
    QCOMPARE(validator->hasValidVLNV(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration);

        QString expectedError = QObject::tr("The type of the vlnv is invalid within design configuration");

        if (vendor.isEmpty())
        {
            expectedError = QObject::tr("No vendor specified for vlnv within design configuration");
        }
        else if (library.isEmpty())
        {
            expectedError = QObject::tr("No library specified for vlnv within design configuration");
        }
        else if (name.isEmpty())
        {
            expectedError = QObject::tr("No name specified for vlnv within design configuration");
        }
        else if (version.isEmpty())
        {
            expectedError = QObject::tr("No version specified for vlnv within design configuration");
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidVLNV_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidVLNV_data()
{
    QTest::addColumn<QString>("vendor");
    QTest::addColumn<QString>("library");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("version");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design configuration with all VLNVs is valid") <<
        "Tengen" << "Toppa" << "Gurren" << "Lagann" << true;
    QTest::newRow("Design configuration without vendor is not valid") <<
        "" << "Toppa" << "Gurren" << "Lagann" << false;
    QTest::newRow("Design configuration without library is not valid") <<
        "Tengen" << "" << "Gurren" << "Lagann" << false;
    QTest::newRow("Design configuration without name is not valid") <<
        "Tengen" << "Toppa" << "" << "Lagann" << false;
    QTest::newRow("Design configuration without version is not valid") <<
        "Tengen" << "Toppa" << "Gurren" << "" << false;
    QTest::newRow("Empty VLNV is not valid") << "" << "" << "" << "" << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidDesignReference()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidDesignReference()
{
    QFETCH(bool, referenceExists);
    QFETCH(QString, vendor);
    QFETCH(QString, library);
    QFETCH(QString, name);
    QFETCH(QString, version);
    QFETCH(bool, addToLibrary);
    QFETCH(bool, isValid);

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<DesignConfiguration> testConfiguration (new DesignConfiguration(
        VLNV(VLNV::DESIGNCONFIGURATION, "Samurai", "Champloo", "MugenJinFuu", "3.0")));

    VLNV designReference (VLNV::DESIGN, vendor, library, name, version);
    
    if (referenceExists)
    {
        testConfiguration->setDesignRef(designReference);

        if (addToLibrary)
        {
            QSharedPointer<Design> referencedDesign (new Design(designReference));
            mockLibrary->addComponent(referencedDesign);
        }
    }

    QSharedPointer<DesignConfigurationValidator> validator = createDesignConfigurationValidator(mockLibrary);

    QCOMPARE(validator->hasValidDesignReference(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration);

        QString expectedError = QObject::tr("The type of the vlnv is invalid within design reference");

        if (vendor.isEmpty())
        {
            expectedError = QObject::tr("No vendor specified for vlnv within design reference");
        }
        else if (library.isEmpty())
        {
            expectedError = QObject::tr("No library specified for vlnv within design reference");
        }
        else if (name.isEmpty())
        {
            expectedError = QObject::tr("No name specified for vlnv within design reference");
        }
        else if (version.isEmpty())
        {
            expectedError = QObject::tr("No version specified for vlnv within design reference");
        }
        else if (!addToLibrary)
        {
            expectedError = QObject::tr("Design reference %1 within design configuration %2 was not found in the "
                "library").arg(designReference.toString()).arg(testConfiguration->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidDesignReference_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidDesignReference_data()
{
    QTest::addColumn<bool>("referenceExists");
    QTest::addColumn<QString>("vendor");
    QTest::addColumn<QString>("library");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("version");
    QTest::addColumn<bool>("addToLibrary");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design configuration without a design reference is valid") <<
        false << "" << "" << "" << "" << false << true;

    QTest::newRow("Design reference with VLNV and referencing a design contained in library is valid") <<
        true << "Tengen" << "Toppa" << "Gurren" << "Lagann" << true << true;
    QTest::newRow("Design reference without vendor is not valid") <<
        true << "" << "Toppa" << "Gurren" << "Lagann" << true << false;
    QTest::newRow("Design reference without library is not valid") <<
        true << "Tengen" << "" << "Gurren" << "Lagann" << true << false;
    QTest::newRow("Design reference without name is not valid") <<
        true << "Tengen" << "Toppa" << "" << "Lagann" << true << false;
    QTest::newRow("Design reference without version is not valid") <<
        true << "Tengen" << "Toppa" << "Gurren" << "" << true << false;

    QTest::newRow("Design reference referencing design not found in library is not valid") <<
        true << "Tengen" << "Toppa" << "Gurren" << "Lagann" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidGeneratorChainConfigurations()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidGeneratorChainConfigurations()
{
    QFETCH(bool, referenceExists);
    QFETCH(QString, vendor);
    QFETCH(QString, library);
    QFETCH(QString, name);
    QFETCH(QString, version);
    QFETCH(bool, copyChain);
    QFETCH(bool, isValid);

    QSharedPointer<DesignConfiguration> testConfiguration (new DesignConfiguration(
        VLNV(VLNV::DESIGNCONFIGURATION, "Samurai", "Champloo", "MugenJinFuu", "3.0")));

    QSharedPointer<ConfigurableVLNVReference> chainVLNV (
        new ConfigurableVLNVReference(VLNV::GENERATORCHAIN, vendor, library, name, version));

    if (referenceExists)
    {
        testConfiguration->getGeneratorChainConfs()->append(chainVLNV);

        if (copyChain)
        {
            QSharedPointer<ConfigurableVLNVReference> otherChain (new ConfigurableVLNVReference(*chainVLNV.data()));
            testConfiguration->getGeneratorChainConfs()->append(otherChain);
        }
    }

    QSharedPointer<DesignConfigurationValidator> validator = createDesignConfigurationValidator(0);

    QCOMPARE(validator->hasValidGeneratorChainConfigurations(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration);

        QString expectedError = QObject::tr("The type of the generator chain configuration is invalid within "
            "design configuration %1.").arg(testConfiguration->getVlnv().toString());

        if (vendor.isEmpty())
        {
            expectedError = QObject::tr("No vendor specified for vlnv within generator chain configuration");
        }
        else if (library.isEmpty())
        {
            expectedError = QObject::tr("No library specified for vlnv within generator chain configuration");
        }
        else if (name.isEmpty())
        {
            expectedError = QObject::tr("No name specified for vlnv within generator chain configuration");
        }
        else if (version.isEmpty())
        {
            expectedError = QObject::tr("No version specified for vlnv within generator chain configuration");
        }
        else if (copyChain)
        {
            expectedError = QObject::tr("Generator chain configuration %1 is not unique within design "
                "configuration %2.").arg(chainVLNV->toString()).arg(testConfiguration->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidGeneratorChainConfigurations_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidGeneratorChainConfigurations_data()
{
    QTest::addColumn<bool>("referenceExists");
    QTest::addColumn<QString>("vendor");
    QTest::addColumn<QString>("library");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("version");
    QTest::addColumn<bool>("copyChain");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design configuration without generator chains is valid") <<
        false << "" << "" << "" << "" << false << true;
    QTest::newRow("Generator chain with VLNV is valid") <<
        true << "Evangelion" << "Shin" << "Geki" << "jouban" << false << true;

    QTest::newRow("Generator chain without vendor is not valid") <<
        true << "" << "Shin" << "Geki" << "jouban" << false << false;
    QTest::newRow("Generator chain without library is not valid") <<
        true << "Evangelion" << "" << "Geki" << "jouban" << false << false;
    QTest::newRow("Generator chain without name is not valid") <<
        true << "Evangelion" << "Shin" << "" << "jouban" << false << false;
    QTest::newRow("Generator chain without version is not valid") <<
        true << "Evangelion" << "Shin" << "Geki" << "" << false << false;
    
    QTest::newRow("Generator chains referencing the same generator chain is not valid") <<
        true << "Evangelion" << "Shin" << "Geki" << "jouban" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidInterconnectionReference()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidInterconnectionReference()
{
    QFETCH(QString, interconnectionName);
    QFETCH(bool, referenceOk);
    QFETCH(bool, isValid);

    QSharedPointer<Design> testDesign (new Design(VLNV(VLNV::DESIGN, "vendor", "library", "design", "1")));

    QSharedPointer<InterconnectionConfiguration> testConfiguration (new InterconnectionConfiguration());
    testConfiguration->setInterconnectionReference(interconnectionName);
    
    if (referenceOk)
    {
        QSharedPointer<Interconnection> testConnection (new Interconnection());

        if (interconnectionName.isEmpty())
        {
            testConnection->setName("Champloo");
        }
        else
        {
            testConnection->setName(interconnectionName);
        }

        testDesign->getInterconnections()->append(testConnection);
    }

    QSharedPointer<InterconnectionConfigurationValidator> validator =
        createInterconnectionConfigurationValidator(0);
    validator->designChanged(testDesign);

    QCOMPARE(validator->hasValidInterconnectionReference(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration, "test");

        QString expectedError = QObject::tr("Invalid interconnection reference '%1' given for interconnection "
            "configuration within %2.")
            .arg(interconnectionName).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidInterconnectionReference_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidInterconnectionReference_data()
{
    QTest::addColumn<QString>("interconnectionName");
    QTest::addColumn<bool>("referenceOk");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Interconnection configuration referencing existing interconnection is valid") <<
        "Baldur" << true << true;
    QTest::newRow("Empty interconnection reference is not valid") << "" << true << false;
    QTest::newRow("Interconnection configuration referencing a non-existing interconnection is not valid") <<
        "Baldur" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<InterconnectionConfiguration> testConfiguration (new InterconnectionConfiguration());
    testConfiguration->setInterconnectionReference("deCarabas");
    testConfiguration->setIsPresent(isPresent);

    QSharedPointer<InterconnectionConfigurationValidator> validator =
        createInterconnectionConfigurationValidator(0);

    QCOMPARE(validator->hasValidIsPresent(testConfiguration->getIsPresent()), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for interconnection configuration %1 within %2")
            .arg(testConfiguration->getInterconnectionReference()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidIsPresent_data()
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
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidAbstractorInstance()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidAbstractorInstance()
{
    QFETCH(QString, instanceName);
    QFETCH(QString, abstractorVendor);
    QFETCH(QString, abstractorLibrary);
    QFETCH(QString, abstractorName);
    QFETCH(QString, abstractorVersion);
    QFETCH(QString, viewName);
    QFETCH(bool, copyInstance);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractorRef (new ConfigurableVLNVReference(
        VLNV::ABSTRACTOR, abstractorVendor, abstractorLibrary, abstractorName, abstractorVersion));

    QSharedPointer<AbstractorInstance> testInstance (new AbstractorInstance());
    testInstance->setInstanceName(instanceName);
    testInstance->setAbstractorRef(abstractorRef);
    testInstance->setViewName(viewName);

    QSharedPointer<MultipleAbstractorInstances> testMultipleInstance (new MultipleAbstractorInstances());
    testMultipleInstance->getAbstractorInstances()->append(testInstance);

    if (copyInstance)
    {
        QSharedPointer<AbstractorInstance> otherInstance (new AbstractorInstance(*testInstance.data()));
        testMultipleInstance->getAbstractorInstances()->append(otherInstance);
    }

    QSharedPointer<InterconnectionConfiguration> testConfiguration (new InterconnectionConfiguration());
    testConfiguration->setInterconnectionReference("Samurai");
    testConfiguration->getAbstractorInstances()->append(testMultipleInstance);

    QSharedPointer<InterconnectionConfigurationValidator> validator =
        createInterconnectionConfigurationValidator(0);

    QCOMPARE(validator->hasValidMultipleAbstractorInstances(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration, "test");

        QString expectedError = QObject::tr("Invalid instance name '%1' set for abstractor instance within "
            "interconnection configuration %2")
            .arg(instanceName).arg(testConfiguration->getInterconnectionReference());

        if (abstractorVendor.isEmpty())
        {
            expectedError = QObject::tr("No vendor specified for vlnv within abstractor instance %1").
                arg(instanceName);
        }
        else if (abstractorLibrary.isEmpty())
        {
            expectedError = QObject::tr("No library specified for vlnv within abstractor instance %1").
                arg(instanceName);
        }
        else if (abstractorName.isEmpty())
        {
            expectedError = QObject::tr("No name specified for vlnv within abstractor instance %1").
                arg(instanceName);
        }
        else if (abstractorVersion.isEmpty())
        {
            expectedError = QObject::tr("No version specified for vlnv within abstractor instance %1").
                arg(instanceName);
        }
        else if (viewName.isEmpty())
        {
            expectedError = QObject::tr("Invalid view name '%1' set for abstractor instance %2 within "
                "interconnection configuration %3")
                .arg(viewName).arg(instanceName).arg(testConfiguration->getInterconnectionReference());
        }
        else if (copyInstance)
        {
            expectedError = QObject::tr("Abstractor instance name '%1' within interconnection configuration %2 is "
                "not unique.")
                .arg(instanceName).arg(testConfiguration->getInterconnectionReference());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidAbstractorInstance_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidAbstractorInstance_data()
{
    QTest::addColumn<QString>("instanceName");
    QTest::addColumn<QString>("abstractorVendor");
    QTest::addColumn<QString>("abstractorLibrary");
    QTest::addColumn<QString>("abstractorName");
    QTest::addColumn<QString>("abstractorVersion");
    QTest::addColumn<QString>("viewName");
    QTest::addColumn<bool>("copyInstance");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Abstractor instance with name, abstractor reference and view name is valid") <<
        "Infinity" << "Yu-Jing" << "JSA" << "O-yoroi" << "Kidobutai" << "HMG" << false << true;
    QTest::newRow("Abstractor instance without name is not valid") <<
        "" << "Yu-Jing" << "JSA" << "O-yoroi" << "Kidobutai" << "HMG" << false << false;

    QTest::newRow("Abstractor instance without referenced abstractor vendor is not valid") <<
        "Infinity" << "" << "JSA" << "O-yoroi" << "Kidobutai" << "HMG" << false << false;
    QTest::newRow("Abstractor instance without referenced abstractor library is not valid") <<
        "Infinity" << "Yu-Jing" << "" << "O-yoroi" << "Kidobutai" << "HMG" << false << false;
    QTest::newRow("Abstractor instance without referenced abstractor name is not valid") <<
        "Infinity" << "Yu-Jing" << "JSA" << "" << "Kidobutai" << "HMG" << false << false;
    QTest::newRow("Abstractor instance without referenced abstractor version is not valid") <<
        "Infinity" << "Yu-Jing" << "JSA" << "O-yoroi" << "" << "HMG" << false << false;

    QTest::newRow("Abstractor instance without view name is not valid") <<
        "Infinity" << "Yu-Jing" << "JSA" << "O-yoroi" << "Kidobutai" << "" << false << false;

    QTest::newRow("Abstractor instance with non-unique instance name is not valid") <<
        "Infinity" << "Yu-Jing" << "JSA" << "O-yoroi" << "Kidobutai" << "HMG" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidAbstractorIsPresent()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidAbstractorIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<ConfigurableVLNVReference> abstractorRef (new ConfigurableVLNVReference(VLNV::ABSTRACTOR,
        QLatin1String("Yu-Jing"), QLatin1String("JSA"), QLatin1String("O-yoroi"), QLatin1String("Kidobutai")));

    QSharedPointer<AbstractorInstance> testInstance (new AbstractorInstance());
    testInstance->setInstanceName("Infinity");
    testInstance->setAbstractorRef(abstractorRef);
    testInstance->setViewName("HMG");

    QSharedPointer<MultipleAbstractorInstances> testMultipleInstance (new MultipleAbstractorInstances());
    testMultipleInstance->getAbstractorInstances()->append(testInstance);
    testMultipleInstance->setIsPresent(isPresent);

    QSharedPointer<InterconnectionConfiguration> testConfiguration (new InterconnectionConfiguration());
    testConfiguration->setInterconnectionReference("Samurai");
    testConfiguration->getAbstractorInstances()->append(testMultipleInstance);

    QSharedPointer<InterconnectionConfigurationValidator> validator =
        createInterconnectionConfigurationValidator(0);

    QCOMPARE(validator->hasValidMultipleAbstractorInstances(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for abstractor instance group within "
            "interconnection configuration %1").arg(testConfiguration->getInterconnectionReference());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidAbstractorIsPresent_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidAbstractorIsPresent_data()
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
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidInterfaceReference()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidInterfaceReference()
{
    QFETCH(bool, interfaceRefExists);
    QFETCH(QString, componentReference);
    QFETCH(bool, componentInstanceExists);
    QFETCH(QString, busReference);
    QFETCH(bool, busInterfaceExists);
    QFETCH(bool, isValid);

    QSharedPointer<Design> testDesign (new Design(VLNV(VLNV::DESIGN, "vendor", "library", "design", "version")));

    QSharedPointer<ConfigurableVLNVReference> abstractorRef (new ConfigurableVLNVReference(VLNV::ABSTRACTOR,
        QLatin1String("Yu-Jing"), QLatin1String("JSA"), QLatin1String("O-yoroi"), QLatin1String("Kidobutai")));

    QSharedPointer<AbstractorInstance> testInstance (new AbstractorInstance());
    testInstance->setInstanceName("Infinity");
    testInstance->setAbstractorRef(abstractorRef);
    testInstance->setViewName("HMG");

    QSharedPointer<MultipleAbstractorInstances> testMultipleInstance (new MultipleAbstractorInstances());
    testMultipleInstance->getAbstractorInstances()->append(testInstance);

    if (interfaceRefExists)
    {
        QSharedPointer<InterfaceRef> interfaceReference (new InterfaceRef());
        interfaceReference->setComponentRef(componentReference);
        interfaceReference->setBusRef(busReference);

        testMultipleInstance->getInterfaceReferences()->append(interfaceReference);
    }

    QSharedPointer<ConfigurableVLNVReference> componentVLNV (
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Baldurs", "Gate", "Shadows", "Amn"));

    if (componentInstanceExists)
    {
        QSharedPointer<ComponentInstance> componentInstance (
            new ComponentInstance(componentReference, componentVLNV));

        testDesign->getComponentInstances()->append(componentInstance);
    }

    LibraryMock* mockLibrary (new LibraryMock(this));

    if (busInterfaceExists)
    {
        QSharedPointer<BusInterface> testInterface (new BusInterface());
        testInterface->setName(busReference);

        QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));
        testComponent->getBusInterfaces()->append(testInterface);

        mockLibrary->addComponent(testComponent);
    }

    QSharedPointer<InterconnectionConfiguration> testConfiguration (new InterconnectionConfiguration());
    testConfiguration->setInterconnectionReference("Samurai");
    testConfiguration->getAbstractorInstances()->append(testMultipleInstance);

    QSharedPointer<InterconnectionConfigurationValidator> validator =
        createInterconnectionConfigurationValidator(mockLibrary);
    validator->designChanged(testDesign);

    QCOMPARE(validator->hasValidMultipleAbstractorInstances(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration, "test");

        QString expectedError = QObject::tr("Invalid component instance reference '%1' set for abstractor "
            "instance group within interconnection configuration %2")
            .arg(componentReference).arg(testConfiguration->getInterconnectionReference());

        if (busReference.isEmpty() || !busInterfaceExists)
        {
            expectedError = QObject::tr("Invalid bus interface reference '%1' set for abstractor "
                "instance group within interconnection configuration %2")
                .arg(busReference).arg(testConfiguration->getInterconnectionReference());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidInterfaceReference_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationHasValidInterfaceReference_data()
{
    QTest::addColumn<bool>("interfaceRefExists");
    QTest::addColumn<QString>("componentReference");
    QTest::addColumn<bool>("componentInstanceExists");
    QTest::addColumn<QString>("busReference");
    QTest::addColumn<bool>("busInterfaceExists");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Non-existing interface reference is valid") <<
        false << "" << false << "" << false << true;
    QTest::newRow("Interface reference with component and bus reference is valid") <<
        true << "Space" << true << "Dandy" << true << true;

    QTest::newRow("Interface reference without component reference is not valid") <<
        true << "" << true << "Dandy" << true << false;
    QTest::newRow("Interface reference with component reference to non-existing instance is not valid") <<
        true << "Space" << false << "Dandy" << true << false;
    QTest::newRow("Interface reference without bus reference is not valid") <<
        true << "Space" << true << "" << true << false;
    QTest::newRow("Interface reference with bus reference to non-existing bus is not valid") <<
        true << "Space" << true << "Dandy" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationInterfaceHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationInterfaceHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<Design> testDesign (new Design(VLNV(VLNV::DESIGN, "Guilty", "Gear", "Heavy", "day")));

    QSharedPointer<ConfigurableVLNVReference> abstractorRef (new ConfigurableVLNVReference(VLNV::ABSTRACTOR,
        QLatin1String("Yu-Jing"), QLatin1String("JSA"), QLatin1String("O-yoroi"), QLatin1String("Kidobutai")));

    QSharedPointer<AbstractorInstance> testInstance (new AbstractorInstance());
    testInstance->setInstanceName("Infinity");
    testInstance->setAbstractorRef(abstractorRef);
    testInstance->setViewName("HMG");

    QSharedPointer<MultipleAbstractorInstances> testMultipleInstance (new MultipleAbstractorInstances());
    testMultipleInstance->getAbstractorInstances()->append(testInstance);

    QSharedPointer<InterfaceRef> interfaceReference (new InterfaceRef());
    interfaceReference->setComponentRef(QLatin1String("Space"));
    interfaceReference->setBusRef(QLatin1String("Dandy"));
    interfaceReference->setIsPresent(isPresent);

    testMultipleInstance->getInterfaceReferences()->append(interfaceReference);

    QSharedPointer<ConfigurableVLNVReference> componentVLNV (
        new ConfigurableVLNVReference(VLNV::COMPONENT, "Vendor", "Library", "Name", "Version"));

    QSharedPointer<ComponentInstance> componentInstance (
        new ComponentInstance(interfaceReference->getComponentRef(), componentVLNV));

    testDesign->getComponentInstances()->append(componentInstance);

    QSharedPointer<BusInterface> testInterface (new BusInterface());
    testInterface->setName(interfaceReference->getBusRef());

    QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));
    testComponent->getBusInterfaces()->append(testInterface);

    QSharedPointer<InterconnectionConfiguration> testConfiguration (new InterconnectionConfiguration());
    testConfiguration->setInterconnectionReference("Samurai");
    testConfiguration->getAbstractorInstances()->append(testMultipleInstance);

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(testComponent);

    QSharedPointer<InterconnectionConfigurationValidator> validator =
        createInterconnectionConfigurationValidator(mockLibrary);
    validator->designChanged(testDesign);

    QCOMPARE(validator->hasValidMultipleAbstractorInstances(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for interface reference in abstractor instance "
            "group within interconnection configuration %1")
            .arg(testConfiguration->getInterconnectionReference());

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testInterconnectionConfigurationInterfaceHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testInterconnectionConfigurationInterfaceHasValidIsPresent_data()
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
// Function: tst_DesignConfigurationValidator::testHasValidInterconnectionConfigurations()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidInterconnectionConfigurations()
{
    QFETCH(bool, createInterconncetionConfiguration);
    QFETCH(bool, interconnectionReferenceExists);
    QFETCH(bool, copyInterconnectionConfiguration);
    QFETCH(bool, multipleAbstractorInstanceExists);
    QFETCH(bool, abstractorInstanceExists);
    QFETCH(bool, isValid);

    LibraryMock* mockLibrary (new LibraryMock(this));

    VLNV designVLNV (VLNV::DESIGN, "Guilty", "Gear", "Xrd", "Revelator");

    QSharedPointer<Design> testDesign (new Design(designVLNV));
    mockLibrary->addComponent(testDesign);

    QSharedPointer<DesignConfiguration> testConfiguration (new DesignConfiguration(
        VLNV(VLNV::DESIGNCONFIGURATION, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testConfiguration->setDesignRef(designVLNV);

    QSharedPointer<InterconnectionConfiguration> interconnectionConfiguration (new InterconnectionConfiguration());

    if (createInterconncetionConfiguration)
    {
        if (interconnectionReferenceExists)
        {
            interconnectionConfiguration->setInterconnectionReference(QLatin1String("Evangelion"));
            QSharedPointer<Interconnection> interconnection (new Interconnection());
            interconnection->setName(interconnectionConfiguration->getInterconnectionReference());

            testDesign->getInterconnections()->append(interconnection);
        }

        if (multipleAbstractorInstanceExists)
        {
            QSharedPointer<MultipleAbstractorInstances> multipleInstance (new MultipleAbstractorInstances());
            interconnectionConfiguration->getAbstractorInstances()->append(multipleInstance);

            if (abstractorInstanceExists)
            {
                QSharedPointer<ConfigurableVLNVReference> abstractorRef (new ConfigurableVLNVReference(
                    VLNV::ABSTRACTOR, QLatin1String("Tengen"), QLatin1String("Toppa"),
                    QLatin1String("Gurren"), QLatin1String("Lagann")));

                QSharedPointer<AbstractorInstance> testInstance (new AbstractorInstance());
                testInstance->setInstanceName("Space");
                testInstance->setAbstractorRef(abstractorRef);
                testInstance->setViewName("Dandy");

                multipleInstance->getAbstractorInstances()->append(testInstance);
            }
        }

        testConfiguration->getInterconnectionConfs()->append(interconnectionConfiguration);

        if (copyInterconnectionConfiguration)
        {
            QSharedPointer<InterconnectionConfiguration> otherConfiguration (
                new InterconnectionConfiguration(*interconnectionConfiguration.data()));
            testConfiguration->getInterconnectionConfs()->append(otherConfiguration);
        }
    }

    QSharedPointer<DesignConfigurationValidator> validator = createDesignConfigurationValidator(mockLibrary);

    QCOMPARE(validator->hasValidInterconnectionConfigurations(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration);

        QString expectedError = QObject::tr("Invalid interconnection reference '%1' given for interconnection "
            "configuration within design configuration %2.")
            .arg(interconnectionConfiguration->getInterconnectionReference())
            .arg(testConfiguration->getVlnv().toString());

        if (copyInterconnectionConfiguration)
        {
            expectedError = QObject::tr("Interconnection reference '%1' set for interconnection configuration "
                "within design configuration %2 is not unique.")
                .arg(interconnectionConfiguration->getInterconnectionReference())
                .arg(testConfiguration->getVlnv().toString());
        }
        else if (!multipleAbstractorInstanceExists)
        {
            expectedError = QObject::tr("No abstractor instances found in interconnection configuration '%1' "
                "within design configuration %2")
                .arg(interconnectionConfiguration->getInterconnectionReference())
                .arg(testConfiguration->getVlnv().toString());
        }
        else if (!abstractorInstanceExists)
        {
            expectedError = QObject::tr("No abstractor instances found in abstractor instance group within "
                "interconnection configuration %2")
                .arg(interconnectionConfiguration->getInterconnectionReference());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidInterconnectionConfigurations_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidInterconnectionConfigurations_data()
{
    QTest::addColumn<bool>("createInterconncetionConfiguration");
    QTest::addColumn<bool>("interconnectionReferenceExists");
    QTest::addColumn<bool>("copyInterconnectionConfiguration");
    QTest::addColumn<bool>("multipleAbstractorInstanceExists");
    QTest::addColumn<bool>("abstractorInstanceExists");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design configuration without interconnection configurations is valid") <<
        false << false << false << false << false << true;
    QTest::newRow("Configuration with interconnection reference and abstractor instance is valid") <<
        true << true << false << true << true << true;

    QTest::newRow("Configuration without interconnection reference is not valid") <<
        true << false << false << true << true << false;
    QTest::newRow("Configurations with non-unique interconnection reference is not valid") <<
        true << true << true << true << true << false;
    QTest::newRow("Configuration without multiple abstractor instances is not valid") << 
        true << true << false << false << false << false;
    QTest::newRow("Configuration without abstractor instance is not valid") <<
        true << true << false << true << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testViewConfigurationHasValidName()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testViewConfigurationHasValidName()
{
    QFETCH(QString, name);
    QFETCH(bool, isValid);

    QSharedPointer<ComponentInstance> testInstance (new ComponentInstance());
    testInstance->setInstanceName(name);

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > instances
        (new QList<QSharedPointer<ComponentInstance> > ());
    instances->append(testInstance);

    QSharedPointer<ViewConfiguration> testConfiguration (new ViewConfiguration(name));

    QSharedPointer<ViewConfigurationValidator> validator =
        createViewConfigurationValidator(0);
    validator->changeComponentInstances(instances);

    QCOMPARE(validator->hasValidName(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration, "test");

        QString expectedError = QObject::tr("Invalid name '%1' set for view configuration within %2")
            .arg(name).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testViewConfigurationHasValidName_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testViewConfigurationHasValidName_data()
{
    QTest::addColumn<QString>("name");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Name test is valid") << "test" << true;
    QTest::newRow("Empty name is invalid") << "" << false;
    QTest::newRow("Name consisting of only white spaces is invalid") << "    " << false;
    QTest::newRow("Name consisting of characters and white spaces is valid") << "  test  " << true;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testViewConfigurationHasValidIsPresent()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testViewConfigurationHasValidIsPresent()
{
    QFETCH(QString, isPresent);
    QFETCH(bool, isValid);

    QSharedPointer<ViewConfiguration> testConfiguration (new ViewConfiguration("Karasu"));
    testConfiguration->setIsPresent(isPresent);

    QSharedPointer<ViewConfigurationValidator> validator =
        createViewConfigurationValidator(0);

    QCOMPARE(validator->hasValidIsPresent(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration, "test");

        QString expectedError = QObject::tr("Invalid isPresent set for view configuration %1 within %2")
            .arg(testConfiguration->getInstanceName()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testViewConfigurationHasValidIsPresent_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testViewConfigurationHasValidIsPresent_data()
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
// Function: tst_DesignConfigurationValidator::testViewConfigurationHasValidViewReference()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testViewConfigurationHasValidViewReference()
{
    QFETCH(QString, viewReference);
    QFETCH(bool, viewExists);
    QFETCH(bool, isValid);

    LibraryMock* mockLibrary (new LibraryMock(this));

    QSharedPointer<ViewConfiguration> testConfiguration (new ViewConfiguration("Karasu"));
    testConfiguration->setViewReference(viewReference);
    testConfiguration->setInstanceName("instantKill");

	QSharedPointer<ViewConfiguration> testSWConfiguration (new ViewConfiguration("SwViewConfiguration"));
	testSWConfiguration->setViewReference(viewReference);
	testSWConfiguration->setInstanceName("swUser");

    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > instances
        (new QList<QSharedPointer<ComponentInstance> > ());

    if (viewExists)
    {
		QSharedPointer<View> testView (new View(viewReference));
		QSharedPointer<SWView> testSWView (new SWView(viewReference));
        if (viewReference.isEmpty())
        {
			testView->setName("Champloo");
			testSWView->setName("TestSofwareView");
        }

        QSharedPointer<ConfigurableVLNVReference> componentVLNV
            (new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "vendor", "library", "testComponent", "1")));

        QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));
		testComponent->getViews()->append(testView);

		QList<QSharedPointer<SWView> > swViews = testComponent->getSWViews();
		swViews.append(testSWView);
		testComponent->setSWViews(swViews);

        mockLibrary->addComponent(testComponent);

        QSharedPointer<ComponentInstance> testInstance (
            new ComponentInstance(testConfiguration->getInstanceName(), componentVLNV));

		instances->append(testInstance);

		QSharedPointer<ComponentInstance> testSWInstance (
			new ComponentInstance(testSWConfiguration->getInstanceName(), componentVLNV));

		instances->append(testSWInstance);
    }

    QSharedPointer<ViewConfigurationValidator> validator = createViewConfigurationValidator(mockLibrary);
    validator->changeComponentInstances(instances);

	QCOMPARE(validator->hasValidViewReference(testConfiguration), isValid);
	QCOMPARE(validator->hasValidViewReference(testSWConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
		validator->findErrorsIn(foundErrors, testConfiguration, "test");
		validator->findErrorsIn(foundErrors, testSWConfiguration, "swtest");

        QString expectedError = QObject::tr("Invalid view reference '%1' set for view configuration %2 within %3")
            .arg(viewReference).arg(testConfiguration->getInstanceName()).arg("test");

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testViewConfigurationHasValidViewReference_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testViewConfigurationHasValidViewReference_data()
{
    QTest::addColumn<QString>("viewReference");
    QTest::addColumn<bool>("viewExists");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("View configuration with a view reference to an existing view is valid") <<
        "Shishigami" << true << true;
    QTest::newRow("View configuration without a view reference is not valid") << "" << true << false;
    QTest::newRow("View configuration referencing non-existing view is not valid") <<
		"Shishigami" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testSWViewConfigurationHasValidViewReference()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testSWViewConfigurationHasValidViewReference()
{
	QFETCH(QString, viewReference);
	QFETCH(bool, viewExists);
	QFETCH(bool, isValid);

	LibraryMock* mockLibrary (new LibraryMock(this));

	QSharedPointer<ViewConfiguration> testSWConfiguration (new ViewConfiguration("SwViewConfiguration"));
	testSWConfiguration->setViewReference(viewReference);
	testSWConfiguration->setInstanceName("swUser");

	QSharedPointer<QList<QSharedPointer<ComponentInstance> > > instances
		(new QList<QSharedPointer<ComponentInstance> > ());

	if (viewExists)
	{
		QSharedPointer<SWView> testSWView (new SWView(viewReference));
		if (viewReference.isEmpty())
		{
			testSWView->setName("TestSoftwareView");
		}

		QSharedPointer<ConfigurableVLNVReference> componentVLNV
			(new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "vendor", "library", "testComponent", "1")));

        QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));

		QList<QSharedPointer<SWView> > swViews = testComponent->getSWViews();
		swViews.append(testSWView);
		testComponent->setSWViews(swViews);

		mockLibrary->addComponent(testComponent);

		QSharedPointer<ComponentInstance> testSWInstance (
			new ComponentInstance(testSWConfiguration->getInstanceName(), componentVLNV));

		instances->append(testSWInstance);
	}

	QSharedPointer<ViewConfigurationValidator> validator = createViewConfigurationValidator(mockLibrary);
	validator->changeComponentInstances(instances);

	QCOMPARE(validator->hasValidViewReference(testSWConfiguration), isValid);

	if (!isValid)
	{
		QVector<QString> foundErrors;
		validator->findErrorsIn(foundErrors, testSWConfiguration, "test");

		QString expectedError = QObject::tr("Invalid view reference '%1' set for view configuration %2 within %3")
			.arg(viewReference).arg(testSWConfiguration->getInstanceName()).arg("test");

		if (errorIsNotFoundInErrorList(expectedError, foundErrors))
		{
			QFAIL("No error message found");
		}
	}
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testSWViewConfigurationHasValidViewReference_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testSWViewConfigurationHasValidViewReference_data()
{
	QTest::addColumn<QString>("viewReference");
	QTest::addColumn<bool>("viewExists");
	QTest::addColumn<bool>("isValid");

	QTest::newRow("View configuration with a view reference to an existing view is valid") <<
		"sampleSWView" << true << true;
	QTest::newRow("View configuration without a view reference is not valid") << "" << true << false;
	QTest::newRow("View configuration referencing non-existing view is not valid") <<
		"sampleSWView" << false << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidViewConfigurations()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidViewConfigurations()
{
    QFETCH(bool, createViewConfiguration);
    QFETCH(QString, instanceName);
    QFETCH(QString, viewReference);
    QFETCH(bool, copyConfiguration);
    QFETCH(bool, isValid);

    QSharedPointer<Design> referencedDesign (
        new Design(VLNV(VLNV::DESIGN, "Evangelion", "Shin", "Geki", "jouban")));

    LibraryMock* mockLibrary (new LibraryMock(this));
    mockLibrary->addComponent(referencedDesign);

    QSharedPointer<DesignConfiguration> testConfiguration (new DesignConfiguration(
        VLNV(VLNV::DESIGNCONFIGURATION, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testConfiguration->setDesignRef(referencedDesign->getVlnv());

    QSharedPointer<ConfigurableVLNVReference> componentVLNV
        (new ConfigurableVLNVReference(VLNV(VLNV::COMPONENT, "vendor", "library", "testComponent", "1")));

    if (createViewConfiguration)
    {
        QSharedPointer<ViewConfiguration> viewConfiguration (new ViewConfiguration(instanceName));
        viewConfiguration->setViewReference(viewReference);
        testConfiguration->getViewConfigurations()->append(viewConfiguration);

        QSharedPointer<View> testView (new View(viewReference));
        if (viewReference.isEmpty())
        {
            testView->setName("SaederKrupp");
        }

        QSharedPointer<Component> testComponent (new Component(*componentVLNV.data()));
        testComponent->getViews()->append(testView);

        mockLibrary->addComponent(testComponent);

        QSharedPointer<ComponentInstance> testInstance (new ComponentInstance(instanceName, componentVLNV));
        referencedDesign->getComponentInstances()->append(testInstance);

        if (copyConfiguration)
        {
            QSharedPointer<ViewConfiguration> otherView (new ViewConfiguration(*viewConfiguration.data()));
            testConfiguration->getViewConfigurations()->append(otherView);
        }
    }

    QSharedPointer<DesignConfigurationValidator> validator = createDesignConfigurationValidator(mockLibrary);

    QCOMPARE(validator->hasValidViewConfigurations(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration);

        QString expectedError = QObject::tr("Invalid name '%1' set for view configuration within design "
            "configuration %2")
            .arg(instanceName).arg(testConfiguration->getVlnv().toString());

        if (viewReference.isEmpty())
        {
            expectedError = QObject::tr("Invalid view reference '%1' set for view configuration %2 within design "
                "configuration %3")
                .arg(viewReference).arg(instanceName).arg(testConfiguration->getVlnv().toString());
        }
        else if (copyConfiguration)
        {
            expectedError = QObject::tr("View configuration name '%1' within design configuration %2 is not "
                "unique.")
                .arg(instanceName).arg(testConfiguration->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidViewConfigurations_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidViewConfigurations_data()
{
    QTest::addColumn<bool>("createViewConfiguration");
    QTest::addColumn<QString>("instanceName");
    QTest::addColumn<QString>("viewReference");
    QTest::addColumn<bool>("copyConfiguration");
    QTest::addColumn<bool>("isValid");

    QTest::newRow("Design configuration without view configurations is valid") <<
        false << "" << "" << false << true;
    QTest::newRow("View configuration with name and view reference is valid") <<
        true << "Bang" << "Shishigami" << false << true;

    QTest::newRow("View configuration without name is not valid") <<
        true << "" << "Shishigami" << false << false;
    QTest::newRow("View configuration without view reference is not valid") <<
        true << "Bang" << "" << false << false;
    QTest::newRow("View configurations with non-unique name is not valid") <<
        true << "Bang" << "Shishigami" << true << false;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidParameters()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidParameters()
{
    QFETCH(QString, name);
    QFETCH(QString, value);
    QFETCH(bool, copyParameter);
    QFETCH(bool, isValid);

    QSharedPointer<Parameter> testParameter (new Parameter());
    testParameter->setName(name);
    testParameter->setValue(value);

    QSharedPointer<DesignConfiguration> testConfiguration (new DesignConfiguration(
        VLNV(VLNV::DESIGNCONFIGURATION, "Samurai", "Champloo", "MugenJinFuu", "3.0")));

    testConfiguration->getParameters()->append(testParameter);

    if (copyParameter)
    {
        QSharedPointer<Parameter> otherParameter (new Parameter(*testParameter.data()));
        testConfiguration->getParameters()->append(otherParameter);
    }

    QSharedPointer<DesignConfigurationValidator> validator = createDesignConfigurationValidator(0);

    QCOMPARE(validator->hasValidParameters(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration);

        QString expectedError =
            QObject::tr("No valid name specified for parameter %1 within design configuration %2")
            .arg(name).arg(testConfiguration->getVlnv().toString());

        if (value.isEmpty())
        {
            expectedError = QObject::tr("No value specified for parameter %1 within design configuration %2")
                .arg(name).arg(testConfiguration->getVlnv().toString());
        }
        else if (copyParameter)
        {
            expectedError = QObject::tr("Parameter name %1 within design configuration %2 is not unique.")
                .arg(name).arg(testConfiguration->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidParameters_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidParameters_data()
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
// Function: tst_DesignConfigurationValidator::testHasValidAssertions()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidAssertions()
{
    QFETCH(QString, name);
    QFETCH(QString, assertValue);
    QFETCH(bool, copyAssertion);
    QFETCH(bool, isValid);

    QSharedPointer<Assertion> testAssertion (new Assertion());
    testAssertion->setName(name);
    testAssertion->setAssert(assertValue);

    QSharedPointer<DesignConfiguration> testConfiguration (
        new DesignConfiguration(VLNV(VLNV::DESIGNCONFIGURATION, "Samurai", "Champloo", "MugenJinFuu", "3.0")));
    testConfiguration->getAssertions()->append(testAssertion);

    if (copyAssertion)
    {
        QSharedPointer<Assertion> otherAssertion (new Assertion(*testAssertion.data()));
        testConfiguration->getAssertions()->append(otherAssertion);
    }

    QSharedPointer<DesignConfigurationValidator> validator = createDesignConfigurationValidator(0);

    QCOMPARE(validator->hasValidAssertions(testConfiguration), isValid);

    if (!isValid)
    {
        QVector<QString> foundErrors;
        validator->findErrorsIn(foundErrors, testConfiguration);

        QString expectedError = QObject::tr("Invalid name set for assertion %1 within design configuration %2")
            .arg(name).arg(testConfiguration->getVlnv().toString());

        QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());
        bool assertOk = true;
        int parsedAssert = parser->parseExpression(assertValue).toInt(&assertOk);

        if (!assertOk || parsedAssert != 1)
        {
            expectedError = QObject::tr("Invalid assert set for assertion %1 within design configuration %2")
                .arg(name).arg(testConfiguration->getVlnv().toString());
        }
        else if (copyAssertion)
        {
            expectedError = QObject::tr("Assertion name %1 within design configuration %2 is not unique.")
                .arg(name).arg(testConfiguration->getVlnv().toString());
        }

        if (errorIsNotFoundInErrorList(expectedError, foundErrors))
        {
            QFAIL("No error message found");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::testHasValidAssertions_data()
//-----------------------------------------------------------------------------
void tst_DesignConfigurationValidator::testHasValidAssertions_data()
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
// Function: tst_DesignConfigurationValidator::errorIsNotFoundInErrorList()
//-----------------------------------------------------------------------------
bool tst_DesignConfigurationValidator::errorIsNotFoundInErrorList(QString const& expectedError,
    QVector<QString> errorList)
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
// Function: tst_DesignConfigurationValidator::createDesignConfigurationValidator()
//-----------------------------------------------------------------------------
QSharedPointer<DesignConfigurationValidator> tst_DesignConfigurationValidator::createDesignConfigurationValidator(
    LibraryInterface* library)
{
    QSharedPointer<QList<QSharedPointer<MonitorInterconnection> > > monitorConnections
        (new QList<QSharedPointer<MonitorInterconnection> > ());
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > componentInstances;

    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());

    QSharedPointer<DesignConfigurationValidator> validator (new DesignConfigurationValidator(parser, library));

    return validator;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::createInterconnectionConfigurationValidator()
//-----------------------------------------------------------------------------
QSharedPointer<InterconnectionConfigurationValidator> tst_DesignConfigurationValidator::
    createInterconnectionConfigurationValidator(LibraryInterface* library)
{
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());

    QSharedPointer<InterconnectionConfigurationValidator> validator (
        new InterconnectionConfigurationValidator(parser, library));

    return validator;
}

//-----------------------------------------------------------------------------
// Function: tst_DesignConfigurationValidator::createViewConfigurationValidator()
//-----------------------------------------------------------------------------
QSharedPointer<ViewConfigurationValidator> tst_DesignConfigurationValidator::createViewConfigurationValidator(
    LibraryInterface* library)
{
    QSharedPointer<ExpressionParser> parser (new SystemVerilogExpressionParser());

    QSharedPointer<ViewConfigurationValidator> validator (new ViewConfigurationValidator(library, parser));

    return validator;
}

QTEST_APPLESS_MAIN(tst_DesignConfigurationValidator)

#include "tst_DesignConfigurationValidator.moc"
