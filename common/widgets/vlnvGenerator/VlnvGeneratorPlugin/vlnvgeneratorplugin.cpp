#include "../vlnvgenerator.h"
#include "vlnvgeneratorplugin.h"

#include <QtCore/QtPlugin>

VLNVGeneratorPlugin::VLNVGeneratorPlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void VLNVGeneratorPlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool VLNVGeneratorPlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *VLNVGeneratorPlugin::createWidget(QWidget *parent)
{
    return new VLNVGenerator(parent);
}

QString VLNVGeneratorPlugin::name() const
{
    return QLatin1String("VLNVGenerator");
}

QString VLNVGeneratorPlugin::group() const
{
    return QLatin1String("IP-Xact widgets");
}

QIcon VLNVGeneratorPlugin::icon() const
{
    return QIcon();
}

QString VLNVGeneratorPlugin::toolTip() const
{
    return QLatin1String("Create/edit VLNV element");
}

QString VLNVGeneratorPlugin::whatsThis() const
{
    return QLatin1String("This widget can be used to create/edit a VLNV.");
}

bool VLNVGeneratorPlugin::isContainer() const
{
    return false;
}

QString VLNVGeneratorPlugin::domXml() const
{
    return QLatin1String("<widget class=\"VLNVGenerator\" name=\"vLNVGenerator\">\n</widget>\n");
}

QString VLNVGeneratorPlugin::includeFile() const
{
    return QLatin1String("vlnvgenerator.h");
}

Q_EXPORT_PLUGIN2(vlnvgeneratorplugin, VLNVGeneratorPlugin)
