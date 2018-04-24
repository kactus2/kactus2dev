//-----------------------------------------------------------------------------
// File: main.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 24.5.2011
//
// Description:
// Kactus2 main entry point.
//-----------------------------------------------------------------------------

#include "mainwindow.h"

#include "CommandLineParser.h"
#include "SplashScreen.h"

#include <common/ui/MessageMediator.h>
#include <common/ui/ConsoleMediator.h>
#include <common/ui/GraphicalMessageMediator.h>

#include <VersionHelper.h>

#include <library/LibraryHandler.h>

#include <Plugins/PluginSystem/PluginManager.h>
#include <Plugins/PluginSystem/PluginUtilityAdapter.h>

#include <settings/SettingsUpdater.h>

#include <QApplication>
#include <QPalette>
#include <QTimer>
#include <QObject>

//-----------------------------------------------------------------------------
//! Private utility functions for main.
//-----------------------------------------------------------------------------
namespace
{
    //-----------------------------------------------------------------------------
    // Function: startGui()
    //-----------------------------------------------------------------------------
    bool startGui(int argc)
    {
        return argc == 1;
    }

    //-----------------------------------------------------------------------------
    // Function: createApplication()
    //-----------------------------------------------------------------------------
    QCoreApplication* createApplication(int argc, char* argv[])
    {
        QCoreApplication* application = 0; 
        if (startGui(argc))
        {
            QApplication* guiApplication = new QApplication(argc, argv);        

            // Set the palette to use nice pastel colors.
            QPalette palette = guiApplication->palette();
            palette.setColor(QPalette::Active, QPalette::Highlight, QColor(33, 135, 237));
            palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(166, 200, 234));
            palette.setColor(QPalette::Inactive, QPalette::Highlight, QColor(166, 200, 234));
            guiApplication->setPalette(palette);

            application =  guiApplication;
        }
        else
        {
            application = new QCoreApplication(argc, argv);
        }

        // Set the identification tags for the application.
        QCoreApplication::setOrganizationDomain(QStringLiteral("tut.fi"));
        QCoreApplication::setOrganizationName(QStringLiteral("TUT"));
        QCoreApplication::setApplicationName(QStringLiteral("Kactus2"));
        QCoreApplication::setApplicationVersion(VersionHelper::createVersionString());

        return application;
    }

    //-----------------------------------------------------------------------------
    // Function: loadPlugins()
    //-----------------------------------------------------------------------------
    void loadPlugins(QSettings const& settings)
    {        
        QStringList pluginsPath = settings.value(QStringLiteral("Platform/PluginsPath"), 
            QStringList(QStringLiteral("Plugins"))).toStringList();
        PluginManager::getInstance().setPluginPaths(pluginsPath);
    }

    //-----------------------------------------------------------------------------
    // Function: createMessageMediator()
    //-----------------------------------------------------------------------------
    MessageMediator* createMessageMediator(int argc)
    {
        if (startGui(argc))
        {
            return new GraphicalMessageMediator();
        }
        else
        {
            return new ConsoleMediator();
        }
    }
};

//-----------------------------------------------------------------------------
// Function: main()
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(kactus);

    QScopedPointer<QCoreApplication> application(createApplication(argc, argv));
    QScopedPointer<MessageMediator> mediator(createMessageMediator(argc));

    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings settings;
    SettingsUpdater::runUpgrade(settings, mediator.data());

    loadPlugins(settings);

    QScopedPointer<LibraryHandler> library(new LibraryHandler(0, mediator.data(), 0));

    if (startGui(argc))
    {
        // Create the main window and close the splash after 1.5 seconds.
        MainWindow mainWindow(library.data(), mediator.data());

        // the release mode
#ifdef NDEBUG

        // Show the splash screen.
        SplashScreen splash(VersionHelper::createVersionString());
        splash.show();
        splash.showMessage("");

        application->processEvents();

        QTimer::singleShot(1500, &splash, SLOT(close()));
        QTimer::singleShot(1500, &mainWindow, SLOT(show()));
        QTimer::singleShot(1600, &mainWindow, SLOT(onLibrarySearch()));

        // the debug mode
#else

        mainWindow.show();    
        application->processEvents();
        mainWindow.onLibrarySearch();

#endif

        return application->exec();
    }

    else // Run console.
    {        
        QStringList arguments = application->arguments();
        CommandLineParser parser;

        parser.readArguments(arguments);

        if (!parser.helpOrVersionOptionSet() && parser.argumentsValid())
        {
            library->searchForIPXactFiles();
        }

        PluginUtilityAdapter utility(library.data(), mediator.data(), VersionHelper::createVersionString(), 0);
        return parser.process(arguments, &utility);
    }   
}
