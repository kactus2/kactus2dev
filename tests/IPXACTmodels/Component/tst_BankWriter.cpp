//-----------------------------------------------------------------------------
// File: tst_BankWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 05.06.2026
//
// Description:
// Unit test for BankWriter.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/BankWriter.h>
#include <IPXACTmodels/Component/Bank.h>
#include <IPXACTmodels/Component/MemoryBlockBase.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>

class tst_BankWriter : public QObject
{
    Q_OBJECT

public:
    tst_BankWriter();

private slots:

    void initTestCase();
    void cleanupTestCase();
    void init();
    void cleanup();

    void writeSimpleBank();
    void writeAccessHandles2014();
    void writeAccessHandles2022();
    void writeBankDefRef2022();
    void writeAddressBlocks();
    void writeBanks();
    void writeSubspaces();
    void writeAccess2014();
    void writeAccessPolicies2022();
    void writeParameters();
    void writeVendorExtensions();

private:

    QSharedPointer<Bank> testBank_;
};

//-----------------------------------------------------------------------------
// Function: tst_BankWriter::tst_BankWriter()
//-----------------------------------------------------------------------------
tst_BankWriter::tst_BankWriter():
testBank_()
{

}

//-----------------------------------------------------------------------------
// Function: tst_BankWriter::initTestCase()
//-----------------------------------------------------------------------------
void tst_BankWriter::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_BankWriter::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_BankWriter::cleanupTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_BankWriter::init()
//-----------------------------------------------------------------------------
void tst_BankWriter::init()
{
    testBank_ = QSharedPointer<Bank>(new Bank("testBank"));
}

//-----------------------------------------------------------------------------
// Function: tst_BankWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_BankWriter::cleanup()
{
    testBank_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_BankWriter::writeSimpleBank()
//-----------------------------------------------------------------------------
void tst_BankWriter::writeSimpleBank()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testBank_->setDisplayName("bank");
    testBank_->setDescription("a bank");
    testBank_->setBaseAddress("'h1000");
    testBank_->setUsage(General::Usage::REGISTER);
    testBank_->setVolatile(true);
    testBank_->setAlignment(Bank::Alignment::PARALLEL);
    testBank_->setIsPresent("true");

    QString expectedOutput(
        "<ipxact:bank bankAlignment=\"parallel\">"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:displayName>bank</ipxact:displayName>"
            "<ipxact:description>a bank</ipxact:description>"
            "<ipxact:baseAddress>'h1000</ipxact:baseAddress>"
            "<ipxact:isPresent>true</ipxact:isPresent>"
            "<ipxact:usage>register</ipxact:usage>"
            "<ipxact:volatile>true</ipxact:volatile>"
        "</ipxact:bank>"
    );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

void tst_BankWriter::writeAccessHandles2014()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QString testIndex("0");
    QSharedPointer<PathSegment> ps(new PathSegment());
    ps->indices_.append(testIndex);
    ps->name_ = QStringLiteral("segment1");

    QSharedPointer<AccessHandle> ah(new AccessHandle());
    ah->getPathSegments()->append(ps);

    testBank_->getAccessHandles()->append(ah);

    QString expectedOutput(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:accessHandles>"
                "<ipxact:accessHandle>"
                    "<ipxact:pathSegments>"
                        "<ipxact:pathSegment>"
                            "<ipxact:pathSegmentName>segment1</ipxact:pathSegmentName>"
                            "<ipxact:indices>"
                               "<ipxact:index>0</ipxact:index>"
                            "</ipxact:indices>"
                        "</ipxact:pathSegment>"
                    "</ipxact:pathSegments>"
                "</ipxact:accessHandle>"
            "</ipxact:accessHandles>"
        "</ipxact:bank>"
    );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

void tst_BankWriter::writeAccessHandles2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<PathSegment> ps(new PathSegment());
    ps->name_ = QStringLiteral("segment1");

    QSharedPointer<AccessHandle> ah(new AccessHandle());
    ah->getPathSegments()->append(ps);

    testBank_->getAccessHandles()->append(ah);

    QString expectedOutput(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:accessHandles>"
                "<ipxact:accessHandle>"
                    "<ipxact:pathSegments>"
                        "<ipxact:pathSegment>segment1</ipxact:pathSegment>"
                    "</ipxact:pathSegments>"
                "</ipxact:accessHandle>"
            "</ipxact:accessHandles>"
        "</ipxact:bank>"
    );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

void tst_BankWriter::writeBankDefRef2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    testBank_->setBankDefinitionRef("another_bank");
    testBank_->setTypeDefinitionsRef("typedefinitions");

    QString expectedOutput(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:bankDefinitionRef typeDefinitions=\"typedefinitions\">another_bank</ipxact:bankDefinitionRef>"
        "</ipxact:bank>"
    );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

void tst_BankWriter::writeAddressBlocks()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<AddressBlock> ab(new AddressBlock("testAB"));
    ab->setRange("4");
    ab->setWidth("32");

    testBank_->getAddressBlocks()->append(ab);

    QString expectedOutput(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:addressBlock>"
                "<ipxact:name>testAB</ipxact:name>"
                "<ipxact:range>4</ipxact:range>"
                "<ipxact:width>32</ipxact:width>"
            "</ipxact:addressBlock>"
        "</ipxact:bank>"
    );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();
    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

void tst_BankWriter::writeBanks()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<Bank> bank(new Bank("nestedBank"));

    testBank_->getBanks()->append(bank);

    QString expectedOutput(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:bank>"
                "<ipxact:name>nestedBank</ipxact:name>"
            "</ipxact:bank>"
        "</ipxact:bank>"
    );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    output.clear();
    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

void tst_BankWriter::writeSubspaces()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<SubSpaceMap> ssm(new SubSpaceMap("bankedSubspace"));
    ssm->setInitiatorReference("a_reference");

    testBank_->getSubSpaceMaps()->append(ssm);

    QString expectedOutput(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:subspaceMap masterRef=\"a_reference\">"
                "<ipxact:name>bankedSubspace</ipxact:name>"
            "</ipxact:subspaceMap>"
        "</ipxact:bank>"
    );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);

    expectedOutput =
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:subspaceMap initiatorRef=\"a_reference\">"
                "<ipxact:name>bankedSubspace</ipxact:name>"
            "</ipxact:subspaceMap>"
        "</ipxact:bank>";

    output.clear();
    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

