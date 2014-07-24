#include <QtTest>

class tst_<Module> : public QObject
{
    Q_OBJECT

public:
    tst_<Module>();

private Q_SLOTS:
    void testCase1();
};

tst_<Module>::tst_<Module>()
{
}

void tst_<Module>::testCase1()
{
    QVERIFY2(true, "Failure");
}

QTEST_APPLESS_MAIN(tst_<Module>)

#include "tst_<Module>.moc"
