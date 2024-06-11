//-----------------------------------------------------------------------------
// File: tst_CppSourceAnalyzer.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Anton Hagqvist
// Date: 11.06.2024
//
// Description:
// Unit test for CppSourceAnalyzer.
//-----------------------------------------------------------------------------

#include <Plugins/CppSourceAnalyzer/CppSourceAnalyzer.h>
#include <IPXACTmodels/Component/Component.h>

#include <QtTest>
#include <QList>
#include <QFile>

class tst_CppSourceAnalyzer : public QObject
{
    Q_OBJECT

public:
    tst_CppSourceAnalyzer();


private slots:
    void cleanup();

    void testGetDependencies();

    void testRemoveComments();

private:

    CppSourceAnalyzer analyzer;

};

tst_CppSourceAnalyzer::tst_CppSourceAnalyzer()
{
}

//-----------------------------------------------------------------------------
// Function: tst_CppSourceAnalyzer::cleanup()
//-----------------------------------------------------------------------------
void tst_CppSourceAnalyzer::cleanup()
{
    QFile sampleFile("sample.c");
    if (sampleFile.exists())
    {
        sampleFile.remove();
    }
}

//-----------------------------------------------------------------------------
// Function: tst_CppSourceAnalyzer::testCase1()
//-----------------------------------------------------------------------------
void tst_CppSourceAnalyzer::testGetDependencies()
{
    QString source =
        "#include <stdio.h>\n"
        "\n"
        "#include \"file2.h\"\n"
        "\n"
        "int main() {\n"
        "// this is a comment\n"
        "/* this is a\n"
        "multiline comment*/\n"
        "    return 0;\n"
        "}";
    
    QFile sampleFile("sample.c");
    sampleFile.open(QFile::ReadWrite);
    QTextStream stream(&sampleFile);
    stream << source;
    sampleFile.close();

    auto deps = analyzer.getFileDependencies(nullptr, "", "sample.c");

    QVERIFY(deps.size() == 2);
    QVERIFY(deps.first().filename == "stdio.h");
    QVERIFY(deps.last().filename == "file2.h");
}

//-----------------------------------------------------------------------------
// Function: tst_CppSourceAnalyzer::testRemoveComments()
//-----------------------------------------------------------------------------
void tst_CppSourceAnalyzer::testRemoveComments()
{
    QString source =
        "#include <stdio.h>\n"
        "\n"
        "#include \"file2.h\"\n"
        "\n"
        "int main() {\n"
        "// this is a comment\n"
        "/* this is a\n"
        "multiline comment*/\n"
        "    printf(\"this is a string\");\n"
        "    return 0;\n"
        "}";

    QString target = 
        "#include <stdio.h>\n"
        "#include \"file2.h\"\n"
        "\n\n"
        "int main() {\n"
        "\n\n"
        "printf();\n"
        "return 0;\n"
        "}\n";

    QFile sampleFile("sample.c");
    sampleFile.open(QFile::ReadWrite);
    QTextStream stream(&sampleFile);
    stream << source;
    sampleFile.close();

    QFile newFile("sample.c");
    newFile.open(QIODevice::ReadOnly | QIODevice::Text);

    QString parsedSource = analyzer.getSourceData(newFile);
    QCOMPARE(parsedSource, target);
}

QTEST_APPLESS_MAIN(tst_CppSourceAnalyzer)

#include "tst_CppSourceAnalyzer.moc"
