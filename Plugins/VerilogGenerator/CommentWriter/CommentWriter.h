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

#include <QTextStream>

//-----------------------------------------------------------------------------
//! Class for writing C/Verilog comments.
//-----------------------------------------------------------------------------
class CommentWriter 
{
public:

    static const int DEFAULT_LINE_LENGHT = 80;

	//! The constructor.
	CommentWriter(QString const& comment);

	//! The destructor.
	~CommentWriter();

    /*!
     *  <Method description>.
     *
     *      @param [in] output   <Desrcription>.
     */
    void write(QTextStream& output) const;

    void setLineLength(int maxLength);

    void setIndent(int size);

private:

	// Disable copying.
	CommentWriter(CommentWriter const& rhs);
	CommentWriter& operator=(CommentWriter const& rhs);

    QString comment_;

    int lineLength_;

    int indentSize_;
};

#endif // COMMENTWRITER_H
