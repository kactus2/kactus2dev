//-----------------------------------------------------------------------------
// File: CompleterPopupTable.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 20.01.2015
//
// Description:
// Popup table.
//-----------------------------------------------------------------------------

#ifndef COMPLETERPOPUPTABLE_H
#define COMPLETERPOPUPTABLE_H

#include <QTableView>

//-----------------------------------------------------------------------------
//! Table for displaying parameter information.
//-----------------------------------------------------------------------------
class CompleterPopupTable : public QTableView
{
    Q_OBJECT
public:

	//! The constructor.
	CompleterPopupTable(QWidget *parent = 0);
	
    //! The destructor.
    ~CompleterPopupTable();

protected:

    /*!
     *  Re-implemented event handler for the popup tables show event.
     *
     *      @param [in] event   The show event.
     */
    virtual void showEvent(QShowEvent *event);

signals:

    /*!
     *  Reset the model.
     */
    void resetParameterModel();

private:
	// Disable copying.
    CompleterPopupTable(CompleterPopupTable const& rhs);
    CompleterPopupTable& operator=(CompleterPopupTable const& rhs);
};

#endif // COMPLETERPOPUPTABLE_H
