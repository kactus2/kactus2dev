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


#endif // COLLECTIONVALIDATORS_H
