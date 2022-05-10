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

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "mainwindow.h"

#include "CommandLineParser.h"
#include "SplashScreen.h"

#include <KactusAPI/include/MessageMediator.h>
#include <KactusAPI/include/ConsoleMediator.h>
#include <common/ui/GraphicalMessageMediator.h>

#include <KactusAPI/KactusAPI.h>

#include <KactusAPI/include/VersionHelper.h>

#include <library/LibraryHandler.h>

#include <KactusAPI/include/PluginManager.h>
#include <KactusAPI/include/PluginUtilityAdapter.h>

#include <settings/SettingsUpdater.h>

#include <QApplication>
#include <QPalette>
#include <QTimer>
#include <QObject>


#include <iostream>

#include "PythonAPI/PythonInterpreter.h"
#include "PythonAPI/StdInputListener.h"
#include "PythonAPI/FileChannel.h"

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
    QCoreApplication* createApplication(int &argc, char* argv[])
    {
        QCoreApplication* application = nullptr; 
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

    LibraryHandler::initialize(0, mediator.data(), 0);
    QScopedPointer<LibraryHandler> library(LibraryHandler::getInstance());

    QScopedPointer<KactusAPI> coreAPI(new KactusAPI(library.data(), mediator.data()));

    wchar_t *program = Py_DecodeLocale(argv[0], NULL);
    if (program == NULL)
    {
       // errorChannel_->write(QString("Fatal error: cannot decode application name."));
        return false;
    }

    Py_SetProgramName(program);
    PyMem_RawFree(program);

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

        parser.process(arguments, mediator.data());
        
        if (parser.commandlineMode())
        {
            library->searchForIPXactFiles();

            QScopedPointer<FileChannel> outChannel(new FileChannel(stdout));
            QScopedPointer<FileChannel> errChannel(new FileChannel(stderr));

            bool interactive = isatty(fileno(stdin));
            PythonInterpreter console(outChannel.data(), errChannel.data(), interactive, application.data());

            if (console.initialize(interactive) == false)
            {
                return 1;
            }

            QScopedPointer<StdInputListener> listener(new StdInputListener(&console, application.data()));

            QObject::connect(listener.data(), SIGNAL(inputFailure()), application.data(), SLOT(quit()));

            return application->exec();
        }

        return 1;
    }   
}
