//-----------------------------------------------------------------------------
// File: SettingsUpdater.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 15.01.2013
//
// Description:
// Settings updater which upgrades Kactus2 settings up-to-date based on a configuration file.
//-----------------------------------------------------------------------------

#ifndef SETTINGSUPDATER_H
#define SETTINGSUPDATER_H

#include <KactusAPI/include/MessageMediator.h>

#include <QFile>
#include <QVariant>
#include <QTextStream>
#include <QSettings>

namespace SettingsUpdater
{
    /*!
     *  Upgrades the settings if not up-to-date.
     */
    void runUpgrade(QSettings& settings, MessageMediator* mediator);

    namespace Details
    {

        /*!
         *  Compares version numbers.
         *
         *      @param [in] version     The version to compare.
         *      @param [in] comparison  The version to compare against.
         *
         *      @return True, if the first version is older than the comparison.
         */
        bool isVersionOlderThan(QString const& version, QString const& comparison);
        
        /*!
        *  Open the configuration file for reading.
        *
        *      @param [in] parent   The parent widget.
        *
        *      @return The open configuration file.
        */
        QFile* openConfigurationFile(MessageMediator* mediator);

        /*!
         *  Parses a QVariant from the given text stream.
         *
         *      @param [in] lineStream  The source text stream.
         *
         *      @return The parsed QVariant.
         */
        QVariant parseVariant(QTextStream& lineStream);
    }
}
#endif // SETTINGSUPDATER_H
