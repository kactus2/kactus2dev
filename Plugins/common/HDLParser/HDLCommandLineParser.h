//-----------------------------------------------------------------------------
// File: HDLCommandLineParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 07.05.2018
//
// Description:
// Generic command-line argument parser for HDL generators.
//-----------------------------------------------------------------------------

#ifndef HDLCOMMANDLINEPARSER_H
#define HDLCOMMANDLINEPARSER_H

#include <IPXACTmodels/common/VLNV.h>

#include <QCommandLineParser>

//-----------------------------------------------------------------------------
//! Generic command-line argument parser for HDL generators.
//-----------------------------------------------------------------------------
class HDLCommandLineParser : public QCommandLineParser
{
public:

    /*!
     *  The constructor.
     *
     *      @param [in] command   The command used to run the generator for the parser.
     *
     *      @return <Description>.
     */
    HDLCommandLineParser(QString const& command);

    //! The destructor.
    virtual ~HDLCommandLineParser();

    //! Store parser results into a single structure.
    struct ParseResults
    {
        VLNV vlnv;          //<! The parsed VLVN.
        QString viewName;   //<! The parsed view name;
        QString path;       //<! The parsed output path.

        bool cancelRun;     //<! Flag for canceling the generator run e.g. for showing help or illegal options.
        QString message;    //<! The message to show for cancel reason.
    };

    /*!
     *  Parse the given arguments.
     *
     *      @param [in] arguments   The arguments to parse.
     *
     *      @return Collection of parse results.
     */
    ParseResults parseArguments(QStringList const& arguments);

private:

    // Disable copying.
    HDLCommandLineParser(HDLCommandLineParser const& rhs);
    HDLCommandLineParser& operator=(HDLCommandLineParser const& rhs);

    // The command used to run the generator for the parser.
    QString command_;
};

#endif // HDLCOMMANDLINEPARSER_H

