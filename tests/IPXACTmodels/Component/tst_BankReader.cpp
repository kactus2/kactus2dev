//-----------------------------------------------------------------------------
// File: tst_BankReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 04.06.2026
//
// Description:
// Unit test for class BankReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/BankReader.h>
#include <IPXACTmodels/Component/Bank.h>
#include <IPXACTmodels/Component/AddressBlockReader.h>
#include <IPXACTmodels/Component/SubspaceMapReader.h>
#include <QtTest>

class tst_BankReader : public QObject
{
    Q_OBJECT

public:
    tst_BankReader();

private slots:
    void readSimpleBank();
    void readPresenceStd14();
    void readBankDefinitionRef2022();
    void readAddressBlocks();
    void readBanks();
    void readSubspaces();
};

//-----------------------------------------------------------------------------
// Function: tst_BankReader::tst_BankReader()
//-----------------------------------------------------------------------------
tst_BankReader::tst_BankReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_BankReader::readSimpleBank()
//-----------------------------------------------------------------------------
void tst_BankReader::readSimpleBank()
{
    QString documentContent(
        "<ipxact:bank bankAlignment=\"parallel\">"
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
            "<ipxact:baseAddress>'h1000'</ipxact:baseAddress>"
            "<ipxact:usage>register</ipxact:usage>"
            "<ipxact:volatile>true</ipxact:volatile>"
        "</ipxact:bank>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode bankNode = document.firstChildElement("ipxact:bank");

    QSharedPointer<Bank> testBank = BankReader::createBankFrom(bankNode, Document::Revision::Std14);

    QCOMPARE(testBank->name(), QString("testBank"));
    QCOMPARE(testBank->getAlignment(), Bank::PARALLEL);
    QCOMPARE(testBank->getBaseAddress(), QString("'h1000'"));
    QCOMPARE(testBank->getUsage(), General::REGISTER);
    QCOMPARE(testBank->getVolatile(), QString("true"));

    auto accessHandles = testBank->getAccessHandles();
    QVERIFY(accessHandles->size() == 1);
    QVERIFY(accessHandles->first()->getPathSegments()->size() == 1);
    QCOMPARE(accessHandles->first()->getPathSegments()->first()->name_, QString("segment1"));
}

//-----------------------------------------------------------------------------
// Function: tst_BankReader::readPresenceStd14()
//-----------------------------------------------------------------------------
void tst_BankReader::readPresenceStd14()
{
    QString documentContent(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:isPresent>true</ipxact:isPresent>"
        "</ipxact:bank>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode bankNode = document.firstChildElement("ipxact:bank");

    QSharedPointer<Bank> testBank = BankReader::createBankFrom(bankNode, Document::Revision::Std14);

    QCOMPARE(testBank->getIsPresent(), QString("true"));
}

//-----------------------------------------------------------------------------
// Function: tst_BankReader::readBankDefinitionRef2022()
//-----------------------------------------------------------------------------
void tst_BankReader::readBankDefinitionRef2022()
{
    QString documentContent(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:bankDefinitionRef typeDefinitions=\"testDefs\">someRef</ipxact:bankDefinitionRef>"
        "</ipxact:bank>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode bankNode = document.firstChildElement("ipxact:bank");

    QSharedPointer<Bank> testBank = BankReader::createBankFrom(bankNode, Document::Revision::Std22);

    QCOMPARE(testBank->getBankDefinitionRef(), QString("someRef"));
    QCOMPARE(testBank->getTypeDefinitionsRef(), QString("testDefs"));
}

//-----------------------------------------------------------------------------
// Function: tst_BankReader::readAddressBlocks()
//-----------------------------------------------------------------------------
void tst_BankReader::readAddressBlocks()
{
    QString documentContent(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:addressBlock>"
                "<ipxact:name>testAB</ipxact:name>"
                "<ipxact:baseAddress>'h1000</ipxact:baseAddress>"
                "<ipxact:typeIdentifier>crazy_id</ipxact:typeIdentifier>"
            "</ipxact:addressBlock>"
        "</ipxact:bank>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode bankNode = document.firstChildElement("ipxact:bank");

    QSharedPointer<Bank> testBank = BankReader::createBankFrom(bankNode, Document::Revision::Std14);

    QCOMPARE(testBank->getAddressBlocks()->size(), 1);
    QCOMPARE(testBank->getAddressBlocks()->first()->name(), QString("testAB"));
    QCOMPARE(testBank->getAddressBlocks()->first()->getBaseAddress(), QString()); // banked block doesn't have base address
    QCOMPARE(testBank->getAddressBlocks()->first()->getTypeIdentifier(), QString()); // banked block doesn't have type identifier'
}

//-----------------------------------------------------------------------------
// Function: tst_BankReader::readBanks()
//-----------------------------------------------------------------------------
void tst_BankReader::readBanks()
{
    QString documentContent(
        "<ipxact:bank>"
            "<ipxact:name>parentBank</ipxact:name>"
            "<ipxact:bank bankAlignment=\"parallel\">"
                "<ipxact:name>childBank</ipxact:name>"
                "<ipxact:baseAddress>'h1000</ipxact:baseAddress>"
                "<ipxact:access>write-only</ipxact:access>"
            "</ipxact:bank>"
        "</ipxact:bank>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode bankNode = document.firstChildElement("ipxact:bank");

    QSharedPointer<Bank> testBank = BankReader::createBankFrom(bankNode, Document::Revision::Std14);

    QCOMPARE(testBank->getBanks()->size(), 1);
    QCOMPARE(testBank->getBanks()->first()->name(), QString("childBank"));
    QCOMPARE(testBank->getBanks()->first()->getBaseAddress(), QString("")); // banked bank has no base address
    QCOMPARE(testBank->getBanks()->first()->getAlignment(), Bank::Alignment::PARALLEL);
    QCOMPARE(testBank->getBanks()->first()->getAccess(), AccessTypes::WRITE_ONLY);

}

//-----------------------------------------------------------------------------
// Function: tst_BankReader::readSubspaces()
//-----------------------------------------------------------------------------
void tst_BankReader::readSubspaces()
{
    QString documentContent(
        "<ipxact:bank>"
            "<ipxact:name>testBank</ipxact:name>"
            "<ipxact:subspaceMap masterRef=\"master\">"
                "<ipxact:name>testSSM</ipxact:name>"
                "<ipxact:baseAddress>'h1000</ipxact:baseAddress>"
                "<ipxact:parameters>"
                    "<ipxact:parameter>"
                        "<ipxact:name>param</ipxact:name>"
                    "</ipxact:parameter>"
                "</ipxact:parameters>"
            "</ipxact:subspaceMap>"
        "</ipxact:bank>"
    );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode bankNode = document.firstChildElement("ipxact:bank");

    QSharedPointer<Bank> testBank = BankReader::createBankFrom(bankNode, Document::Revision::Std14);

    QCOMPARE(testBank->getSubSpaceMaps()->size(), 1);
    QCOMPARE(testBank->getSubSpaceMaps()->first()->name(), QString("testSSM"));
    QCOMPARE(testBank->getSubSpaceMaps()->first()->getBaseAddress(), QString()); // no base address for banked subspace
    QCOMPARE(testBank->getSubSpaceMaps()->first()->getInitiatorReference(), QString("master"));
    QCOMPARE(testBank->getSubSpaceMaps()->first()->getParameters()->size(), 1);
}

QTEST_APPLESS_MAIN(tst_BankReader)

#include "tst_BankReader.moc"
