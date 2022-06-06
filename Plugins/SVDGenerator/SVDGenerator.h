//-----------------------------------------------------------------------------
// File: SVDGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.04.2021
//
// Description:
// Creates a CMSIS System View Description listing.
//-----------------------------------------------------------------------------

#ifndef SVDGENERATOR_H
#define SVDGENERATOR_H

#include <QList>
#include <QString>
#include <QSharedPointer>

#include "svdgeneratorplugin_global.h"

#include <ConnectivityGraphUtilities.h>

#include <editors/MemoryDesigner/ConnectivityGraphFactory.h>

#include <IPXACTmodels/Component/BusInterface.h>

#include <IPXACTmodels/Design/ActiveInterface.h>
#include <IPXACTmodels/Design/Design.h>

#include <IPXACTmodels/generaldeclarations.h>

class AddressBlock;
class BusInterface;
class Component;
class ExpressionParser;
class Field;
class ConnectivityInterface;
class LibraryInterface;
class MemoryItem;
class Register;
class Cpu;
class MemoryMap;
class EnumeratedValue;

//-----------------------------------------------------------------------------
//! Creates a CMSIS System View Description listing.
//-----------------------------------------------------------------------------
class SVDGENERATORPLUGIN_EXPORT SVDGenerator
{
public:
    
    /*!
     *  The constructor.
     *
     *      @param [in] library     Interface for accessing libraries.
     */
    explicit SVDGenerator(LibraryInterface* library);

    /*!
     *  The destructor.
     */
    ~SVDGenerator() = default;

    // Disable copying.
    SVDGenerator(SVDGenerator const& rhs) = delete;
    SVDGenerator& operator=(SVDGenerator const& rhs) = delete;

    /*!
     *  Generates slave memory listing for the given component.
     *
     *      @param [in] topComponent            The top component in the hierarchy to generate listing for.
     *      @param [in] componentPath           Path to the component folder.
     *      @param [in] cpuRoutes               CPU and its connected routes.
     */
    void generate(QSharedPointer<Component> topComponent, QString const& componentPath,
        QVector<QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> > const& cpuRoutes);

    /*!
     *  Get the generated files.
     *
     *      @return The generated files.
     */
    QStringList getGeneratedFiles() const;

private:


    /*!
     *  Writes the CPU listing into a given file.
     *
     *      @param [in] topComponent            Top component of the design.
     *      @param [in] componentPath           Path to the component folder.
     *      @param [in] cpuRoute                The selected CPU route container.
     *      @param [in] fileNames               Names of the generated SVD files.
     */
    void writeFile(QSharedPointer<Component> topComponent, QString const& componentPath,
        QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> cpuRoute, QStringList& fileNames);

    /*!
     *  Get the number of files containing the selected name.
     *
     *      @param [in] fileNames   List of the generated files.
     *      @param [in] fileName    The selected file name.
     *
     *      @return Number of files containing the same name.
     */
    int getFileNumberExtension(QStringList const& fileNames, QString const& fileName) const;

    /*!
     *  Write the device information.
     *
     *      @param [in] writer          The xml stream writer.
     *      @param [in] topComponent    Top component of the containing design.
     */
    void writeDevice(QXmlStreamWriter& writer, QSharedPointer<Component> topComponent);

    /*!
     *  Remove ' ' from the front and back of the selected name and replace the remaining with _.
     *
     *      @param [in] name    The selected name.
     *
     *      @return The formatted name.
     */
    QString formatName(QString const& name) const;

    /*!
     *  Write the selected CPU.
     *
     *      @param [in] writer          The xml stream writer.
     *      @param [in] currentCPU      The selected CPU.
     *      @param [in] cpuContianer    The CPU data container.
     */
    void writeCPU(QXmlStreamWriter& writer, QSharedPointer<Cpu> currentCPU,
        QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> cpuContainer);

    /*!
     *  Write a boolean flag.
     *
     *      @param [in] writer          The xml stream writer.
     *      @param [in] elementName     Name of the selected boolean flag.
     *      @param [in] state           The flag state.
     */
    void writeBoolean(QXmlStreamWriter& writer, QString const& elementName, bool state);

    /*!
     *  Write the address space of the selected CPU.
     *
     *      @param [in] writer          The xml stream writer.
     *      @param [in] cpuInterface    Interface containing the selected CPU.
     */
    void writeAddressSpaceData(QXmlStreamWriter& writer, QSharedPointer<const ConnectivityInterface> cpuInterface);

