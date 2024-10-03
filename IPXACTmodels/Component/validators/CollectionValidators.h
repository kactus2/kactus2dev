//-----------------------------------------------------------------------------
// File: CollectionValidators.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 01.08.2024
//
// Description:
// Validators for validating top-level items inside a component.
//-----------------------------------------------------------------------------

#ifndef COLLECTIONVALIDATORS_H
#define COLLECTIONVALIDATORS_H

#include <QList>
#include <QSharedPointer>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <IPXACTmodels/common/validators/HierarchicalValidator.h>

namespace CollectionValidators
{
    //-----------------------------------------------------------------------------
    // Function: itemListToNameGroupList()
    //-----------------------------------------------------------------------------
    template <typename T> QSharedPointer<QList<QSharedPointer<NameGroup> > >
    itemListToNameGroupList(QSharedPointer<QList<QSharedPointer<T> > > items)
    {
        QSharedPointer<QList<QSharedPointer<NameGroup> > >listWithNameGroups(new QList<QSharedPointer<NameGroup> >);
        std::transform(items->begin(), items->end(), std::back_inserter(*listWithNameGroups), [](QSharedPointer<T> item)
            {
                return qSharedPointerCast<NameGroup>(item);
            });
        return listWithNameGroups;
    }
}

class MemoryMap;
class MemoryMapValidator;

/*
 *  Validator for memory maps.
 */
class IPXACTMODELS_EXPORT MemoryMapsValidator : public HierarchicalValidator
{
public:

    using MapList = QSharedPointer<QList<QSharedPointer<MemoryMap> > >;

    MemoryMapsValidator(QSharedPointer<MemoryMapValidator> mapValidator);
    virtual ~MemoryMapsValidator() = default;

    MemoryMapsValidator(MemoryMapsValidator& other) = delete;
    MemoryMapsValidator& operator=(MemoryMapsValidator& other) = delete;

    bool validate(MapList memoryMaps);

private:

    QSharedPointer<MemoryMapValidator> memMapValidator_;
};


class FileSet;
class FileSetValidator;

/*
 *  Validator for file sets.
 */
class IPXACTMODELS_EXPORT FileSetsValidator : public HierarchicalValidator
{
public:

    using FileSetList = QSharedPointer<QList<QSharedPointer<FileSet> > >;

    explicit FileSetsValidator(QSharedPointer<FileSetValidator> fileSetValidator);
    virtual ~FileSetsValidator() = default;

    FileSetsValidator(FileSetsValidator& other) = delete;
    FileSetsValidator& operator=(FileSetsValidator& other) = delete;

    bool validate(FileSetList fileSets);

private:

    QSharedPointer<FileSetValidator> fileSetValidator_;
};


class AddressSpace;
class AddressSpaceValidator;

/*
 *  Validator for address spaces.
 */
class IPXACTMODELS_EXPORT AddressSpacesValidator : public HierarchicalValidator
{
public:

    using AddressSpaceList = QSharedPointer<QList<QSharedPointer<AddressSpace> > >;

    explicit AddressSpacesValidator(QSharedPointer<AddressSpaceValidator> addressSpaceValidator );
    virtual ~AddressSpacesValidator() = default;

    AddressSpacesValidator(AddressSpacesValidator& other) = delete;
    AddressSpacesValidator& operator=(AddressSpacesValidator& other) = delete;

    bool validate(AddressSpaceList addressSpaces);

private:

    QSharedPointer<AddressSpaceValidator> addressSpaceValidator_;
};


class InstantiationsValidator;
class Component;
/*
 *  Validator for all instantiations.
 */
class IPXACTMODELS_EXPORT AllInstantiationsValidator: public HierarchicalValidator
{
public:

    explicit AllInstantiationsValidator(QSharedPointer<InstantiationsValidator> singleInstantiationValidator);
    virtual ~AllInstantiationsValidator() = default;

    AllInstantiationsValidator(AllInstantiationsValidator& other) = delete;
    AllInstantiationsValidator& operator=(AllInstantiationsValidator& other) = delete;

    bool validate(QSharedPointer<Component> component);

    bool hasValidComponentInstantiations(QSharedPointer<Component> component) const;

    bool hasValidDesignConfigurationInstantiations(QSharedPointer<Component> component) const;

