#include "../libraryrefgenerator.h"
#include "libraryrefgeneratorplugin.h"

#include <QtCore/QtPlugin>

LibraryRefGeneratorPlugin::LibraryRefGeneratorPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void LibraryRefGeneratorPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool LibraryRefGeneratorPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *LibraryRefGeneratorPlugin::createWidget(QWidget *parent)
{
    return new LibraryRefGenerator(QString("Title of widget"), parent, true);
}

QString LibraryRefGeneratorPlugin::name() const
{
    return QLatin1String("LibraryRefGenerator");
}

QString LibraryRefGeneratorPlugin::group() const
{
    return QLatin1String("IP-Xact widgets");
}

QIcon LibraryRefGeneratorPlugin::icon() const
{
    return QIcon();
}

QString LibraryRefGeneratorPlugin::toolTip() const
{
    return QLatin1String("Creates a reference to IPXact-document");
}

QString LibraryRefGeneratorPlugin::whatsThis() const
{
    return QLatin1String("This widget can be used to generate reference from IP-Xact document element to another IP-Xact document using VLNV.");
}

bool LibraryRefGeneratorPlugin::isContainer() const
{
    return false;
}

QString LibraryRefGeneratorPlugin::domXml() const
{
    return QLatin1String("<widget class=\"LibraryRefGenerator\" name=\"libraryRefGenerator\">\n</widget>\n");
}

QString LibraryRefGeneratorPlugin::includeFile() const
{
    return QLatin1String("libraryrefgenerator.h");
}

Q_EXPORT_PLUGIN2(libraryrefgeneratorplugin, LibraryRefGeneratorPlugin)
