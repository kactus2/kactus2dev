//-----------------------------------------------------------------------------
// File: DocumentGeneratorHTML.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 26.10.2017
//
// Description:
// Common abbreviations for document generation in HTML.
//-----------------------------------------------------------------------------

#ifndef DOCUMENTGENERATORHTML
#define DOCUMENTGENERATORHTML

//-----------------------------------------------------------------------------
//! Common abbreviations for document generation in HTML.
//-----------------------------------------------------------------------------
namespace DocumentGeneratorHTML
{
    static QString space()
    {
        static const QString SPACE = QString("&nbsp;");
        return SPACE;
    }

    static QString indent()
    {
        static const QString INDENT = QString("&nbsp;&nbsp;&nbsp;");
        return INDENT;
    }

    static QString table()
    {
        static const QString TABLE = QString("<table frame=\"box\" rules=\"all\" border=\"1\" cellPadding=\"3\" "
            "title=\"");
        return TABLE;
    }

    static QString docType()
    {
        static const QString DOCTYPE = QString("<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01//EN\" "
            "\"http://www.w3.org/TR/html4/strict.dtd\">");
        return DOCTYPE;
    }

    static QString encoding()
    {
        static const QString ENCODING = QString("<meta http-equiv=\"Content-Type\" content=\"text/html; "
            "charset=utf-8\">");
        return ENCODING;
    }
         
    static QString valid3wStrict()
    {
        static const QString VALID_W3C_STRICT = QString("\t\t<p>\n"
            "\t\t\t<a href=\"https://validator.w3.org/#validate_by_upload\">\n"
            "\t\t\t<img src=\"http://www.w3.org/Icons/valid-html401\""
            "alt=\"Valid HTML 4.01 Strict\" height=\"31\"""width=\"88\">\n"
            "\t\t\t</a>\n"
            "\t\t</p>");
        return VALID_W3C_STRICT;
    }
}

#endif // DOCUMENTGENERATORHTML