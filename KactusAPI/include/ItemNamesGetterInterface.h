#ifndef ITEMNAMESGETTER_H
#define ITEMNAMESGETTER_H
#include "CommonInterface.h"
#include <QSharedPointer>
#include <QList>
template <typename T>
class KACTUS2_API ItemNamesGetterInterface : public CommonInterface
{
public:
    ItemNamesGetterInterface(QSharedPointer<QList<QSharedPointer<T>>> reservedNames);
    virtual ~ItemNamesGetterInterface() = default;

    std::vector<std::string> getItemNames() const override;

    int itemCount() const override;

    bool validateItems() const override;
private:
    QSharedPointer<QList<QSharedPointer<T>>> reservedNames_;
};
#endif // ITEMNAMESGETTER_H