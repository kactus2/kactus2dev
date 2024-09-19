//#include "ItemNamesGetterInterface.h"
//#include <IPXACTmodels/Component/DesignInstantiation.h>
//#include <IPXACTmodels/Component/MemoryMap.h>
//
//
////-----------------------------------------------------------------------------
//// Function: ItemNamesGetterInterface::ItemNamesGetterInterface()
////-----------------------------------------------------------------------------
//template <typename T>
//ItemNamesGetterInterface<T>::ItemNamesGetterInterface(QSharedPointer<QList<QSharedPointer<T>>> existingItems)
//    : NameGroupInterface(),
//    existingItems_(existingItems)
//{
//
//}
//template <typename T>
//ItemNamesGetterInterface<T>::ItemNamesGetterInterface()
//    : NameGroupInterface()
//{
//
//}
//
//template <typename T>
//std::vector<std::string> ItemNamesGetterInterface<T>::getItemNames() const
//{
//    std::vector<std::string> names;
//    for (auto item : *existingItems_)
//    {
//        names.push_back(item->name().toStdString());
//    }
//
//    return names;
//}
//
//template <typename T>
//int ItemNamesGetterInterface<T>::itemCount() const
//{
//    return -1;
//}
//
//
//template <typename T>
//bool ItemNamesGetterInterface<T>::validateItems() const
//{
//    return false;
//}
//
//template <typename T>
//int ItemNamesGetterInterface<T> ::getItemIndex(std::string const& itemName) const {
//    return -1;
//}
//
//template <typename T>
//std::string ItemNamesGetterInterface<T> ::getIndexedItemName(int itemIndex) const {
//    return std::string();
//}
//
//template <typename T>
//bool ItemNamesGetterInterface<T> ::setName(std::string const& currentName, std::string const& newName) {
//    return false;
//}
//template<typename T>
//bool ItemNamesGetterInterface<T>::itemHasValidName(std::string const& itemName) const
//{
//    return false;
//}
//template<typename T>
//QSharedPointer<NameGroup> ItemNamesGetterInterface<T>::getItem(std::string const& itemName) const
//{
//    return QSharedPointer<NameGroup>();
//}
//template class ItemNamesGetterInterface<DesignInstantiation>;
//template class ItemNamesGetterInterface<MemoryMap>;