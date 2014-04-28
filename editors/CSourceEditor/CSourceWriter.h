//-----------------------------------------------------------------------------
// File: CSourceWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 6.12.2011
//
// Description:
// Text writer for writing C source files.
//-----------------------------------------------------------------------------

#ifndef CSOURCEWRITER_H
#define CSOURCEWRITER_H

#include <common/Global.h>

#include <QTextStream>
#include <QFile>
#include <QString>

//-----------------------------------------------------------------------------
//! CSourceWriter class.
//-----------------------------------------------------------------------------
class KACTUS2_API CSourceWriter
{
public:
    /*!
     * Constructor.
     *
     *      @param [in] filename      The name of the destination file.
     *      @param [in] indentString  The indentation string.
     */
    CSourceWriter(QString const& filename, QString const& indentString);

    /*!
     *  Destructor.
     */
    ~CSourceWriter();

    /*!
     *  Opens the file for writing.
     *
     *      @return False if errors. Otherwise true.
     */
    bool open();

    /*!
     *  Increases the indentation level by one.
     */
    void increaseIndentLevel();

    /*!
     *  Decreases the indentation level by one.
     */
    void decreaseIndentLevel();

    /*!
     *  Writes a line to the source file. Newline is automatically written after the line.
     *
     *      @param [in] line The line to write.
     */
    void writeLine(QString const& line);

    /*!
     *  Writes an empty line to the source file.
     */
    void writeEmptyLine();

    /*!
     *  Writes a #include to the source file.
     *
     *      @param [in] headerName The name of the header file.
     *      @param [in] local      If true, the header is included with "" instead of <>.
     */
    void writeInclude(QString const& headerName, bool local = false);

    /*!
     *  Begins an indented code block. Increases the indentation level by one.
     */
    void beginBlock();

    /*!
     *  Ends an indented code block. Decreases the indentation level by one.
     */
    void endBlock();

    /*!
     *  Begins a structure type definition.
     */
    void beginStruct();

    /*!
     *  Ends a structure type definition.
     *
     *      @param [in] name The name of the struct.
     */
    void endStruct(QString const& name);

    /*!
     *  Writes a header block comment.
     *
     *      @param [in] desc The descriptive text.
     */
    void writeHeaderComment(QString const& desc);

private:
    // Disable copying.
    CSourceWriter(CSourceWriter const& rhs);
    CSourceWriter& operator=(CSourceWriter const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The indentation string.
    QString indentString_;

    //! The current indentation level.
    unsigned int indentLevel_;

    //! The file used as the IO device.
    QFile file_;

    //! The text stream used for writing the file.
    QTextStream stream_;
};

//-----------------------------------------------------------------------------

#endif // CSOURCEWRITER_H
