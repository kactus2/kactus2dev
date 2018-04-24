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

#include <QStringList>

#include <QCoreApplication>
#include <QCommandLineParser>

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/PluginManager.h>
#include <Plugins/PluginSystem/CommandLineSupport.h>
#include <Plugins/PluginSystem/IPluginUtility.h>

#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>

//-----------------------------------------------------------------------------
// Function: CommandLineParser::CommandLineParser()
//-----------------------------------------------------------------------------
CommandLineParser::CommandLineParser(): optionParser_(), preReadDone_(false)
{
    optionParser_.addHelpOption();
    optionParser_.addVersionOption();
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::~CommandLineParser()
//-----------------------------------------------------------------------------
CommandLineParser::~CommandLineParser()
{

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
// Function: CommandLineParser::argumentsValid()
//-----------------------------------------------------------------------------
bool CommandLineParser::argumentsValid() const
{
    return optionParser_.errorText().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::helpOrVersionOptionSet()
//-----------------------------------------------------------------------------
bool CommandLineParser::helpOrVersionOptionSet() const
{
    return optionParser_.isSet(QStringLiteral("help")) || optionParser_.isSet(QStringLiteral("version"));
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::process()
//-----------------------------------------------------------------------------
int CommandLineParser::process(QStringList const& arguments, IPluginUtility* utility)
{   
    if (preReadDone_ == false)
    {
        optionParser_.parse(arguments);
    }

    if (optionParser_.positionalArguments().isEmpty() == false)
    {
        QString command = optionParser_.positionalArguments().first();       

        PluginManager& pluginManager = PluginManager::getInstance();
        foreach (IPlugin* plugin, pluginManager.getAllPlugins())
        {
            CommandLineSupport* support = dynamic_cast<CommandLineSupport*>(plugin);
            if (support && support->getCommand().compare(command) == 0)
            {
                QStringList pluginArguments = arguments;
                pluginArguments.pop_front();

                support->process(pluginArguments, utility);
                return 0;
            }
        }
    }
     
    if (optionParser_.isSet(QStringLiteral("help")))
    {
        utility->printInfo(helpText());
        return 0;
    }

    if (optionParser_.isSet(QStringLiteral("version")))
    {
        utility->printInfo(QStringLiteral("Kactus2 ") + utility->getKactusVersion());
        return 0;
    }

    optionParser_.process(arguments);
    return 0;
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::helpText()
//-----------------------------------------------------------------------------
QString CommandLineParser::helpText()
{
    QString text("Usage: Kactus2 <command> [options]\n" 
        "Kactus2 is a design environment for IP-XACT based SoC design.\n"
        "Running Kactus2 without any options opens the graphical user interface.\n"
        "\n"
        "Arguments:\n"
        "  command         The command to run.\n"
        "\n"
        "The available commands are:\n");

    PluginManager& pluginManager = PluginManager::getInstance();
    foreach (IPlugin* plugin, pluginManager.getAllPlugins())
    {
        CommandLineSupport* support = dynamic_cast<CommandLineSupport*>(plugin);
        if (support)
        {
            text.append(QString("  %1\n").arg(support->getCommand()));
        }
    }

    text.append(     
        "\n"
        "The command may require additional options. See 'Kactus2 <command> -h'\n"
        "for command-specific information.\n"
        "\n"
        "Options:\n"
        "  -?, -h, --help  Displays this help.\n"
        "  -v, --version   Displays version information.\n");

    return text;
}
