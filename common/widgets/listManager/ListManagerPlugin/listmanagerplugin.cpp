#include "../listmanager.h"
#include "listmanagerplugin.h"

#include <QtCore/QtPlugin>

ListManagerPlugin::ListManagerPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void ListManagerPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool ListManagerPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *ListManagerPlugin::createWidget(QWidget *parent)
{
    return new ListManager(QString("List title"), parent);
}

QString ListManagerPlugin::name() const
{
    return QLatin1String("ListManager");
}

QString ListManagerPlugin::group() const
{
    return QLatin1String("IP-Xact widgets");
}

QIcon ListManagerPlugin::icon() const
{
    return QIcon();
}

QString ListManagerPlugin::toolTip() const
{
    return QLatin1String("Manage a string list");
}

QString ListManagerPlugin::whatsThis() const
{
    return QLatin1String("This widget manages a list of strings");
}

bool ListManagerPlugin::isContainer() const
{
    return false;
}

QString ListManagerPlugin::domXml() const
{
    return QLatin1String("<widget class=\"ListManager\" name=\"listManager\">\n</widget>\n");
}

QString ListManagerPlugin::includeFile() const
{
    return QLatin1String("listmanager.h");
}

Q_EXPORT_PLUGIN2(listmanagerplugin, ListManagerPlugin)
