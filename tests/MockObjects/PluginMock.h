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

#include <KactusAPI/include/IPlugin.h>
#include <KactusAPI/include/CLIGenerator.h>

class PluginMock : public IPlugin, public CLIGenerator
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

    virtual QString getLicense() const;

    virtual QString getLicenseHolder() const;

    virtual QWidget* getSettingsWidget();

    virtual PluginSettingsModel* getSettingsModel();

    virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

    virtual QString getCommand() const;

    virtual void process(QStringList const& arguments, IPluginUtility* utility);

    QString getOutputFormat() const override;

    void runGenerator(IPluginUtility* utility, QSharedPointer<Component> component, QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration, QString const& viewName, QString const& outputDirectory) override;

private:

    // Disable copying.
    PluginMock(PluginMock const& rhs);
    PluginMock& operator=(PluginMock const& rhs);

};

#endif // PLUGINMOCK_H

