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
class Field;
class Register;
class RegisterBase;
class AddressBlock;
class MemoryMap;

class PortsInterface;
class ParametersInterface;
class ResetInterface;
class FieldInterface;
class RegisterInterface;
class AddressBlockInterface;

class ComponentParameterFinder;
class ExpressionParser;
class PortValidator;
class ExpressionFormatter;

class ParameterValidator;
class AddressBlockValidator;

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
     *  Get the interface for accessing address blocks.
     *
     *      @param [in] mapName     Name of the memory map containing the desired address blocks.
     *
     *      @return Interface for accessing address blocks.
     */
    AddressBlockInterface* getAddressBlockInterface(std::string const& mapName);

    /*!
     *  Get the interface for accessing registers.
     *
     *      @param [in] mapName     Name of the memory map containing the desired address blocks.
     *      @param [in] blockName   Name of the address block containing the desired registers.
     *
     *      @return Interface for accessing registers.
     */
    RegisterInterface* getRegisterInterface(std::string const& mapName, std::string const& blockName);

    /*!
     *  Get the interface for accessing fields.
     *
     *      @param [in] mapName         Name of the memory map containing the desired address blocks.
     *      @param [in] blockName       Name of the address block containing the desired registers.
     *      @param [in] registerName    Name of the register containing the desired fields.
     *
     *      @return Interface for accessing fields.
     */
    FieldInterface* getFieldInterface(std::string const& mapName, std::string const& blockName,
        std::string const& registerName);

    /*!
     *  Get the interface for accessing resets.
     *
     *      @param [in] mapName         Name of the memory map containing the desired address blocks.
     *      @param [in] blockName       Name of the address block containing the desired registers.
     *      @param [in] registerName    Name of the register containing the desired fields.
     *      @param [in] fieldName       Name of the field containing the desired resets.
     *
     *      @return Interface for accessing fields.
     */
    ResetInterface* getResetInterface(std::string const& mapName, std::string const& blockName,
        std::string const& registerName, std::string const& fieldName);

private:

    /*!
     *  Construct validator for memory items.
     */
    void constructMemoryValidators();

    /*!
     *  Construct interfaces for memory items.
     */
    void constructMemoryInterface();

    /*!
     *  Get the selected memory map.
     *
     *      @param [in] mapName     Name of the selected memory map.
     *
     *      @return The selected memory map.
     */
    QSharedPointer<MemoryMap> getMemoryMap(QString const& mapName) const;

    /*!
     *  Get the selected address block.
     *
     *      @param [in] containingMap   Memory map containing the address block.
     *      @param [in] blockName       Name of the selected address block.
     *
     *      @return The selected address block.
     */
    QSharedPointer<AddressBlock> getAddressBock(QSharedPointer<MemoryMap> containingMap, QString const& blockName)
        const;

    /*!
     *  Get the selected register.
     *
     *      @param [in] containingBlock     Address block containing the registers.
     *      @param [in] registerName        Name of the selected register.
     *
     *      @return The selected register.
     */
    QSharedPointer<Register> getRegister(QSharedPointer<AddressBlock> containingBlock, QString const& registerName)
        const;

    /*!
     *  Get the selected field.
     *
     *      @param [in] containingRegister  Register containing the fields.
     *      @param [in] fieldName           Name of the selected field.
     *
     *      @return The selected field.
     */
    QSharedPointer<Field> getField(QSharedPointer<Register> containingRegister, QString const& fieldName) const;

    /*!
     *  Send an error text for a non-existing memory map.
     *
     *      @param [in] mapName     Name of the memory map.
     */
    void sendMemoryMapNotFoundError(QString const& mapName) const;

    /*!
     *  Send an error text for a non-existing address block.
     *
     *      @param [in] mapName     Name of the memory map containing the address block.
     *      @param [in] blockName   Name of the address block.
     */
    void sendAddressBlockNotFoundError(QString const& mapName, QString const& blockName) const;

    /*!
     *  Send an error text for a non-existing register.
     *
     *      @param [in] mapName         Name of the memory map containing the address block.
     *      @param [in] blockName       Name of the address block containing the register.
     *      @param [in] registerName    Name of the register.
     */
    void sendRegisterNotFoundError(QString const& mapName, QString const& blockName, QString const& registerName)
        const;

    /*!
     *  Send an error text for a non-existing field.
     *
     *      @param [in] mapName         Name of the memory map containing the address block.
     *      @param [in] blockName       Name of the address block containing the register.
     *      @param [in] registerName    Name of the register containing the field.
     *      @param [in] fieldName       Name of the field.
     */
    void sendFieldNotFoundError(QString const& mapName, QString const& blockName, QString const& registerName,
        QString const& fieldName) const;

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

    //! Interface for accessing address blocks.
    AddressBlockInterface* blockInterface_;

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

    //! Validator for address blocks.
    QSharedPointer<AddressBlockValidator> blockValidator_;
};
