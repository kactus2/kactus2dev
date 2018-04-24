//-----------------------------------------------------------------------------
// File: PluginMock.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.04.2018
//
// Description:
// <Short description of the class/file contents>
//-----------------------------------------------------------------------------

#ifndef PLUGINMOCK_H
#define PLUGINMOCK_H

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/CommandLineSupport.h>

class PluginMock : public IPlugin, public CommandLineSupport
{
public:

    //! The constructor.
    PluginMock();

    //! The destructor.
    virtual ~PluginMock();

    virtual QString getName() const;

    virtual QString getVersion() const;

    virtual QString getDescription() const;

    virtual QString getVendor() const;

    virtual QString getLicence() const;

    virtual QString getLicenceHolder() const;

    virtual QWidget* getSettingsWidget();

    virtual PluginSettingsModel* getSettingsModel();

    virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

    virtual QString getCommand() const;

    virtual void process(QStringList const& arguments, IPluginUtility* utility);

private:

    // Disable copying.
    PluginMock(PluginMock const& rhs);
    PluginMock& operator=(PluginMock const& rhs);

};

#endif // PLUGINMOCK_H

