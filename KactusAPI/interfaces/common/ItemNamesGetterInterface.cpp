#include "ItemNamesGetterInterface.h"
#include <IPXACTmodels/Component/DesignInstantiation.h>


//-----------------------------------------------------------------------------
// Function: ItemNamesGetterInterface::ItemNamesGetterInterface()
//-----------------------------------------------------------------------------
template <typename T>
ItemNamesGetterInterface<T>::ItemNamesGetterInterface(QSharedPointer<QList<QSharedPointer<T>>> reservedNames) 
    : CommonInterface(),
    reservedNames_(reservedNames)
{

}

template <typename T>
std::vector<std::string> ItemNamesGetterInterface<T>::getItemNames() const
{
    std::vector<std::string> names;
    for (auto item : *reservedNames_)
    {
        names.push_back(item->name().toStdString());
    }

    return names;
}

template <typename T>
int ItemNamesGetterInterface<T>::itemCount() const
{
    return -1;
}


template <typename T>
bool ItemNamesGetterInterface<T>::validateItems() const
{
    return false;
}
template class ItemNamesGetterInterface<DesignInstantiation>;