//-----------------------------------------------------------------------------
// File: tst_AccessPolicyWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 8.8.2023
//
// Description:
// Unit test for class AccessPolicyWriter>.    
//-----------------------------------------------------------------------------

#include <IPXACTmodels/Component/AccessPolicyWriter.h>
#include <IPXACTmodels/Component/AccessPolicy.h>
#include <IPXACTmodels/common/GenericVendorExtension.h>

#include <QtTest>
#include <QDomDocument>

class tst_AccessPolicyWriter : public QObject
{
    Q_OBJECT

public:
    tst_AccessPolicyWriter();

private slots:

    void init();
    void cleanup();

    void writeModeRefs();
    void writeAccess();
    void writeVendorExtensions();

private:
    QSharedPointer<AccessPolicy> accessPolicy_;

};

//-----------------------------------------------------------------------------
// Function: tst_AccessPolicyWriter::tst_AccessPolicyWriter()
//-----------------------------------------------------------------------------
tst_AccessPolicyWriter::tst_AccessPolicyWriter()
{

}

//-----------------------------------------------------------------------------
// Function: tst_AccessPolicyWriter::init()
//-----------------------------------------------------------------------------
void tst_AccessPolicyWriter::init()
{
    accessPolicy_ = QSharedPointer<AccessPolicy>(new AccessPolicy());
}

//-----------------------------------------------------------------------------
// Function: tst_AccessPolicyWriter::cleanup()
//-----------------------------------------------------------------------------
void tst_AccessPolicyWriter::cleanup()
{
    accessPolicy_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_AccessPolicyWriter::writeModeRefs()
//-----------------------------------------------------------------------------
void tst_AccessPolicyWriter::writeModeRefs()
{
    QSharedPointer<ModeReference> modeRef1(new ModeReference());
    modeRef1->setPriority("0");
    modeRef1->setReference("testMode1");

    QSharedPointer<ModeReference> modeRef2(new ModeReference());
    modeRef2->setPriority("1");
    modeRef2->setReference("testMode2");

    accessPolicy_->getModeReferences()->append(modeRef1);
    accessPolicy_->getModeReferences()->append(modeRef2);

    QString expectedOutput(
        "<ipxact:accessPolicy>"
            "<ipxact:modeRef priority=\"0\">testMode1</ipxact:modeRef>"
            "<ipxact:modeRef priority=\"1\">testMode2</ipxact:modeRef>"
        "</ipxact:accessPolicy>"
    );
    
    QString output;
    QXmlStreamWriter writer(&output);

    AccessPolicyWriter::writeAccessPolicy(writer, accessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AccessPolicyWriter::writeAccess()
//-----------------------------------------------------------------------------
void tst_AccessPolicyWriter::writeAccess()
{
    accessPolicy_->setAccess(AccessTypes::READ_WRITE);

    QString expectedOutput(
        "<ipxact:accessPolicy>"
            "<ipxact:access>read-write</ipxact:access>"
        "</ipxact:accessPolicy>"
    );

    QString output;
    QXmlStreamWriter writer(&output);

    AccessPolicyWriter::writeAccessPolicy(writer, accessPolicy_);
    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_AccessPolicyWriter::writeVendorExtensions()
//-----------------------------------------------------------------------------
void tst_AccessPolicyWriter::writeVendorExtensions()
{
    QString output;
    QXmlStreamWriter writer(&output);

    QDomDocument document;
    QDomElement extensionNode = document.createElement("testExtension");
    extensionNode.setAttribute("testExtensionAttribute", "extension");
    extensionNode.appendChild(document.createTextNode("testValue"));

    QSharedPointer<GenericVendorExtension> testExtension(new GenericVendorExtension(extensionNode));

    accessPolicy_->getVendorExtensions()->append(testExtension);

    QString expectedOutput(
        "<ipxact:accessPolicy>"
            "<ipxact:vendorExtensions>"
                "<testExtension testExtensionAttribute=\"extension\">testValue</testExtension>"
            "</ipxact:vendorExtensions>"
        "</ipxact:accessPolicy>"
        );

    AccessPolicyWriter::writeAccessPolicy(writer, accessPolicy_);
    QCOMPARE(output, expectedOutput);
}

QTEST_APPLESS_MAIN(tst_AccessPolicyWriter)

#include "tst_AccessPolicyWriter.moc"
