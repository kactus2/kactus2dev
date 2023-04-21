//-----------------------------------------------------------------------------
// File: tst_CommandLineParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.03.2018
//
// Description:
// Unit test for class CommandLineParser.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <tests/MockObjects/PluginMock.h>

#include <KactusAPI/KactusAPI.h>
#include <KactusAPI/include/PluginManager.h>
#include <Plugins/PluginSystem/GeneratorPlugin/MessagePasser.h>

#include <mainwindow/CommandLineParser.h>

class tst_CommandLineParser : public QObject
{
    Q_OBJECT

public:
    tst_CommandLineParser();

public slots:

    void writeToLog(QString const& msg);

private:
    MessagePasser mediator_;
    PluginMock* testPlugin_;
    QStringList log_;

private slots:
    void initTestCase();
    void cleanup();
    void cleanupTestCase();

    void testShowVersion();
    void testShowVersion_data();

    void testRunPluginCommand();
    void testPluginHelp();

    void testShowUsage();
    void testShowUsage_data();

};

tst_CommandLineParser::tst_CommandLineParser(): QObject(0),
    mediator_(),
    testPlugin_(new PluginMock()),
    log_()
{
    connect(&mediator_, SIGNAL(noticeMessage(QString const&)), this, SLOT(writeToLog(QString const&)));
}

//-----------------------------------------------------------------------------
// Function: tst_CommandLineParser::writeToLog()
//-----------------------------------------------------------------------------
void tst_CommandLineParser::writeToLog(QString const& msg)
{
    log_.append(msg);
}

//-----------------------------------------------------------------------------
// Function: tst_CommandLineParser::initTestCase()
//-----------------------------------------------------------------------------
void tst_CommandLineParser::initTestCase()
{
    QSettings testSettings("tst_CommandLineParser.ini");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    PluginManager& plugins = PluginManager::getInstance();
    plugins.addPlugin(testPlugin_);
}

//-----------------------------------------------------------------------------
// Function: tst_CommandLineParser::cleanup()
//-----------------------------------------------------------------------------
void tst_CommandLineParser::cleanup()
{
    log_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_CommandLineParser::cleanupTestCase()
//-----------------------------------------------------------------------------
void tst_CommandLineParser::cleanupTestCase()
{
    delete testPlugin_;
}


//-----------------------------------------------------------------------------
// Function: tst_CommandLineParser::testShowVersion()
//-----------------------------------------------------------------------------
void tst_CommandLineParser::testShowVersion()
{
    QFETCH(QString, input);

    QStringList arguments = input.split(QLatin1Char(' '));
    CommandLineParser parser;
    parser.process(arguments, &mediator_);

    QString expected = KactusAPI::getVersion() + QStringLiteral(" Copyright (C) 2023 Tampere University\n"
        "License GPL2: GNU GPL version 2 <https://gnu.org/licenses/gpl.html>\n");

    QCOMPARE(log_.count(), 1);
    QCOMPARE(log_.first(), expected);
}

//-----------------------------------------------------------------------------
// Function: tst_CommandLineParser::testShowVersion_data()
//-----------------------------------------------------------------------------
void tst_CommandLineParser::testShowVersion_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("-v option") << "tst_CommandLineParser -v";
    QTest::newRow("--version option") << "tst_CommandLineParser --version";
}

//-----------------------------------------------------------------------------
// Function: tst_CommandLineParser::testRunPluginCommand()
//-----------------------------------------------------------------------------
void tst_CommandLineParser::testRunPluginCommand()
{
    QString input("testApp mock");

    CommandLineParser parser; 
    parser.process(input.split(QLatin1Char(' ')), &mediator_);

    QFile settingsFile("tst_CommandLineParser.ini");
    if (settingsFile.exists())
    {
        settingsFile.remove();
    }

    QVERIFY(settingsFile.exists() == false);
    QCOMPARE(log_.count(), 1);
    QCOMPARE(log_.first(), QStringLiteral("mock"));
}

//-----------------------------------------------------------------------------
// Function: tst_CommandLineParser::testPluginHelp()
//-----------------------------------------------------------------------------
void tst_CommandLineParser::testPluginHelp()
{
    QString input("testApp mock -h");

    CommandLineParser parser; 
    parser.process(input.split(QLatin1Char(' ')), &mediator_);

    QFile settingsFile("tst_CommandLineParser.ini");
    if (settingsFile.exists())
    {
        settingsFile.remove();
    }

    QVERIFY(settingsFile.exists() == false);
    QCOMPARE(log_.count(), 1);
    QCOMPARE(log_.first(), QStringLiteral("mock -h"));
}

//-----------------------------------------------------------------------------
// Function: tst_CommandLineParser::testShowUsage()
//-----------------------------------------------------------------------------
void tst_CommandLineParser::testShowUsage()
{
    QFETCH(QString, input);

    QStringList arguments = input.split(QLatin1Char(' '));
    CommandLineParser parser;
    parser.process(arguments, &mediator_);

    QString expected(
        "Usage: Kactus2 <command> [options]\n"
        "Kactus2 is a design environment for IP-XACT based SoC design.\n"
        "Running Kactus2 without any options opens the graphical user interface.\n"
        "\n"
        "Arguments:\n"
        "  command         The command to run.\n"
        "\n"
        "The available commands are:\n"
        "  mock\n"
        "\n"
        "The command may require additional options. See 'Kactus2 <command> -h'\n"
        "for command-specific information.\n"
        "\n"
        "Options:\n"
        "  -h, --help  Displays this help.\n"
        "  -v, --version   Displays version information.\n"
    );

    QCOMPARE(log_.count(), 1);
    QCOMPARE(log_.first(), expected);
}

//-----------------------------------------------------------------------------
// Function: tst_CommandLineParser::testShowUsage_data()
//-----------------------------------------------------------------------------
void tst_CommandLineParser::testShowUsage_data()
{
    QTest::addColumn<QString>("input");

    QTest::newRow("-h option") << "tst_CommandLineParser -h";
    QTest::newRow("--help option") << "tst_CommandLineParser --help";
}
QTEST_MAIN(tst_CommandLineParser)

#include "tst_CommandLineParser.moc"
