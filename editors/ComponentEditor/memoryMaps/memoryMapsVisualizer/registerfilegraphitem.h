#ifndef REGISTERFILEGRAPHITEM_H
#define REGISTERFILEGRAPHITEM_H

#include <editors/ComponentEditor/visualization/memoryvisualizationitem.h>

#include <IPXACTmodels/Component/RegisterFile.h>

#include <QGraphicsItem>
#include <QSharedPointer>

class MemoryVisualizationItem;
class ExpressionParser;
class RegisterFileGraphItem : public MemoryVisualizationItem
{
public:
    RegisterFileGraphItem(QSharedPointer<RegisterFile> regFile,
                          QSharedPointer<ExpressionParser> expressionParser,
                          QGraphicsItem *parent);

    virtual ~RegisterFileGraphItem();
    virtual void refresh();
    virtual void updateDisplay();

    virtual quint64 getOffset() const;
    virtual quint64 getLastAddress() const;
    virtual int getBitWidth() const;

    virtual unsigned int getAddressUnitSize() const;
    void setDimensionIndex(unsigned int index);
    virtual bool isPresent() const;

private:
    RegisterFileGraphItem(const RegisterFileGraphItem& other);
    RegisterFileGraphItem& operator=(const RegisterFileGraphItem& other);

    //! Pointer to the register being visualized.
    QSharedPointer<RegisterFile> regFile_;
    unsigned int dimensionIndex_;
};

#endif // REGISTERFILEGRAPHITEM_H
