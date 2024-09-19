

#ifndef ADDRESSSPACEINTERFACE_H
#define ADDRESSSPACEINTERFACE_H

#include <KactusAPI/KactusAPIGlobal.h>

#include <MemoryBlockInterface.h>

#include <IPXACTmodels/generaldeclarations.h>
#include <IPXACTmodels/common/AccessTypes.h>

class AddressBlock;
class MemoryBlockBase;
class AddressBlockValidator;
class RegisterInterface;
class BusInterfaceInterface;

#include <QVector>
#include <QMap>
#include <ItemNamesGetterInterface.h>

template <typename T>
class KACTUS2_API AddressSpaceInterface : public NameGroupInterface
{
//
//public:
//    AddressSpaceInterface();
//    using ItemNamesGetterInterface<T>::ItemNamesGetterInterface;
//
//    virtual int getItemIndex(std::string const& itemName) const override final;

};

#endif // ADDRESSSPACEINTERFACE_H
