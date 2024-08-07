

#include "AddressSpaceInterface.h"

#include <AddressBlockExpressionsGatherer.h>

#include <IPXACTmodels/Component/MemoryBlockBase.h>
#include <IPXACTmodels/Component/AddressBlock.h>
#include <IPXACTmodels/Component/AccessPolicy.h>
#include <IPXACTmodels/Component/validators/AddressBlockValidator.h>

template <typename T>
AddressSpaceInterface<T>::AddressSpaceInterface() {

}

template <typename T>
int AddressSpaceInterface<T>::getItemIndex(std::string const& itemName) const
{
    QString itemNameQ = QString::fromStdString(itemName);

    int index = 0;
    for (const auto& item : *this->existingItems_) {
        if (item->getName() == itemNameQ) {
            return index;
        }
        ++index;
    }

    return -1;
}