    bool hasValidDesignInstantiations(QSharedPointer<Component> component) const;

private:

    QSharedPointer<InstantiationsValidator> singleInstantiationValidator_;
};

class View;
class ViewValidator;
/*
 *  Validator for views.
 */
class IPXACTMODELS_EXPORT ViewsValidator : public HierarchicalValidator
{
public:

    explicit ViewsValidator(QSharedPointer<ViewValidator> viewValidator);
    virtual ~ViewsValidator() = default;

    ViewsValidator(ViewsValidator& other) = delete;
    ViewsValidator& operator=(ViewsValidator& other) = delete;

    bool validate(QSharedPointer<QList<QSharedPointer<View> > > views);

private:

    QSharedPointer<ViewValidator> viewValidator_;
};

class Port;
class PortValidator;
/*
 *  Validator for ports.
 */
class IPXACTMODELS_EXPORT PortsValidator: public HierarchicalValidator
{
public:

    explicit PortsValidator(QSharedPointer<PortValidator> portValidator);
    virtual ~PortsValidator() = default;

    PortsValidator(PortsValidator& other) = delete;
    PortsValidator& operator=(PortsValidator& other) = delete;

    bool validate(QSharedPointer<QList<QSharedPointer<Port> > > ports);

private:

    QSharedPointer<PortValidator> portValidator_;
};

class BusInterface;
class BusInterfaceValidator;
/*
 *  Validator for bus interfaces.
 */
class IPXACTMODELS_EXPORT BusInterfacesValidator : public HierarchicalValidator
{
public:

    explicit BusInterfacesValidator(QSharedPointer<BusInterfaceValidator> busInterfaceValidator);
    virtual ~BusInterfacesValidator() = default;

    BusInterfacesValidator(BusInterfacesValidator& other) = delete;
    BusInterfacesValidator& operator=(BusInterfacesValidator& other) = delete;

    bool validate(QSharedPointer<QList<QSharedPointer<BusInterface> > > interfaces, Document::Revision docRevision);

private:

    QSharedPointer<BusInterfaceValidator> busInterfaceValidator_;
};

class IndirectInterface;
class IndirectInterfaceValidator;
/*
 *  Validator for indirect interfaces.
 */
class IPXACTMODELS_EXPORT IndirectInterfacesValidator : public HierarchicalValidator
{
public:

    explicit IndirectInterfacesValidator(QSharedPointer<IndirectInterfaceValidator> indirectInterfaceValidator);
    virtual ~IndirectInterfacesValidator() = default;

    IndirectInterfacesValidator(IndirectInterfacesValidator& other) = delete;
    IndirectInterfacesValidator& operator=(IndirectInterfacesValidator& other) = delete;

    bool validate(QSharedPointer<QList<QSharedPointer<IndirectInterface> > > indirectInterfaces);

private:

    QSharedPointer<IndirectInterfaceValidator> indirectInterfaceValidator_;
};


class Cpu;
class CPUValidator;
/*
 *  Validator for modes.
 */
class IPXACTMODELS_EXPORT CPUsValidator: public HierarchicalValidator
{
public:

    explicit CPUsValidator(QSharedPointer<CPUValidator> cpuValidator);
    virtual ~CPUsValidator() = default;

    CPUsValidator(CPUsValidator& other) = delete;
    CPUsValidator& operator=(CPUsValidator& other) = delete;

    bool validate(QSharedPointer<QList<QSharedPointer<Cpu> > > cpus);

private:

    QSharedPointer<CPUValidator> cpuValidator_;
};


class PowerDomain;
class PowerDomainValidator;
/*
 *  Validator for modes.
 */
class IPXACTMODELS_EXPORT PowerDomainsValidator : public HierarchicalValidator
{
public:

    explicit PowerDomainsValidator(QSharedPointer<PowerDomainValidator> powerDomainValidator);
    virtual ~PowerDomainsValidator() = default;

    PowerDomainsValidator(PowerDomainsValidator& other) = delete;
    PowerDomainsValidator& operator=(PowerDomainsValidator& other) = delete;

    bool validate(QSharedPointer<QList<QSharedPointer<PowerDomain> > > powerDomains);

private:

    QSharedPointer<PowerDomainValidator> powerDomainValidator_;
};

#endif // COLLECTIONVALIDATORS_H
