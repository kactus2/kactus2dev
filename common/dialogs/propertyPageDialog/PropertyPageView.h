//-----------------------------------------------------------------------------
// File: PropertyPageView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 8.6.2011
//
// Description:
// Base class for property page views.
//-----------------------------------------------------------------------------

#ifndef PROPERTYPAGEVIEW_H
#define PROPERTYPAGEVIEW_H

#include <QWidget>

//-----------------------------------------------------------------------------
//! PropertyPageView class.
//-----------------------------------------------------------------------------
class PropertyPageView : public QWidget
{
    Q_OBJECT

public:
    /*!
     *  Constructor.
     */
    PropertyPageView();

    /*!
     *  Destructor.
     */
    virtual ~PropertyPageView();

    /*!
     *  Pre-validates the contents of the page. This is used for enabling/disabling the OK button.
     *
     *      @return True, if the contents are valid and OK button should be enabled.
     *              False, if the contents are invalid and OK button should be disabled.
     *
     *      @remarks Must not show any message boxes.
     */
    virtual bool prevalidate() const;

    /*!
     *  Validates the contents of the page thoroughly.
     *
     *      @return True, if the contents are valid. False, if they are invalid.
     *
     *      @remarks Showing message boxes for errors is allowed.
     */
    virtual bool validate();

    /*!
     *  Applies the changes that were done in the page.
     */
    virtual void apply();

    /*!
     *  Called when the page is to be changed and this page would be hidden.
     *
     *      @return False, if the page change should be rejected. Otherwise true.
     */
    virtual bool onPageChange();

signals:
    void contentChanged();

private:

	//! \brief No copying
	PropertyPageView(const PropertyPageView& other);

	//! \brief No assignment
	PropertyPageView& operator=(const PropertyPageView& other);
};

//-----------------------------------------------------------------------------

#endif // PROPERTYPAGEVIEW_H
