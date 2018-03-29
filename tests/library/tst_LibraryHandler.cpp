//-----------------------------------------------------------------------------
// File: tst_LibraryHandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: <Name>
// Date: <Date in the format dd.mm.yyyy>
//
// Description:
// Unit test for class LibraryHandler.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/PluginSystem/GeneratorPlugin/MessagePasser.h>
#include <library/LibraryHandler.h>

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
    
    void testLibraryDoesNotContainItem();

    void testLibraryContainsItem();

    void testLibraryGetModel();

    void testDocumentsInLocationAreRead();



};

tst_LibraryHandler::tst_LibraryHandler()
{

}

void tst_LibraryHandler::testLibraryDoesNotContainItem()
{
    LibraryHandler* library = createLibraryHandler();

    setupTestLibrary();

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
    LibraryHandler* library = createLibraryHandler();

    setupTestLibrary();
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
    LibraryHandler* library = createLibraryHandler();

    setupTestLibrary();
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
    LibraryHandler* library = createLibraryHandler();
    
    QVERIFY(library->getAllVLNVs().count() == 0);

    setupTestLibrary();

    QBENCHMARK
    {
        library->searchForIPXactFiles();
    }

    QVERIFY(library->getAllVLNVs().count() != 0);
}

LibraryHandler* tst_LibraryHandler::createLibraryHandler()
{
    return new LibraryHandler(0, &messageChannel_, 0);
}

void tst_LibraryHandler::setupTestLibrary()
{
    QCoreApplication::setOrganizationDomain(QStringLiteral("tut.fi"));
    QCoreApplication::setOrganizationName(QStringLiteral("TUT"));
    QCoreApplication::setApplicationName(QStringLiteral("Kactus2_tests"));

    QSettings settings("tests.ini", QSettings::IniFormat, 0);
    settings.setValue("Library/ActiveLocations", QStringList("C:/Users/pekkarie/Local/Data/kactus_test_libraries/ipxactexamplelib"));
}

QTEST_APPLESS_MAIN(tst_LibraryHandler)

#include "tst_LibraryHandler.moc"
