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

#include <KactusAPI/include/MessageMediator.h>
#include <KactusAPI/KactusAPI.h>

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QStringList>

const QHash<CommandLineParser::Option, CommandLineParser::OptionData > CommandLineParser::CLI_OPTS = {
    {
        CommandLineParser::Option::NoGui, 
        CommandLineParser::OptionData{QChar('c'), QStringLiteral("no-gui"), 
            QStringLiteral("Run in interactive command-line mode.")}
    },
    {
        CommandLineParser::Option::InputScript, 
        CommandLineParser::OptionData{QChar('i'), QStringLiteral("input-script"),
            QStringLiteral("Run script then exit."), QStringLiteral("script path")}
    },
};

//-----------------------------------------------------------------------------
// Function: CommandLineParser::CommandLineParser()
//-----------------------------------------------------------------------------
CommandLineParser::CommandLineParser() : optionParser_(), preReadDone_(false)
{
    optionParser_.addHelpOption();
    optionParser_.addVersionOption();

    for (auto const& option : CLI_OPTS.values())
    {
        QCommandLineOption newOpt({ option.character, option.shortName }, option.description);
        
        if (option.valueName.isEmpty() == false)
        {
            newOpt.setValueName(option.valueName);
        }

        optionParser_.addOption(newOpt);
    }
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
    return optionIsSet(Option::NoGui);
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::runScriptMode()
//-----------------------------------------------------------------------------
bool CommandLineParser::runScriptMode() const
{
    return optionIsSet(Option::InputScript);
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::versionMode()
//-----------------------------------------------------------------------------
bool CommandLineParser::versionMode() const
{
    return optionParser_.isSet(QStringLiteral("version"));
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

    if (versionMode())
    {
        QString versionText = KactusAPI::getVersion() +
            " Copyright (C) 2025 Tampere University\n" +
            "License GPL2: GNU GPL version 2 <https://gnu.org/licenses/gpl.html>\n";
        
        messageChannel->showMessage(versionText);
        return 0;
    }

    if (optionIsSet(Option::InputScript))
    {
        auto scriptPath = getOptionValue(Option::InputScript);

        if (scriptPath.isEmpty())
        {
            messageChannel->showFailure("Error: No script path given");
            return 1;
        }
        else if (QFile::exists(scriptPath) == false)
        {
            messageChannel->showFailure("Error: Invalid script path");
            return 1;
        }
    }

    optionParser_.process(arguments);
    return 0;
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::getOptionParser()
//-----------------------------------------------------------------------------
QString CommandLineParser::getOptionValue(CommandLineParser::Option option) const
{
    return optionParser_.value(CLI_OPTS[option].shortName);
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::optionIsSet()
//-----------------------------------------------------------------------------
bool CommandLineParser::optionIsSet(Option option) const
{
    return optionParser_.isSet(CLI_OPTS[option].shortName);
}