    /*!
     *  Write the peripherals of the selected CPU route container.
     *
     *      @param [in] writer                  The xml stream writer.
     *      @param [in] routeCollection         The selected CPU route container.
     */
    void writePeripherals(QXmlStreamWriter& writer,
        QSharedPointer<ConnectivityGraphUtilities::cpuDetailRoutes> routeCollection);

    /*!
     *  Write memory map peripheral of the selected memory item.
     *
     *      @param [in] writer                  The xml stream writer.
     *      @param [in] component               Component containing the interfaced memory map item.
     *      @param [in] mapItem                 The selected memory item.
     *      @param [in] mapBaseAddress          Base address of the memory map.
     *      @param [in] mapBaseAddressInHexa    Base address of the memory map in hex format.
     */
    void writePeripheral(QXmlStreamWriter& writer, QSharedPointer<const Component> component,
        QSharedPointer<MemoryItem> mapItem, quint64 mapBaseAddress, QString const& mapBaseAddressInHexa);

    /*!
     *  Write address blocks of the selected memory map item.
     *
     *      @param [in] writer                  The xml stream writer.
     *      @param [in] containingComponent     Component containing the interfaced memory map item.
     *      @param [in] mapItem                 The selected memory map item.
     *      @param [in] mapBaseAddress          Base address of the memory map.
     */
    void writeAddressBlocks(QXmlStreamWriter& writer, QSharedPointer<const Component> containingComponent,
        QSharedPointer<MemoryItem> mapItem, quint64 mapBaseAddress);

    /*!
     *  Get the available address block items of the selected memory map item.
     *
     *      @param [in] mapItem     The selected memory map item.
     *
     *      @return Address blocks with usage other than memory of the selected memory map item.
     */
    QVector<QSharedPointer<MemoryItem> > getAddressBlockItems(QSharedPointer<MemoryItem> mapItem) const;

    /*!
     *  Get a list of the selected sub items of the selected memory item.
     *
     *      @param [in] memoryItem      The selected memory item.
     *      @param [in] subItemType     Type of the sub item.
     *
     *      @return List of the selected sub items.
     */
    QVector<QSharedPointer<MemoryItem>> getSubMemoryItems(QSharedPointer<MemoryItem> memoryItem,
        QString const& subItemType) const;

    /*!
     *  Write the selected address block.
     *
     *      @param [in] writer              The xml stream writer.
     *      @param [in] offset              Offset of the address block.
     *      @param [in] blockItem           The selected address block item.
     */
    void writeSingleAddressBlock(QXmlStreamWriter& writer, quint64 const& offset, 
         QSharedPointer<MemoryItem> blockItem);

    void writeRegisters(QXmlStreamWriter& writer, QSharedPointer<const Component> containingComponent,
        QSharedPointer<MemoryItem> mapItem, quint64 mapBaseAddress);

    /*!
     *  Write the register cluster.
     *
     *      @param [in] writer                  The xml stream writer.
     *      @param [in] containingBlock         The address block containing the registers.
     *      @param [in] blockItem               The selected address block item.
     *      @param [in] addressOffsetInHexa     Offset of the address block in hex format.  
     */
    void writeRegisterCluster(QXmlStreamWriter& writer, QSharedPointer<AddressBlock> containingBlock,
        QSharedPointer<MemoryItem> blockItem, QString const& addressOffsetInHexa);

    /*!
     *  Write the selected register elements.
     *
     *      @param [in] writer             The xml stream writer.
     *      @param [in] registerItem       The selected register item.
     *      @param [in] realRegister       The selected IP-XACT register.
     */
    void writeRegister(QXmlStreamWriter& writer, QSharedPointer<MemoryItem> registerItem,
        QSharedPointer<Register> realRegister);

    /*!
     * Write the selected reset element.
     *
     *     @param [in] writer       The xml stream writer.
     *     @param [in] resetItem    The selecter reset item.
     */
     void writeReset(QXmlStreamWriter& writer, QSharedPointer<MemoryItem> resetItem) const;

    /*!
     *  Get the field items of the selected register item in ascending offset order.
     *
     *      @param [in] registerItem    The selected register item.
     *      @param [in] registerOffset  Offset of the register.
     *
     *      @return Field items of the selected register item in ascending offset order.
     */
    QMap<quint64, QSharedPointer<MemoryItem> > getFieldItemsInOrder(QSharedPointer<MemoryItem> registerItem,
        quint64 const& registerOffset);

    /*!
     *  Get the first reset item of the selected memory item.
     *
     *      @param [in] registerItem    The selected memory item.
     *
     *      @return The first reset item of the selected memory item.
     */
    QSharedPointer<MemoryItem> getResetItem(QSharedPointer<MemoryItem> registerItem) const;

