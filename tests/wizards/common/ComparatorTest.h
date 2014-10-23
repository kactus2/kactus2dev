//-----------------------------------------------------------------------------
// File: ComparatorTest.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.10.2014
//
// Description:
// Base class for all comparator unit tests.
//-----------------------------------------------------------------------------

#ifndef COMPARATORTEST_H
#define COMPARATORTEST_H


#include <QSharedPointer>

#include <wizards/common/IPXactDiff.h>

//-----------------------------------------------------------------------------
//! Base class for all comparator unit tests.
//-----------------------------------------------------------------------------
class ComparatorTest 
{    
    
public:
    ComparatorTest();

    virtual ~ComparatorTest();
   
protected:

    virtual void createTestInputsForStringValues();

    virtual void createTestInputForStringListValues();

    virtual void verifyDiffElementAndType(QSharedPointer<IPXactDiff> diff, QString const& expectedElement,
        QString const& expectedName, IPXactDiff::DiffType expectedType) const;

    virtual void verifyModificationIs(IPXactDiff::Modification const& modification, QString const& element, 
        QString const& previousValue, QString const& newValue) const;
};

#endif // COMPARATORTEST_H
