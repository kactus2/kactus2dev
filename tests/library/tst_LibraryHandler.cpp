//-----------------------------------------------------------------------------
// File: tst_LibraryHandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.03.2018
//
// Description:
// Unit test for class LibraryHandler.
//-----------------------------------------------------------------------------

#include <QtTest>
#include <QDebug>

#include <Plugins/PluginSystem/GeneratorPlugin/MessagePasser.h>
#include <KactusAPI/include/LibraryHandler.h>

void noMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg) {}

class tst_LibraryHandler : public QObject
{
    Q_OBJECT

public:
    tst_LibraryHandler();

private:

    LibraryHandler* createLibraryHandler();
    
    void setupTestLibrary();

    MessagePasser messageChannel_;

private slots:

    void initTestcase();

    void testLibraryDoesNotContainItem();

    void testLibraryContainsItem();

    void testLibraryGetModel();

    void testDocumentsInLocationAreRead();


};

tst_LibraryHandler::tst_LibraryHandler()
{

}

void tst_LibraryHandler::initTestcase()
{
    qInstallMessageHandler(noMessageOutput);
    LibraryHandler::initialize(&messageChannel_, this);
}

void tst_LibraryHandler::setupTestLibrary()
{
    QCoreApplication::setOrganizationDomain(QStringLiteral("tut.fi"));
    QCoreApplication::setOrganizationName(QStringLiteral("TUT"));
    QCoreApplication::setApplicationName(QStringLiteral("Kactus2_tests"));

    QSettings settings;
    settings.setPath(QSettings::IniFormat, QSettings::UserScope, "tests.ini");
    settings.setValue("Library/ActiveLocations", QStringList("D:/Data/kactus_test_libraries/"));
}

LibraryHandler* tst_LibraryHandler::createLibraryHandler()
{
    return LibraryHandler::getInstance();
}

void tst_LibraryHandler::testLibraryDoesNotContainItem()
{
    QScopedPointer<LibraryHandler> library(createLibraryHandler());

    //setupTestLibrary();

    VLNV nonExistingItem(VLNV::COMPONENT, QStringLiteral("tut.fi:testLibrary:noItem:0"));

    bool result = true;
    QBENCHMARK
    {
        result = library->contains(nonExistingItem);
    }

    QVERIFY(result == false);
}

void tst_LibraryHandler::testLibraryContainsItem()
{
    QScopedPointer<LibraryHandler> library(createLibraryHandler());

    //setupTestLibrary();
    library->searchForIPXactFiles();

    VLNV validItem(VLNV::COMPONENT, QStringLiteral("tut.fi:cpu.logic:alu:1.0"));

    bool result = false;
    QBENCHMARK
    {
        result = library->contains(validItem) ;
    }

    QVERIFY(result == true);
}

void tst_LibraryHandler::testLibraryGetModel()
{
    QScopedPointer<LibraryHandler> library(createLibraryHandler());

    //setupTestLibrary();
    library->searchForIPXactFiles();

    VLNV validItem(VLNV::COMPONENT, QStringLiteral("tut.fi:cpu.logic:alu:1.0"));

    QSharedPointer<Document> model;
    QBENCHMARK
    {
        model = library->getModel(validItem);
    }

    QVERIFY(model.isNull() == false);
}

void tst_LibraryHandler::testDocumentsInLocationAreRead()
{
    QScopedPointer<LibraryHandler> library(createLibraryHandler());

    QVERIFY(library->getAllVLNVs().count() == 0);

    setupTestLibrary();

    QBENCHMARK
    {
        library->searchForIPXactFiles();
    }

    QVERIFY(library->getAllVLNVs().count() == 1338);
}


QTEST_APPLESS_MAIN(tst_LibraryHandler)

#include "tst_LibraryHandler.moc"
