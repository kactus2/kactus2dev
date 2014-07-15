//-----------------------------------------------------------------------------
// File: HDLUtils.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.7.2014
//
// Description:
// Utilities for all HDL generation.
//-----------------------------------------------------------------------------

#ifndef HDLUTILS_H
#define HDLUTILS_H

#include <QString>
#include <QTextStream>

namespace HDLUtils {

	/*! Write a description of an element to the stream.
	 * 
	 *      @param [in] description The description to write.
	 *      @param [in] stream      The text stream to write into.
     *      @param [in] commentTag  The tag for a single-line comment to use.
	 *      @param [in] indentation Added to the beginning of each new line, e.g."  " or  "\t"
	 */
	void writeDescription(QString const& description, QTextStream& stream, 
        QString const& commentTag, QString const& indentation = QString(""));

}

#endif // HDLUTILS_H