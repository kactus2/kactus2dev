//-----------------------------------------------------------------------------
// File: tst_FieldReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 31.7.2023
//
// Description:
// Unit test for class FieldAccessPolicyReader.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/FieldAccessPolicyReader.h>

#include <IPXACTmodels/Component/FieldAccessPolicy.h>

#include <IPXACTmodels/common/VendorExtension.h>

#include <QtTest>

class tst_FieldAccessPolicyReader : public QObject
{
    Q_OBJECT

public:
    tst_FieldAccessPolicyReader();

private slots:

    void readModeRefs();
    void readDefinitionRef();
    void readAccess();
    void readModifiedWrite();
    void readWriteConstraint();
    void readReadAction();
    void readResponse();
    void readBroadcasts();
    void readAccessRestrictions();
    void readTestable();
    void readReserved();
    void readVendorExtensions();
};

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::tst_FieldAccessPolicyReader()
//-----------------------------------------------------------------------------
tst_FieldAccessPolicyReader::tst_FieldAccessPolicyReader()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readModeRefs()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readModeRefs()
{
    QString doc(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:modeRef priority=\"0\">normal</ipxact:modeRef>"
        "</ipxact:fieldAccessPolicy>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getModeReferences()->size(), 1);
    QCOMPARE(fieldAccessPolicy->getModeReferences()->first()->getReference(), QString("normal"));

     doc =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:modeRef priority=\"0\">normal</ipxact:modeRef>"
            "<ipxact:modeRef priority=\"1\">test</ipxact:modeRef>"
        "</ipxact:fieldAccessPolicy>"
        ;

    document.setContent(doc);

    fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getModeReferences()->size(), 2);
    QCOMPARE(fieldAccessPolicy->getModeReferences()->first()->getReference(), QString("normal"));
    QCOMPARE(fieldAccessPolicy->getModeReferences()->first()->getPriority(), 0);
    QCOMPARE(fieldAccessPolicy->getModeReferences()->at(1)->getReference(), QString("test"));
    QCOMPARE(fieldAccessPolicy->getModeReferences()->at(1)->getPriority(), 1);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readDefinitionRef()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readDefinitionRef()
{
    QString doc(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:fieldAccessPolicyDefinitionRef typeDefinitions=\"testTypeDefinitions\">testDefinition</ipxact:fieldAccessPolicyDefinitionRef>"
        "</ipxact:fieldAccessPolicy>"
    );

    QDomDocument document;
    document.setContent(doc);

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getFieldAccessPolicyDefinitionRef(), QString("testDefinition"));
    QCOMPARE(fieldAccessPolicy->getFieldAccessPolicyTypeDefinitionRef(), QString("testTypeDefinitions"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readAccess()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readAccess()
{
    QString documentContent(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:access>read-write</ipxact:access>"
        "</ipxact:fieldAccessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getAccess(), AccessTypes::Access::READ_WRITE);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readModifiedWrite()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readModifiedWrite()
{
     QString documentContent(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:modifiedWriteValue>zeroToClear</ipxact:modifiedWriteValue>"
        "</ipxact:fieldAccessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getModifiedWrite(), General::ZERO_TO_CLEAR);

    documentContent =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:modifiedWriteValue modify=\"modifier\">oneToToggle</ipxact:modifiedWriteValue>"
        "</ipxact:fieldAccessPolicy>"
        ;

    document.setContent(documentContent);
    fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getModifiedWrite(), General::ONE_TO_TOGGLE);
    QCOMPARE(fieldAccessPolicy->getModifiedWriteModify(), QString("modifier"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readWriteConstraint()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readWriteConstraint()
{
    QString documentContent(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:writeAsRead>true</ipxact:writeAsRead>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:fieldAccessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getWriteValueConstraint().isNull(), false);
    QCOMPARE(fieldAccessPolicy->getWriteValueConstraint()->getType(), WriteValueConstraint::WRITE_AS_READ);

    documentContent =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:useEnumeratedValues>true</ipxact:useEnumeratedValues>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:fieldAccessPolicy>"
        ;

    document.setContent(documentContent);
    
    fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getWriteValueConstraint().isNull(), false);
    QCOMPARE(fieldAccessPolicy->getWriteValueConstraint()->getType(), WriteValueConstraint::USE_ENUM);

    documentContent =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:minimum>0</ipxact:minimum>"
                "<ipxact:maximum>10</ipxact:maximum>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:fieldAccessPolicy>"
        ;

    document.setContent(documentContent);
    
    fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getWriteValueConstraint().isNull(), false);
    QCOMPARE(fieldAccessPolicy->getWriteValueConstraint()->getType(), WriteValueConstraint::MIN_MAX);
    QCOMPARE(fieldAccessPolicy->getWriteValueConstraint()->getMinimum(), QString("0"));
    QCOMPARE(fieldAccessPolicy->getWriteValueConstraint()->getMaximum(), QString("10"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readReadAction()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readReadAction()
{
    QString documentContent(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:readAction>set</ipxact:readAction>"
        "</ipxact:fieldAccessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getReadAction(), General::READ_SET);

    documentContent =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:readAction modify=\"modifier\">clear</ipxact:readAction>"
        "</ipxact:fieldAccessPolicy>"
        ;

    document.setContent(documentContent);

    fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getReadAction(), General::READ_CLEAR);
    QCOMPARE(fieldAccessPolicy->getReadActionModify(), QString("modifier"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readResponse()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readResponse()
{
    QString documentContent(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:readResponse>8+8</ipxact:readResponse>"
        "</ipxact:fieldAccessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getReadResponse(), QString("8+8"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readBroadcasts()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readBroadcasts()
{
    QString documentContent(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:broadcasts>"
                "<ipxact:broadcastTo>"
                    "<ipxact:fieldRef fieldRef=\"testField\">"
                        "<ipxact:indices>"
                            "<ipxact:index>0</ipxact:index>"
                        "</ipxact:indices>"
                    "</ipxact:fieldRef>"
                "</ipxact:broadcastTo>"
                "<ipxact:broadcastTo>"
                    "<ipxact:memoryMapRef memoryMapRef=\"testMemoryMap\"/>"
                    "<ipxact:addressBlockRef addressBlockRef=\"testAddressBlock\"/>"
                    "<ipxact:registerRef registerRef=\"testRegister\"/>"
                    "<ipxact:fieldRef fieldRef=\"testField\"/>"
                "</ipxact:broadcastTo>"
            "</ipxact:broadcasts>"
        "</ipxact:fieldAccessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getBroadcasts()->size(), 2);
    QCOMPARE(fieldAccessPolicy->getBroadcasts()->first()->getReference(FieldReference::FIELD)->reference_, QString("testField"));
    QCOMPARE(fieldAccessPolicy->getBroadcasts()->first()->getReference(FieldReference::FIELD)->indices_.size(), 1);
    QCOMPARE(fieldAccessPolicy->getBroadcasts()->first()->getReference(FieldReference::FIELD)->indices_.first(), QString("0"));

    QCOMPARE(fieldAccessPolicy->getBroadcasts()->at(1)->getReference(FieldReference::MEMORY_MAP)->reference_, QString("testMemoryMap"));
    QCOMPARE(fieldAccessPolicy->getBroadcasts()->at(1)->getReference(FieldReference::ADDRESS_BLOCK)->reference_, QString("testAddressBlock"));
    QCOMPARE(fieldAccessPolicy->getBroadcasts()->at(1)->getReference(FieldReference::REGISTER)->reference_, QString("testRegister"));
    QCOMPARE(fieldAccessPolicy->getBroadcasts()->at(1)->getReference(FieldReference::FIELD)->reference_, QString("testField"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readAccessRestrictions()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readAccessRestrictions()
{
    QString documentContent(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:accessRestrictions>"
                "<ipxact:accessRestriction>"
                    "<ipxact:modeRef priority=\"0\">testMode</ipxact:modeRef>"
                    "<ipxact:modeRef priority=\"1\">testMode2</ipxact:modeRef>"
                    "<ipxact:readAccessMask>'b00</ipxact:readAccessMask>"
                    "<ipxact:writeAccessMask>'b11</ipxact:writeAccessMask>"
                "</ipxact:accessRestriction>"
                "<ipxact:accessRestriction>"
                    "<ipxact:readAccessMask>'b1000</ipxact:readAccessMask>"
                    "<ipxact:writeAccessMask>'b1111</ipxact:writeAccessMask>"
                "</ipxact:accessRestriction>"
            "</ipxact:accessRestrictions>"
        "</ipxact:fieldAccessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    auto accessRestrictions = fieldAccessPolicy->getAccessRestrictions();

    QCOMPARE(accessRestrictions->size(), 2);
    QCOMPARE(accessRestrictions->first()->modeRefs_->size(), 2);
    QCOMPARE(accessRestrictions->first()->readAccessMask_, QString("'b00"));
    QCOMPARE(accessRestrictions->first()->writeAccessMask_, QString("'b11"));

    QCOMPARE(accessRestrictions->at(1)->modeRefs_->size(), 0);
    QCOMPARE(accessRestrictions->at(1)->readAccessMask_, QString("'b1000"));
    QCOMPARE(accessRestrictions->at(1)->writeAccessMask_, QString("'b1111"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readTestable()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readTestable()
{
    QString documentContent(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:testable>false</ipxact:testable>"
        "</ipxact:fieldAccessPolicy>"
    );

    QDomDocument document;
    document.setContent(documentContent);
    
    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getTestable().toString(), QString("false"));

    documentContent =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:testable testConstraint=\"restore\">true</ipxact:testable>"
        "</ipxact:fieldAccessPolicy>"
        ;

    document.setContent(documentContent);
    fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getTestable().toString(), QString("true"));
    QCOMPARE(fieldAccessPolicy->getTestConstraint(), General::TEST_RESTORE);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readReserved()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readReserved()
{
    QString documentContent(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:reserved>reserveWarning</ipxact:reserved>"
        "</ipxact:fieldAccessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getReserved(), QString("reserveWarning"));
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyReader::readVendorExtensions()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyReader::readVendorExtensions()
{
    QString documentContent(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:fieldAccessPolicy>"
        );

    QDomDocument document;
    document.setContent(documentContent);

    QDomNode fieldNode = document.firstChildElement("ipxact:field");

    auto fieldAccessPolicy = FieldAccessPolicyReader::createFieldAccessPolicyFrom(document.firstChildElement("ipxact:fieldAccessPolicy"));

    QCOMPARE(fieldAccessPolicy->getVendorExtensions()->size(), 1);
    QCOMPARE(fieldAccessPolicy->getVendorExtensions()->first()->type(), QString("testExtension"));
}

QTEST_APPLESS_MAIN(tst_FieldAccessPolicyReader)

#include "tst_FieldAccessPolicyReader.moc"
