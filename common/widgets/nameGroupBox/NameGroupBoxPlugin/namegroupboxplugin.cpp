#include "../namegroupbox.h"
#include "namegroupboxplugin.h"

#include <QtCore/QtPlugin>

NameGroupBoxPlugin::NameGroupBoxPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void NameGroupBoxPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool NameGroupBoxPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *NameGroupBoxPlugin::createWidget(QWidget *parent)
{
    return new NameGroupBox(parent);
}

QString NameGroupBoxPlugin::name() const
{
    return QLatin1String("NameGroupBox");
}

QString NameGroupBoxPlugin::group() const
{
    return QLatin1String("IP-Xact widgets");
}

QIcon NameGroupBoxPlugin::icon() const
{
    return QIcon();
}

QString NameGroupBoxPlugin::toolTip() const
{
    return QLatin1String("Element to edit spirit:nameGroup element");
}

QString NameGroupBoxPlugin::whatsThis() const
{
    return QLatin1String("This widget provides means to edit/create spirit:nameGroup element in IP-Xact");
}

bool NameGroupBoxPlugin::isContainer() const
{
    return false;
}

QString NameGroupBoxPlugin::domXml() const
{
    return QLatin1String("<widget class=\"NameGroupBox\" name=\"nameGroupBox\">\n</widget>\n");
}

QString NameGroupBoxPlugin::includeFile() const
{
    return QLatin1String("namegroupbox.h");
}

Q_EXPORT_PLUGIN2(namegroupboxplugin, NameGroupBoxPlugin)
