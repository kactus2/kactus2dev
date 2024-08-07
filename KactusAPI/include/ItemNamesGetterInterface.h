#ifndef ITEMNAMESGETTER_H
#define ITEMNAMESGETTER_H
#include "CommonInterface.h"
#include "NameGroupInterface.h"
#include <QSharedPointer>
#include <QList>
template <typename T>
class KACTUS2_API ItemNamesGetterInterface : public NameGroupInterface
{
public:
    ItemNamesGetterInterface(QSharedPointer<QList<QSharedPointer<T>>> existingItems);
    ItemNamesGetterInterface();
    virtual ~ItemNamesGetterInterface() = default;

    std::vector<std::string> getItemNames() const override;

    int itemCount() const override; // Implemented only as a placeholder to make a class not abstract

    bool validateItems() const override; // Implemented only as a placeholder to make a class not abstract

    int getItemIndex(std::string const& itemName) const override; // Implemented only as a placeholder to make a class not abstract
    std::string getIndexedItemName(int itemIndex) const override; // Implemented only as a placeholder to make a class not abstract
    bool setName(std::string const& currentName, std::string const& newName) override; // Implemented only as a placeholder to make a class not abstract

    bool itemHasValidName(std::string const& itemName) const override;
protected:
    QSharedPointer<QList<QSharedPointer<T>>> existingItems_;
private:
    QSharedPointer<NameGroup> getItem(std::string const& itemName) const override;
};
#endif // ITEMNAMESGETTER_H