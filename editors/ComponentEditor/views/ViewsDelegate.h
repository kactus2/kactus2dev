//-----------------------------------------------------------------------------
// File: ViewsDelegate.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.07.2015
//
// Description:
// The delegate that provides editors to edit the views summary.
//-----------------------------------------------------------------------------

#ifndef VIEWSDELEGATE_H
#define VIEWSDELEGATE_H

#include <editors/ComponentEditor/common/MultilineDescriptionDelegate.h>

#include <QEvent>

//-----------------------------------------------------------------------------
//! The delegate that provides editors to edit the views summary.
//-----------------------------------------------------------------------------
class ViewsDelegate : public MultilineDescriptionDelegate 
{
	Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent      The parent object.
     */
	ViewsDelegate(QObject* parent);
	
	//! The destructor
	virtual ~ViewsDelegate();

protected:

    // Gets the column for description.
    virtual int descriptionColumn() const;

private:
	//! No copying
	ViewsDelegate(const ViewsDelegate& other);
	ViewsDelegate& operator=(const ViewsDelegate& other);

};

#endif // VIEWSDELEGATE_H
