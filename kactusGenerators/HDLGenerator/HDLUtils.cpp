//-----------------------------------------------------------------------------
// File: HDLUtils.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.7.2014
//
// Description:
// Utilities for all HDL generation.
//-----------------------------------------------------------------------------

#include "HDLUtils.h"

#include <QString>
#include <QStringList>

void HDLUtils::writeDescription(QString const& description, QTextStream& stream, 
                                QString const& commentTag, QString const& indentation /*= QString("")*/) {

	if (description.isEmpty()) {
		stream << endl;
		return;
	}

	// split the description into lines whenever there is a line separator
	QStringList lines = description.split(QString("\n"));
	
	// at least one line has to be because description was not empty
	Q_ASSERT(!lines.isEmpty());

	// Split long lines.  Simple routine, does not account 
	// indentation and the line's last word can go over 80 chars
	for (int i = 0; i < lines.size(); ++i) {
		QStringList words = lines.at(i).split(" ");	
		int line_len = 0; // num of chars
		for (int w = 0; w < words.size(); ++w) {			
			if (line_len == 0) {
				stream << indentation << commentTag;
			}
			stream << " " << words.at(w);
			line_len += words.at(w).size() +1; //+1 = white space	

			if (line_len > 70) {
				stream << endl;
				line_len = 0;
			}
		}			
		stream << endl;
	}
}

