//-----------------------------------------------------------------------------
// File: CommentWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 08.08.2014
//
// Description:
// Class for writing C/Verilog comments.
//-----------------------------------------------------------------------------

#ifndef COMMENTWRITER_H
#define COMMENTWRITER_H

#include "../common/Writer.h"

#include <QTextStream>

//-----------------------------------------------------------------------------
//! Class for writing C/Verilog comments.
//-----------------------------------------------------------------------------
class CommentWriter : public Writer
{
public:
    //! Default number of characters per comment line.
    static const int DEFAULT_LINE_LENGHT = 80;

	//! The constructor.
	CommentWriter(QString const& comment);

	//! The destructor.
	~CommentWriter();

    /*!
     *  Writes the comment to the given output.
     *
     *      @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const;

    /*!
     *  Sets the maximum number of characters per line.
     *
     *      @param [in] maxLength   The maximum number of characters to set.
     */
    void setLineLength(int maxLength);

    /*!
     *  Sets the indent for each line.
     *
     *      @param [in] size   The number of spaces used for indent.
     */
    void setIndent(int size);

private:

	// Disable copying.
	CommentWriter(CommentWriter const& rhs);
	CommentWriter& operator=(CommentWriter const& rhs);
    
    /*!
     *  Checks if the writer should write nothing.
     *
     *      @return True, if the writer has nothing to write, otherwise false.
     */
    bool nothingToWrite() const;
    
    //! The comment to write.
    QString comment_;

    //! The maximum length of each line.
    int lineLength_;

    //! The number of indenting spaces.
    int indentSize_;
};

#endif // COMMENTWRITER_H
