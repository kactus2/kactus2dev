//-----------------------------------------------------------------------------
// File: tst_FieldAccessPolicyWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 2.8.2023
//
// Description:
// Unit test for class FieldAccessPolicyWriter>.
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/FieldAccessPolicyWriter.h>
#include <IPXACTmodels/Component/FieldAccessPolicy.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>
#include <QDomDocument>

class tst_FieldAccessPolicyWriter : public QObject
{
    Q_OBJECT

public:
    tst_FieldAccessPolicyWriter();

private slots:

    void init();
    void cleanup();

    void writeModeRefs();
    void writeDefinitionRef();
    void writeAccess();
    void writeModifiedWriteValue();
    void writeValueConstraint();
    void writeReadAction();
    void writeReadResponse();
    void writeBroadcasts();
    void writeAccessRestrictions();
    void writeTestable();
    void writeReserved();
    void writeVendorExtensions();

private:
    QSharedPointer<FieldAccessPolicy> fieldAccessPolicy_;

};

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::tst_FieldAccessPolicyWriter()
//-----------------------------------------------------------------------------
tst_FieldAccessPolicyWriter::tst_FieldAccessPolicyWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::init()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::init()
{
    fieldAccessPolicy_ = QSharedPointer<FieldAccessPolicy>(new FieldAccessPolicy());
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::cleanup()
{
    fieldAccessPolicy_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeModeRefs()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeModeRefs()
{
    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    modeRef1->setPriority(0);
    modeRef1->setReference("testMode1");

    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    modeRef2->setPriority(1);
    modeRef2->setReference("testMode2");

    fieldAccessPolicy_->getModeReferences()->append(modeRef1);
    fieldAccessPolicy_->getModeReferences()->append(modeRef2);

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:modeRef priority=\"0\">testMode1</ipxact:modeRef>"
            "<ipxact:modeRef priority=\"1\">testMode2</ipxact:modeRef>"
        "</ipxact:fieldAccessPolicy>"
    );
    
    QString output;
    QXmlStreamWriter writer(&output);

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeDefinitionRef()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeDefinitionRef()
{
    fieldAccessPolicy_->setFieldAccessPolicyTypeDefinitionRef("testTypeDef");
    fieldAccessPolicy_->setFieldAccessPolicyDefinitionRef("testFieldAccessPolicy");

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:fieldAccessPolicyDefinitionRef typeDefinitions=\"testTypeDef\">testFieldAccessPolicy"
            "</ipxact:fieldAccessPolicyDefinitionRef>"
        "</ipxact:fieldAccessPolicy>"
    );
    
    QString output;
    QXmlStreamWriter writer(&output);

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeAccess()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeAccess()
{
    fieldAccessPolicy_->setAccess(AccessTypes::READ_WRITE);

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:access>read-write</ipxact:access>"
        "</ipxact:fieldAccessPolicy>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeModifiedWriteValue()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeModifiedWriteValue()
{
    fieldAccessPolicy_->setModifiedWrite(General::ZERO_TO_CLEAR);

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:modifiedWriteValue>zeroToClear</ipxact:modifiedWriteValue>"
        "</ipxact:fieldAccessPolicy>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);

    fieldAccessPolicy_->setModifiedWriteModify("modifier");
    output.clear();

    expectedOutput = 
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:modifiedWriteValue modify=\"modifier\">zeroToClear</ipxact:modifiedWriteValue>"
        "</ipxact:fieldAccessPolicy>"
        ;


    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeValueConstraint()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeValueConstraint()
{
    QSharedPointer<WriteValueConstraint> writeConstraint (new WriteValueConstraint());
    writeConstraint->setType(WriteValueConstraint::WRITE_AS_READ);

    fieldAccessPolicy_->setWriteValueConstraint(writeConstraint);

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:writeAsRead>true</ipxact:writeAsRead>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:fieldAccessPolicy>"
        );

    QString output;
    QXmlStreamWriter writer(&output);

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);

    output.clear();
    writeConstraint->setType(WriteValueConstraint::USE_ENUM);

    expectedOutput =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:useEnumeratedValues>true</ipxact:useEnumeratedValues>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:fieldAccessPolicy>"
        ;

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);

    output.clear();
    writeConstraint->setMinimum("0");
    writeConstraint->setMaximum("10");

    expectedOutput =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:writeValueConstraint>"
                "<ipxact:minimum>0</ipxact:minimum>"
                "<ipxact:maximum>10</ipxact:maximum>"
            "</ipxact:writeValueConstraint>"
        "</ipxact:fieldAccessPolicy>"
        ;

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeReadAction()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeReadAction()
{
    fieldAccessPolicy_->setReadAction(General::READ_SET);

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:readAction>set</ipxact:readAction>"
        "</ipxact:fieldAccessPolicy>"
        );

    QString output;
    QXmlStreamWriter writer(&output);

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);

    output.clear();

    fieldAccessPolicy_->setReadAction(General::READ_CLEAR);
    fieldAccessPolicy_->setReadActionModify("modifier");

    expectedOutput =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:readAction modify=\"modifier\">clear</ipxact:readAction>"
        "</ipxact:fieldAccessPolicy>"
        ;

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeReadResponse()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeReadResponse()
{
    fieldAccessPolicy_->setReadResponse("'b10");

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
        "<ipxact:readResponse>'b10</ipxact:readResponse>"
        "</ipxact:fieldAccessPolicy>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeBroadcasts()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeBroadcasts()
{
    QSharedPointer<FieldReference> fieldRef(new FieldReference());
    QSharedPointer<FieldReference::IndexedReference> testFieldRefElement(new FieldReference::IndexedReference);
    testFieldRefElement->reference_ = "testField";
    testFieldRefElement->indices_.append("1");
    
    fieldRef->setReference(testFieldRefElement, FieldReference::FIELD);

    fieldAccessPolicy_->getBroadcasts()->append(fieldRef);

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:broadcasts>"
                "<ipxact:broadcastTo>"
                    "<ipxact:fieldRef fieldRef=\"testField\">"
                        "<ipxact:indices>"
                            "<ipxact:index>1</ipxact:index>"
                        "</ipxact:indices>"
                    "</ipxact:fieldRef>"
                "</ipxact:broadcastTo>"
            "</ipxact:broadcasts>"
        "</ipxact:fieldAccessPolicy>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeAccessRestrictions()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeAccessRestrictions()
{
    QSharedPointer<FieldAccessPolicy::AccessRestriction> accessRestriction(
        new FieldAccessPolicy::AccessRestriction());

    QSharedPointer<ModeReference> testModeRef(new ModeReference());
    testModeRef->setPriority(0);
    testModeRef->setReference("modereference");

    accessRestriction->modeRefs_->append(testModeRef);

    fieldAccessPolicy_->getAccessRestrictions()->append(accessRestriction);

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:accessRestrictions>"
                "<ipxact:accessRestriction>"
                    "<ipxact:modeRef priority=\"0\">modereference</ipxact:modeRef>"
                "</ipxact:accessRestriction>"
            "</ipxact:accessRestrictions>"
        "</ipxact:fieldAccessPolicy>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);

    output.clear();
    accessRestriction->readAccessMask_ = "'b11";
    expectedOutput =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:accessRestrictions>"
                "<ipxact:accessRestriction>"
                    "<ipxact:modeRef priority=\"0\">modereference</ipxact:modeRef>"
                    "<ipxact:readAccessMask>'b11</ipxact:readAccessMask>"
                "</ipxact:accessRestriction>"
            "</ipxact:accessRestrictions>"
        "</ipxact:fieldAccessPolicy>"
        ;

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);

    output.clear();
    accessRestriction->writeAccessMask_ = "'b00";
    expectedOutput =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:accessRestrictions>"
                "<ipxact:accessRestriction>"
                    "<ipxact:modeRef priority=\"0\">modereference</ipxact:modeRef>"
                    "<ipxact:readAccessMask>'b11</ipxact:readAccessMask>"
                    "<ipxact:writeAccessMask>'b00</ipxact:writeAccessMask>"
                "</ipxact:accessRestriction>"
            "</ipxact:accessRestrictions>"
        "</ipxact:fieldAccessPolicy>"
        ;

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);

    output.clear();
    QSharedPointer<FieldAccessPolicy::AccessRestriction> otherAccessRestriction(
        new FieldAccessPolicy::AccessRestriction(*accessRestriction));

    otherAccessRestriction->modeRefs_->clear();
    otherAccessRestriction->readAccessMask_.clear();
    otherAccessRestriction->writeAccessMask_ = "'b11";

    fieldAccessPolicy_->getAccessRestrictions()->append(otherAccessRestriction);

    expectedOutput =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:accessRestrictions>"
                "<ipxact:accessRestriction>"
                    "<ipxact:modeRef priority=\"0\">modereference</ipxact:modeRef>"
                    "<ipxact:readAccessMask>'b11</ipxact:readAccessMask>"
                    "<ipxact:writeAccessMask>'b00</ipxact:writeAccessMask>"
                "</ipxact:accessRestriction>"
                "<ipxact:accessRestriction>"
                    "<ipxact:writeAccessMask>'b11</ipxact:writeAccessMask>"
                "</ipxact:accessRestriction>"
            "</ipxact:accessRestrictions>"
        "</ipxact:fieldAccessPolicy>"
        ;

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeTestable()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeTestable()
{
    QString output;
    QXmlStreamWriter writer(&output);

    fieldAccessPolicy_->setTestable(false);

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:testable>false</ipxact:testable>"
        "</ipxact:fieldAccessPolicy>"
        );

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);

    output.clear();

    fieldAccessPolicy_->setTestable(true);
    fieldAccessPolicy_->setTestConstraint(General::TEST_RESTORE);

    expectedOutput =
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:testable testConstraint=\"restore\">true</ipxact:testable>"
        "</ipxact:fieldAccessPolicy>"
        ;

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeReserved()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeReserved()
{
    QString output;
    QXmlStreamWriter writer(&output);

    fieldAccessPolicy_->setReserved("reserveWarning");

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:reserved>reserveWarning</ipxact:reserved>"
        "</ipxact:fieldAccessPolicy>"
        );

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_FieldAccessPolicyWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void tst_FieldAccessPolicyWriter::writeVendorExtensions()
{
    QString output;
    QXmlStreamWriter writer(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    fieldAccessPolicy_->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:fieldAccessPolicy>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:fieldAccessPolicy>"
        );

    FieldAccessPolicyWriter::writeFieldAccessPolicy(writer, fieldAccessPolicy_);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_FieldAccessPolicyWriter)

#include "tst_FieldAccessPolicyWriter.moc"
