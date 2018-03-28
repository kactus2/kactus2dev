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
// Function: CommandLineParser::helpOrVersionOptionSet()
//-----------------------------------------------------------------------------
bool CommandLineParser::helpOrVersionOptionSet(QStringList const& arguments)
{
    return arguments.contains("-h") || arguments.contains("--help") ||
        arguments.contains("-v") || arguments.contains("--version");
}

//-----------------------------------------------------------------------------
// Function: CommandLineParser::process()
//-----------------------------------------------------------------------------
int CommandLineParser::process(QStringList const& arguments, IPluginUtility* utility)
{
    
    QCommandLineParser optionParser;      
    optionParser.setApplicationDescription(
        "Kactus2 is a design environment for IP-XACT based SoC design.\n"
        "Running Kactus2 without any options opens the graphical user interface.");

    QCommandLineOption commandOption(QStringList() << QStringLiteral("r") << QStringLiteral("run"), 
        QObject::tr("Runs the given command."), QObject::tr("command"));
    optionParser.addOption(commandOption);
    
    QCommandLineOption helpOption = optionParser.addHelpOption();
    QCommandLineOption versionOption = optionParser.addVersionOption();

    optionParser.parse(arguments);

    if (optionParser.isSet(helpOption))
    {
        utility->printInfo(optionParser.helpText());
        return 0;
    }

    if (optionParser.isSet(versionOption))
    {
        utility->printInfo(QStringLiteral("Kactus2 ") + utility->getKactusVersion());
        return 0;
    }

    QString command = optionParser.value(commandOption);       

    PluginManager& pluginManager = PluginManager::getInstance();
    foreach (IPlugin* plugin, pluginManager.getAllPlugins())
    {        
        CommandLineSupport* support = dynamic_cast<CommandLineSupport*>(plugin);
        if (support && support->getCommand().compare(command) == 0)
        {
            support->process(arguments, utility);
            return 0;
        }
    }
     

    //optionParser.process(arguments);
    return 0;
}
