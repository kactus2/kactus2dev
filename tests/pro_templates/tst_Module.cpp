//-----------------------------------------------------------------------------
// File: tst_<Module>.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: <Name>
// Date: <Date in the format dd.mm.yyyy>
//
// Description:
// Unit test for class <Module>.
//-----------------------------------------------------------------------------

#include <QtTest>

class tst_<Module> : public QObject
{
    Q_OBJECT

public:
    tst_<Module>();

private slots:
    void testCase1();
};

tst_<Module>::tst_<Module>()
{
}

void tst_<Module>::testCase1()
{
    QVERIFY2(false, "Failing here on purpose.");
}

QTEST_APPLESS_MAIN(tst_<Module>)

#include "tst_<Module>.moc"
