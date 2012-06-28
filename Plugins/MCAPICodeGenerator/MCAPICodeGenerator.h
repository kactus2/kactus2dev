//-----------------------------------------------------------------------------
// File: McapiCodeGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// MCAPI code generator plugin.
//-----------------------------------------------------------------------------

#ifndef MCAPICODEGENERATOR_H
#define MCAPICODEGENERATOR_H

#include "mcapicodegenerator_global.h"

#include <PluginSystem/IPluginInformation.h>
#include <PluginSystem/IGeneratorPlugin.h>

#include <QObject>

class Component;
class CSourceWriter;

//-----------------------------------------------------------------------------
//! MCAPI code generator.
//-----------------------------------------------------------------------------
class MCAPICodeGenerator : public QObject, public IPluginInformation, public IGeneratorPlugin
{
    Q_OBJECT
    Q_INTERFACES(IPluginInformation)
    Q_INTERFACES(IGeneratorPlugin)

public:
    MCAPICodeGenerator();
    ~MCAPICodeGenerator();

    /*!
     *  Returns the name of the plugin.
     */
    virtual QString const& getName() const;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString const& getVersion() const;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString const& getDescription() const;

    /*!
     *  Checks whether the generator supports generation for the given library component.
     *
     *      @param [in] libComp The library component for which to check support.
     *
     *      @return True, if the generator supports the given component. Otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent> libComp) const;

    /*!
     *  Runs the generator.
     *
     *      @param [in,out] libComp       The component for which the generator is run.
     *      @param [in]     libInterface  The library interface to get access to the library.
     */
    virtual void runGenerator(QSharedPointer<LibraryComponent> libComp,
                              LibraryInterface const* libInterface);
private:
    void generateHeader(QString const& filename, QSharedPointer<Component> component);

    void generateMainTemplate(QString const& filename, QSharedPointer<Component> component);
    void generateSource(QString const& filename, QSharedPointer<Component> component);
    QString createIndentString();
    void writeWaitCall(CSourceWriter& writer, QString const& requestName, QString const& sizeName);
    void writeStatusCheck(CSourceWriter &writer);
    void generateInitializeMCAPIFunc(CSourceWriter& writer, QSharedPointer<Component> component);
    void generateCloseConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component);
    void generateCreateConnectionsFunc(CSourceWriter& writer, QSharedPointer<Component> component);
};

#endif // MCAPICODEGENERATOR_H
