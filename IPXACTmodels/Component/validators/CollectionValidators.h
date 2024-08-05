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
                return item.staticCast<NameGroup>();
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

#endif // COLLECTIONVALIDATORS_H