    /*!
     *  Write the fields of the selected register.
     *
     *      @param [in] writer              The xml stream writer.
     *      @param [in] containingRegister  The selected register.
     *      @param [in] fieldItems          Ordered field items.
     */
    void writeFields(QXmlStreamWriter& writer, QSharedPointer<Register> containingRegister,
        QMap<quint64, QSharedPointer<MemoryItem> > fieldItems);

    /*!
     *  Write the enumerated values of the selected field.
     *
     *      @param [in] writer              The xml stream writer.
     *      @param [in] containingField     The selected field.
     *      @param [in] fieldItem           The selected field item.
     */
    void writeEnumeratedValues(QXmlStreamWriter& writer, QSharedPointer<Field> containingField,
        QSharedPointer<MemoryItem> fieldItem);

    /*!
     *  Get the selected memory map.
     *
     *      @param [in] interfaceMemory         Memory item of the selected memory map.
     *      @param [in] containingComponent     Component containing the memory map.
     *
     *      @return Memory map of the selected memory map item.
     */
    QSharedPointer<MemoryMap> getMemoryMap(QSharedPointer<MemoryItem> interfaceMemory,
        QSharedPointer<const Component> containingComponent) const;

    /*!
     *  Get the selected address block.
     *
     *      @param [in] containingComponent     Component containing the address block.
     *      @param [in] mapItem                 Memory item of the containing memory map.
     *      @param [in] blockItem               Memory item of the selected address block.
     *
     *      @return Address block of the selected address block item.
     */
    QSharedPointer<AddressBlock> getAddressBlock(QSharedPointer<const Component> containingComponent,
        QSharedPointer<MemoryItem> mapItem, QSharedPointer<MemoryItem> blockItem) const;

    /*!
     *  Get the selected register.
     *
     *      @param [in] containingBlock     Memory item of the containing address block.
     *      @param [in] registerItem        Memory item of the selected register.
     *
     *      @return Register of the selected register item.
     */
    QSharedPointer<Register> getRegister(QSharedPointer<AddressBlock> containingBlock,
        QSharedPointer<MemoryItem> registerItem) const;

    /*!
     *  Get the selected field.
     *
     *      @param [in] containingRegister  Memory item of the containing register.
     *      @param [in] fieldItem           Memory item of the selected field.
     *
     *      @return Field of the selected field item.
     */
    QSharedPointer<Field> getField(QSharedPointer<Register> containingRegister,
        QSharedPointer<MemoryItem> fieldItem) const;

    /*!
     *  Get the start of the selected field.
     *
     *      @param [in] fieldItem       The selected field item.
     *      @param [in] registerOffset  Offset of the containing register.
     *
     *      @return Start of the selected field.
     */
    quint64 getFieldStart(QSharedPointer<MemoryItem> fieldItem, quint64 registerOffset);

    /*!
     *  Get the end of the selected field.
     *
     *      @param [in] fieldItem       The selected field item.
     *      @param [in] registerOffset  Offset of the containing register.
     *
     *      @return End of the selected field.
     */
    quint64 getFieldEnd(QSharedPointer<MemoryItem> fieldItem, quint64 fieldOffset);

    /*!
     *  Get the selected enumerated value.
     *
     *      @param [in] containingField     Memory item of the containing field.
     *      @param [in] enumeratedItem      Memory item of the selected enumerated value.
     *
     *      @return Enumerated value of the selected enumerated value item.
     */
    QSharedPointer<EnumeratedValue> getEnumeratedValue(QSharedPointer<Field> containingField,
        QSharedPointer<MemoryItem> enumeratedItem) const;

    /*!
     *  Change the selected value to hex format.
     *
     *      @param [in] value   The selected value.
     *
     *      @return The selected value in hex format.
     */
    QString valueToHexa(quint64 const& value) const;

    /*!
     *  Write an optional the text element.
     *
     *      @param [in] writer          The xml stream writer.
     *      @param [in] elementName     Name of the element.
     *      @param [in] elementValue    Value of the element.
     */
    void writeOptionalElement(QXmlStreamWriter& writer, QString const& elementName, QString const& elementValue);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The available IP-XACT library.
    LibraryInterface* library_;

    //! Factory for creating connectivity graphs.
    ConnectivityGraphFactory graphFactory_;

    //! Paths to the generated files.
    QStringList generatedFiles_;
};

#endif // SVDGENERATOR_H
