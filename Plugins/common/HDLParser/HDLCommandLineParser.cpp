//-----------------------------------------------------------------------------
// File: HDLCommandLineParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.05.2018
//
// Description:
// Generic command-line argument parser for HDL generators.
//-----------------------------------------------------------------------------

#include "HDLCommandLineParser.h"

#include <QStringList>

//-----------------------------------------------------------------------------
// Function: HDLCommandLineParser::HDLCommandLineParser()
//-----------------------------------------------------------------------------
HDLCommandLineParser::HDLCommandLineParser(QString const& command): command_(command)
{
    addHelpOption();

    QCommandLineOption targetComponent(QStringList() << QStringLiteral("c") << QStringLiteral("top-component"), 
        QStringLiteral("The top-component VLNV separated by semicolons (:)."), QStringLiteral("VLVN"));

    QCommandLineOption targetView(QStringList() << QStringLiteral("w") << QStringLiteral("target-view"), 
        QStringLiteral("The component view to use in generation."), QStringLiteral("view name"));

    QCommandLineOption outputDirectory(QStringList() << QStringLiteral("o") << QStringLiteral("output"), 
        QStringLiteral("The output directory for generation."), QStringLiteral("path"), 
        QStringLiteral("generated_vhdl"));

    addOption(targetComponent);
    addOption(targetView);
    addOption(outputDirectory);
}

//-----------------------------------------------------------------------------
// Function: HDLCommandLineParser::~HDLCommandLineParser()
//-----------------------------------------------------------------------------
HDLCommandLineParser::~HDLCommandLineParser()
{

}

//-----------------------------------------------------------------------------
// Function: HDLCommandLineParser::parseArguments()
//-----------------------------------------------------------------------------
HDLCommandLineParser::ParseResults HDLCommandLineParser::parseArguments(QStringList const& arguments)
{
    bool validArguments = parse(arguments);

    ParseResults result;
    result.vlnv = VLNV(VLNV::COMPONENT, value("c"));
    result.viewName = value("w");
    result.path = value("o");
    result.cancelRun = !validArguments;

    if (isSet(QStringLiteral("help")))
    {
        result.message = helpText();
        result.message.replace(0, result.message.indexOf(QLatin1Char('[')), 
            QString("Usage: Kactus2 %1 ").arg(command_));

        result.cancelRun = true;
    }

    return result;
}