void tst_BankWriter::writeAccess2014()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    testBank_->setAccess(AccessTypes::READ_ONLY);

    QString expectedOutput(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:access>read-only</ipxact:access>"
        "</ipxact:bank>"
    );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

void tst_BankWriter::writeAccessPolicies2022()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QSharedPointer<AccessPolicy> ap(new AccessPolicy());
    QSharedPointer<ModeReference> modeRef(new ModeReference());
    modeRef->setReference("a_mode");

    ap->getModeReferences()->append(modeRef);
    ap->setAccess(AccessTypes::READ_ONLY);

    testBank_->getAccessPolicies()->append(ap);

    QString expectedOutput(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:accessPolicies>"
                "<ipxact:accessPolicy>"
                    "<ipxact:modeRef priority=\"0\">a_mode</ipxact:modeRef>"
                    "<ipxact:access>read-only</ipxact:access>"
                "</ipxact:accessPolicy>"
            "</ipxact:accessPolicies>"
        "</ipxact:bank>"
    );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

void tst_BankWriter::writeParameters()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);
    QSharedPointer<Parameter> param(new Parameter());
    param->setName("param");
    param->setValue("32");
    param->setValueId("testID");


    testBank_->getParameters()->append(param);

    QString expectedOutput(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:parameters>"
                "<ipxact:parameter parameterId=\"testID\">"
                    "<ipxact:name>param</ipxact:name>"
                    "<ipxact:value>32</ipxact:value>"
                "</ipxact:parameter>"
            "</ipxact:parameters>"
        "</ipxact:bank>"
    );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std22);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_BankWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void tst_BankWriter::writeVendorExtensions()
{
    QString output;
    QXmlStreamWriter xmlStreamWriter(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    testBank_->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:bank>"
        );

    BankWriter::writeBank(xmlStreamWriter, testBank_, Document::Revision::Std14);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_BankWriter)

#include "tst_BankWriter.moc"
