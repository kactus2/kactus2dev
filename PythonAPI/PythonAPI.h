//-----------------------------------------------------------------------------
// File: PythonAPI.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.02.2020
//
// Description:
// Interface for accessing Kactus2 data through python.
//-----------------------------------------------------------------------------

// #pragma once

#include "pythonapi_global.h"

#include <QString>

#include <string>
#include <vector>
#include <QSharedPointer>

class LibraryInterface;
class MessageMediator;
class Component;

class SimplePort;

//-----------------------------------------------------------------------------
//! Interface for accessing Kactus2 data through python.
//-----------------------------------------------------------------------------
// class PYTHONAPI_EXPORT PythonAPI
class PythonAPI
{
public:
    
    /*!
     *  The constructor.
     */
    PythonAPI();

    /*!
     *  The destructor.
     */
    ~PythonAPI() = default;

    /*!
     *  Setup the library.
     *
     *      @param [in] settingsFile    Path of the settings file.
     */
    void setupLibrary(QString const& settingsFile);

    /*!
     *  Get the number of files in the library.
     *
     *      @return The number of files in the library.
     */
    int getFileCount() const;

    /*!
     *  List all the VLNVs in the library.
     *
     *      @param[in] vendor   The desired vendor.
     */
    void listVLNVs(QString const& vendor = QString("")) const;

    /*!
     *  List all the component VLNVs in the library.
     */
    void listComponentVLNVs() const;

    /*!
     *  Set the selected component as active component.
     *
     *      @param [in] componentVLNV   VLNV of the selected component.
     *
     *      @return True, if the component exists, false otherwise.
     */
    bool openComponent(QString const& componentVLNV);

    /*!
     *  Remove the active component.
     */
    void closeOpenComponent();

    /*!
     *  Get the name of the active component.
     */
    QString getComponentName();

    /*!
     *  Get the description of the active component.
     */
    QString getComponentDescription();

    /*!
     *  Save the component to the library.
     */
    void saveComponent();

    /*!
     *  List the ports in the active component.
     */
    void listComponentPorts();

    /*!
     *  Set a new name for the selected port.
     *
     *      @param [in] currentPortName     Name of the selected port.
     *      @param [in] newPortName         New name for the port.
     */
    void setPortName(QString const& currentPortName, QString const& newPortName);

    /*!
     *  Create simple ports from the active component.
     *
     *      @return Simple ports created from the ports of the active component.
     */
    std::vector<SimplePort*> createSimplePortsForComponent();

private:

    //! Interface to the library.
    LibraryInterface* library_;

    //! Message handler.
    MessageMediator* messager_;

    //! Currently active component.
    QSharedPointer<Component> activeComponent_;
};
