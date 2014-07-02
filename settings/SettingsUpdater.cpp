//-----------------------------------------------------------------------------
// File: SettingsUpdater.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 15.01.2013
//
// Description:
// Settings updater which upgrades Kactus2 settings up-to-date based on a
// configuration file.
//-----------------------------------------------------------------------------

#include "SettingsUpdater.h"

#include <version.h>
#include <common/widgets/assistedTextEdit/HighlightStyleDesc.h>

#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QFile>
#include <QSettings>
#include <QMessageBox>
#include <QCoreApplication>
#include <QVariant>

#define STRINGIFY(str) TOSTRING(str)
#define TOSTRING(str) #str

//-----------------------------------------------------------------------------

namespace SettingsUpdater
{
    namespace
    {
        /*!
         *  Compares version numbers.
         *
         *      @param [in] lhs The first version to compare.
         *      @param [in] rhs The second version to compare.
         *
         *      @return True, if the first version is older than the second one.
         */
        bool isVersionOlder(QString const& lhs, QString const& rhs)
        {
            QStringList numbers1 = lhs.split('.');
            QStringList numbers2 = rhs.split('.');

            for (int i = 0; i < 4; ++i)
            {
                if (numbers1[i].toUInt() != numbers2[i].toUInt())
                {
                    return numbers1[i].toUInt() < numbers2[i].toUInt();
                }
            }

            return false;
        }

        /*!
         *  Parses a QVariant from the given text stream.
         *
         *      @param [in] stream The source text stream.
         *
         *      @return The parsed QVariant.
         */
        QVariant parseVariant(QTextStream& stream)
        {
            // Retrieve the variant type.
            QString type;
            stream >> type;
            stream.skipWhiteSpace();

            if (type == "StringList")
            {
                QStringList list = stream.readAll().split(',');
                return list;
            }
            else if (type == "String")
            {
                QString str;
                str = stream.readAll();
                return str;
            }
            else if (type == "Size")
            {
                int w = 0;
                int h = 0;
                stream >> w >> h;
                return QSize(w, h);
            }
            else if (type == "Point")
            {
                int x = 0;
                int y = 0;
                stream >> x >> y;
                return QPoint(x, y);
            }
            else if (type == "Integer")
            {
                int i = 0;
                stream >> i;
                return i;
            }
            else if (type == "Boolean")
            {
                QString value;
                stream >> value;
                return bool(value == "true");
            }
            else if (type == "Font")
            {
                QString data = stream.readAll();
                QStringList arguments = data.split(',');
                return QFont(arguments[0], arguments[1].toInt());
            }
            else if (type == "HighlightStyle")
            {
                QString data = stream.readAll();
                QStringList arguments = data.split(',');
                return qVariantFromValue(HighlightStyleDesc(QColor(arguments[0]), arguments.contains("bold"),
                                                            arguments.contains("italic")));
            }
            else if (type == "ByteArray")
            {
                QString bytes;
                bytes = stream.readAll();
                return QVariant(bytes.toLatin1());
            }

            Q_ASSERT(false);
            return QVariant();
        }
    }

    //-----------------------------------------------------------------------------
    // Function: runUpgrade()
    //-----------------------------------------------------------------------------
    void runUpgrade(QWidget* parent)
    {
        QSettings settings;

        // Retrieve the version number of the settings file and check if it is not up-to-date.
        QString settingsVersion = settings.value("Platform/Version", "1.5.0.0").toString();
        
        if (!isVersionOlder(settingsVersion, STRINGIFY(VERSION_FULL)))
        {
            // Already up-to-date.
            return;
        }

        // Run the upgrade from the configuration file.
#ifdef Q_OS_WIN
        QFile file(QCoreApplication::applicationDirPath() + QString("/upgrade.cfg"));
#else
        QFile file("/usr/share/kactus2/upgrade.cfg");
#endif

        if (!file.open(QIODevice::ReadOnly))
        {
            QMessageBox msgBox(QMessageBox::Critical, QCoreApplication::applicationName(),
                               QObject::tr("Settings upgrade configuration file was not found. Please reinstall Kactus2."),
                               QMessageBox::Ok, parent);

            msgBox.exec();
            return;
        }

        // Run the upgrade based on the configuration file.
        QTextStream stream(&file);
        QString curVersion = "";

        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            QTextStream lineStream(&line);

            // Read the command token.
            QString token;
            lineStream >> token;
            token = token.toLower();

            // Skip comments.
            if (token.startsWith("//"))
            {
                continue;
            }

            // Take notice of version changes.
            if (token == "version")
            {
                lineStream >> curVersion;
            }
            // Otherwise check whether the current version patch needs to be applied.
            else if (isVersionOlder(settingsVersion, curVersion))
            {
                // Perform the upgrade operation based on the command token.
                if (token == "add")
                {
                    QString key;
                    lineStream >> key;
                    lineStream.skipWhiteSpace();

                    if (!settings.contains(key))
                    {
                        settings.setValue(key, parseVariant(lineStream));
                    }
                }
                else if (token == "remove")
                {
                    QString key;
                    lineStream >> key;

                    if (settings.contains(key))
                    {
                        settings.remove(key);
                    }
                }
                else if (token == "rename")
                {
                    QString oldKey;
                    QString newKey;
                    lineStream >> oldKey >> newKey;
                    
                    QVariant value = settings.value(oldKey);
                    settings.remove(oldKey);
                    settings.setValue(newKey, value);                    
                }
                else if (token == "copy")
                {
                    QString srcKey;
                    QString destKey;
                    lineStream >> srcKey >> destKey;

                    QVariant value = settings.value(srcKey);
                    settings.setValue(destKey, value);
                }
            }
        }

        // Finally update the settings version.
        settings.setValue("Platform/Version", QString(STRINGIFY(VERSION_FULL)));
    }
}
