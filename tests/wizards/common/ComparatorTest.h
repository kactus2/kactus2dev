

#ifndef COMPARATORTEST_H
#define COMPARATORTEST_H


#include <QSharedPointer>

#include <wizards/common/IPXactDiff.h>


class ComparatorTest 
{    
    
public:
    ComparatorTest();

    virtual ~ComparatorTest();
   
protected:
  
    void verifyDiffElementAndType(QSharedPointer<IPXactDiff> diff, QString const& expectedElement,
        QString const& expectedName, IPXactDiff::DiffType expectedType) const;

    void verifyModificationIs(IPXactDiff::Modification const& modification, QString const& element, 
        QString const& previousValue, QString const& newValue) const;
};

#endif // COMPARATORTEST_H
