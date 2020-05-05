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
class FieldInterface;
class FieldValidator;
class Field;
class RegisterBase;
class RegisterInterface;
class RegisterValidator;

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

    /*!
     *  Get the interfaces for accessing the address block fields.
     *
     *      @return Interface for accessing the address block fields.
     */
    std::vector<RegisterInterface*> getRegisterInterfaces() const;

private:

    /*!
     *  Setup the register interfaces.
     *
     *      @param [in] component   Component containing the registers.
     */
    void setupRegisterInterfaces(QSharedPointer<Component> component);

    /*!
     *  Setup the field interfaces.
     *
     *      @param [in] containingRegisterInterface     Register interface containing the field interfaces.
     *      @param [in] registers                       List of component registers.
     *      @param [in] validator                       Validator for fields.
     */
    void setupFieldInterfaces(RegisterInterface* containingRegisterInterface,
        QSharedPointer<QList<QSharedPointer<RegisterBase> > > registers, QSharedPointer<FieldValidator> validator)
        const;

    /*!
     *  Setup the reset interfaces.
     *
     *      @param [in] containingFieldInterface    Field interface containing the reset interfaces.
     *      @param [in] registerFields              List of register fields.
     *      @param [in] validator                   Validator for fields.
     */
    void setupResetInterfaces(FieldInterface* containingFieldInterface,
        QSharedPointer<QList<QSharedPointer<Field> > > registerFields, QSharedPointer<FieldValidator> validator)
        const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

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

    //! List of register interfaces.
    std::vector<RegisterInterface*> registerInterfaces_;

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

    //! Validator for registers.
    QSharedPointer<RegisterValidator> registerValidator_;
};
