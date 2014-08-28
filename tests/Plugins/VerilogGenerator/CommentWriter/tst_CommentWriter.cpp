//-----------------------------------------------------------------------------
// File: tst_CommentWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 08.08.2014
//
// Description:
// Unit test for class CommentWriter.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>

class tst_CommentWriter : public QObject
{
    Q_OBJECT

public:
    tst_CommentWriter();

private slots:
    void testWritesNothing();
    void testWritesOneLineComment();
    void testWritesMultilineComment();
    void testSplitsLines();
    void testSplitsLines_data();
    void testEmptyLines();
};

tst_CommentWriter::tst_CommentWriter()
{
}

//-----------------------------------------------------------------------------
// Function: tst_CommentWriter::testWritesNothing()
//-----------------------------------------------------------------------------
void tst_CommentWriter::testWritesNothing()
{
    CommentWriter writer("");

    QString output;
    QTextStream outputStream(&output);

    writer.write(outputStream);

    QVERIFY(output.isEmpty());
}

//-----------------------------------------------------------------------------
// Function: tst_CommentWriter::testWritesOneLineComment()
//-----------------------------------------------------------------------------
void tst_CommentWriter::testWritesOneLineComment()
{
    CommentWriter writer("One line comment.");

    QString output;
    QTextStream outputStream(&output);

    writer.write(outputStream);

    QCOMPARE(output, QString("// One line comment.\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CommentWriter::testWritesMultilineComment()
//-----------------------------------------------------------------------------
void tst_CommentWriter::testWritesMultilineComment()
{
    CommentWriter writer("Multi\nline\ncomment.");

    QString output;
    QTextStream outputStream(&output);

    writer.write(outputStream);

    QCOMPARE(output, QString("// Multi\n"
        "// line\n"
        "// comment.\n"));
}

//-----------------------------------------------------------------------------
// Function: tst_CommentWriter::testSplitsLines()
//-----------------------------------------------------------------------------
void tst_CommentWriter::testSplitsLines()
{
    QFETCH(int, lineLength);
    QFETCH(QString, comment);
    QFETCH(QString, expectedOutput);

    CommentWriter writer(comment);
    writer.setLineLength(lineLength);

    QString output;
    QTextStream outputStream(&output);

    writer.write(outputStream);

    QCOMPARE(output, expectedOutput);
}

//-----------------------------------------------------------------------------
// Function: tst_CommentWriter::testSplitsLines_data()
//-----------------------------------------------------------------------------
void tst_CommentWriter::testSplitsLines_data()
{
    QTest::addColumn<int>("lineLength");
    QTest::addColumn<QString>("comment");
    QTest::addColumn<QString>("expectedOutput");

    QTest::newRow("Short comment on one line") << 100 << "A comment." << "// A comment.\n";
    QTest::newRow("Two lines") << 5 << "Split here." << "// Split\n// here.\n";
    QTest::newRow("Word on a single line") << 1 << "Unsplittable." << "// Unsplittable.\n";
    QTest::newRow("Every word on own line") << 1 << "One two three four." << "// One\n// two\n// three\n// four.\n";
    QTest::newRow("Two words per line") << 10 << "First two, other two." << "// First two,\n// other two.\n";
}

//-----------------------------------------------------------------------------
// Function: tst_CommentWriter::testEmptyLines()
//-----------------------------------------------------------------------------
void tst_CommentWriter::testEmptyLines()
{
    CommentWriter writer("First paragraph.\n\nSecond paragraph.");

    QString output;
    QTextStream outputStream(&output);

    writer.write(outputStream);

    QCOMPARE(output, QString("// First paragraph.\n"
        "// \n"
        "// Second paragraph.\n"));
}

QTEST_APPLESS_MAIN(tst_CommentWriter)

#include "tst_CommentWriter.moc"
