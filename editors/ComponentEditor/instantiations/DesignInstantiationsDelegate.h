//-----------------------------------------------------------------------------
// File: DesignInstantiationsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 29.01.2016
//
// Description:
// The delegate that provides editors to edit the design instantiations summary.
//-----------------------------------------------------------------------------

#ifndef DESIGNINSTANTIATIONSDELEGATE_H
#define DESIGNINSTANTIATIONSDELEGATE_H

#include <editors/ComponentEditor/common/MultilineDescriptionDelegate.h>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit the design instantiations summary.
//-----------------------------------------------------------------------------
class DesignInstantiationsDelegate : public MultilineDescriptionDelegate 
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent      The parent object.
     */
	DesignInstantiationsDelegate(QObject* parent);
	
	//! The destructor
	virtual ~DesignInstantiationsDelegate();

protected:

    // Gets the column for description.
    virtual int descriptionColumn() const;

private:
	//! No copying
	DesignInstantiationsDelegate(const DesignInstantiationsDelegate& other);
	DesignInstantiationsDelegate& operator=(const DesignInstantiationsDelegate& other);

};

#endif // DESIGNINSTANTIATIONSDELEGATE_H
