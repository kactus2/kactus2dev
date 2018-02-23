//-----------------------------------------------------------------------------
// File: SettingsUpdater.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 15.01.2013
//
// Description:
// Settings updater which upgrades Kactus2 settings up-to-date based on a configuration file.
//-----------------------------------------------------------------------------

#include "SettingsUpdater.h"

#include <VersionHelper.h>

#include <common/widgets/assistedTextEdit/HighlightStyleDesc.h>

#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QCoreApplication>
#include <QVariant>

//-----------------------------------------------------------------------------
// Function: SettingsUpdater::runUpgrade()
//-----------------------------------------------------------------------------
void SettingsUpdater::runUpgrade(QSettings& settings, MessageMediator* mediator)
{   
    // Retrieve the version number of the settings file and check if it is not up-to-date.
    QString settingsVersion = 
        settings.value(QStringLiteral("Platform/Version"), QStringLiteral("1.5.0.0")).toString();

    if (!Details::isVersionOlderThan(settingsVersion, VersionHelper::versionFull()))
    {
        // Already up-to-date.
        return;
    }

    // Run the upgrade based on the configuration file.
    QFile* configurationFile = Details::openConfigurationFile(mediator);

    QTextStream fileStream(configurationFile);

    QString versionIterator = QString();
    while (!fileStream.atEnd())
    {
        QString line = fileStream.readLine();

        QTextStream lineStream(&line);

        // Read the command token.
        QString action;
        lineStream >> action;
        action = action.toLower();

        // Skip comments.
        if (action.startsWith(QLatin1String("//")))
        {
            continue;
        }

        // Update to next found version.
        if (action == QLatin1String("version"))
        {
            lineStream >> versionIterator;
        }
        // Otherwise check whether the current version patch needs to be applied.
        else if (Details::isVersionOlderThan(settingsVersion, versionIterator))
        {
            // Perform the upgrade operation based on the command token.
            if (action.compare(QLatin1String("add")) == 0)
            {
                QString key;
                lineStream >> key;
                lineStream.skipWhiteSpace();

                if (!settings.contains(key))
                {
                    settings.setValue(key, Details::parseVariant(lineStream));
                }
            }
            else if (action.compare(QLatin1String("remove")) == 0)
            {
                QString key;
                lineStream >> key;

                if (settings.contains(key))
                {
                    settings.remove(key);
                }
            }
            else if (action.compare(QLatin1String("rename")) == 0)
            {
                QString oldKey;
                QString newKey;
                lineStream >> oldKey >> newKey;

                QVariant value = settings.value(oldKey);
                settings.remove(oldKey);
                settings.setValue(newKey, value);
            }
            else if (action.compare(QLatin1String("copy")) == 0)
            {
                QString srcKey;
                QString destKey;
                lineStream >> srcKey >> destKey;

                QVariant value = settings.value(srcKey);
                settings.setValue(destKey, value);
            }
        }
    }

    configurationFile->close();
    delete configurationFile;
    configurationFile = 0;

    // Finally update the settings version.
    settings.setValue(QStringLiteral("Platform/Version"), VersionHelper::versionFull());
}

//-----------------------------------------------------------------------------
// Function: SettingsUpdater::Details::isVersionOlderThan()
//-----------------------------------------------------------------------------
bool SettingsUpdater::Details::isVersionOlderThan(QString const& version, QString const& comparison)
{
    QStringList numbers1 = version.split(QLatin1Char('.'));
    QStringList numbers2 = comparison.split(QLatin1Char('.'));

    for (int i = 0; i < 4; ++i)
    {
        if (numbers1.at(i).toUInt() != numbers2.at(i).toUInt())
        {
            return numbers1.at(i).toUInt() < numbers2.at(i).toUInt();
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: SettingsUpdater::Details::openConfigurationFile()
//-----------------------------------------------------------------------------
QFile* SettingsUpdater::Details::openConfigurationFile(MessageMediator* mediator)
{        
#ifdef Q_OS_WIN
    QFile* configurationFile = new QFile(QCoreApplication::applicationDirPath() + 
        QStringLiteral("/upgrade.cfg"));

#else
    QFile* configurationFile = new QFile(QStringLiteral("./upgrade.cfg"));

    if (!configurationFile->exists())
    {
        configurationFile->setFileName(QStringLiteral("/usr/share/kactus2/upgrade.cfg"));
    }
#endif

    if (!configurationFile->open(QIODevice::ReadOnly))
    {
       mediator->showFailure(QObject::tr("Settings upgrade configuration file 'upgrade.cfg' was not found. "
           "Restoring default settings."));
    }

    return configurationFile;
}

//-----------------------------------------------------------------------------
// Function: SettingsUpdater::Details::parseVariant()
//-----------------------------------------------------------------------------
QVariant SettingsUpdater::Details::parseVariant(QTextStream& lineStream)
{
    // Retrieve the variant type.
    QString type;
    lineStream >> type;
    lineStream.skipWhiteSpace();

    if (type.compare(QLatin1String("StringList")) == 0)
    {
        return lineStream.readAll().split(QLatin1Char(','));
    }
    else if (type.compare(QLatin1String("String")) == 0)
    {
        return lineStream.readAll();
    }
    else if (type.compare(QLatin1String("Size")) == 0)
    {
        int w = 0;
        int h = 0;
        lineStream >> w >> h;
        return QSize(w, h);
    }
    else if (type.compare(QLatin1String("Point")) == 0)
    {
        int x = 0;
        int y = 0;
        lineStream >> x >> y;
        return QPoint(x, y);
    }
    else if (type.compare(QLatin1String("Integer")) == 0)
    {
        int i = 0;
        lineStream >> i;
        return i;
    }
    else if (type.compare(QLatin1String("Boolean")) == 0)
    {
        QString value;
        lineStream >> value;
        return bool(value.compare(QLatin1String("true")) == 0);
    }
    else if (type.compare(QLatin1String("Font")) == 0)
    {
        QStringList arguments = lineStream.readAll().split(QLatin1Char(','));
        return QFont(arguments.first(), arguments.last().toInt());
    }
    else if (type.compare(QLatin1String("HighlightStyle")) == 0)
    {
        QStringList arguments = lineStream.readAll().split(QLatin1Char(','));
        return qVariantFromValue(HighlightStyleDesc(QColor(arguments.first()), 
            arguments.contains(QStringLiteral("bold")),
            arguments.contains(QStringLiteral("italic"))));
    }
    else if (type.compare(QLatin1String("ByteArray")) == 0)
    {
        return QVariant(lineStream.readAll().toLatin1());
    }

    Q_ASSERT(false);
    return QVariant();
}
