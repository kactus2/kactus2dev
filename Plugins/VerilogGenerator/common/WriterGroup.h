//-----------------------------------------------------------------------------
// File: WriterGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 29.08.2014
//
// Description:
// Composite for writers.
//-----------------------------------------------------------------------------

#ifndef WriterGROUP_H
#define WriterGROUP_H

#include "Writer.h"

#include "../veriloggeneratorplugin_global.h"

#include <QSharedPointer>
#include <QTextStream>

//-----------------------------------------------------------------------------
//! Composite for writers.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT WriterGroup : public Writer
{
public:

    //! The constructor.
    WriterGroup();

	//! The destructor.
    virtual ~WriterGroup();

    /*!
     *  Writes the content to the given output.
     *
     *      @param [in] output   The output to write to.
     */
    virtual void write(QTextStream& output) const;
    
    /*!
     *  Adds a new writer to the group.
     *
     *      @param [in] writer   The writer to add.     
     */
    void add(QSharedPointer<Writer> writer);
        /*!
     *  Checks if the group has no writers in it.
     *
     *      @return True, if the group has no writers, otherwise false.
     */
    bool hasNoWriters() const;   
protected:

 

private:

    //! The writers in the group.
    QList<QSharedPointer<Writer> > childWriters_;
};

#endif // WriterGROUP_H
