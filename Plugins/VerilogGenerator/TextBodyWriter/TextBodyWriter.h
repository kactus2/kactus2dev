//-----------------------------------------------------------------------------
// File: TextBodyWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 09.06.2016
//
// Description:
// Class for writing text body.
//-----------------------------------------------------------------------------

#ifndef TEXTBODYWRITER_H
#define TEXTBODYWRITER_H

#include "../common/Writer.h"
#include "../veriloggeneratorplugin_global.h"

#include <QTextStream>

class VERILOGGENERATORPLUGIN_EXPORT TextBodyWriter : public Writer
{
public:
	//! The constructor.
	TextBodyWriter(QString const& comment);

	//! The destructor.
	virtual ~TextBodyWriter();

    /*!
     *  Writes the comment to the given output.
     *
     *      @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const;

private:

	// Disable copying.
	TextBodyWriter(TextBodyWriter const& rhs);
	TextBodyWriter& operator=(TextBodyWriter const& rhs);
    
    //! The text to write.
    QString text_;
};

#endif // TEXTBODYWRITER_H
