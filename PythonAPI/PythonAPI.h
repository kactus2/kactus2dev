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
class PortsInterface;
class ComponentParameterFinder;
class ExpressionParser;
class PortValidator;
class ExpressionFormatter;
class ParametersInterface;
class ParameterValidator;

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
     *  Get the interface for accessing the component ports.
     *
     *      @return Interface for accessing the component ports.
     */
    PortsInterface* getPortsInterface() const;

    /*!
     *  Get the interface for accessing the component parameters.
     *
     *      @return Interface for accessing the component parameters.
     */
    ParametersInterface* getComponentParameterInterface() const;

private:

    //! Interface to the library.
    LibraryInterface* library_;

    //! Message handler.
    MessageMediator* messager_;

    //! Currently active component.
    QSharedPointer<Component> activeComponent_;

    //! Interface for accessing the component ports.
    PortsInterface* portsInterface_;

    //! Interface for accessing the component parameters.
    ParametersInterface* componentParameterInterface_;

    //! Component parameter finder.
    QSharedPointer<ComponentParameterFinder> parameterFinder_;

    //! Parser for expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! Formatter for expressions.
    QSharedPointer<ExpressionFormatter>expressionFormatter_;

    //! Validator for ports.
    QSharedPointer<PortValidator> portValidator_;

    //! Validator for parameters.
    QSharedPointer<ParameterValidator> parameterValidator_;
};
