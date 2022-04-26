//-----------------------------------------------------------------------------
// File: CommandLineParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.02.2018
//
// Description:
// Utilities for parsing and executing command line arguments.
//-----------------------------------------------------------------------------

#include "CommandLineParser.h"

#include <common/ui/MessageMediator.h>
#include <KactusAPI/KactusAPI.h>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: CommandLineParser::CommandLineParser()
//-----------------------------------------------------------------------------
CommandLineParser::CommandLineParser() : optionParser_(), preReadDone_(false)
{
    optionParser_.addHelpOption();
    optionParser_.addVersionOption();

    QCommandLineOption interactiveOption({ "c", "no-gui" }, "Run in command-line mode.");

    optionParser_.addOption(interactiveOption);
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::readArguments()
//-----------------------------------------------------------------------------
void CommandLineParser::readArguments(QStringList const& arguments)
{
    optionParser_.parse(arguments);
    preReadDone_ = true;
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::commandlineMode()
//-----------------------------------------------------------------------------
bool CommandLineParser::commandlineMode() const
{
    return optionParser_.isSet(QStringLiteral("no-gui"));
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::process()
//-----------------------------------------------------------------------------
int CommandLineParser::process(QStringList const& arguments, MessageMediator* messageChannel)
{
    if (preReadDone_ == false)
    {
        optionParser_.parse(arguments);
    }

    if (optionParser_.isSet(QStringLiteral("version")))
    {
        QString versionText = KactusAPI::getVersion() +
            "Copyright (C) 2021 Tampere University\n" +
            "License GPL2: GNU GPL version 2 <https://gnu.org/licenses/gpl.html>\n";
        
        messageChannel->showMessage(versionText);
        return 0;
    }

    optionParser_.process(arguments);
    return 0;
}
