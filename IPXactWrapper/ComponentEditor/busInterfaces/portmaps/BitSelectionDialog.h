//-----------------------------------------------------------------------------
// File: BitSelectionDialog.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 3.10.2013
//
// Description:
// Dialog for selecting bit indexes of physical port.
//-----------------------------------------------------------------------------

#include <QDialog>
#include <QSpinBox>
#include <QDialogButtonBox>

#ifndef BITSELECTIONDIALOG_H
#define BITSELECTIONDIALOG_H

//-----------------------------------------------------------------------------
// class BitSelectionDialog.
//-----------------------------------------------------------------------------
class BitSelectionDialog : public QDialog
{
	Q_OBJECT

public:
	/*!
	 *  The constructor.
	 *
	 *      @param [in] parent   The parent object.
	 */
	BitSelectionDialog(int portSize, int portMaxSize, QWidget *parent = 0);

	/*!
	 *  The destructor.
	*/
	~BitSelectionDialog();

    int getLeft() const;

    int getRight() const;

protected:
	
private slots:

    /*!
     *  Called when the user changes left index.
     */
    virtual void onLeftChanged();

    /*!
     *  Called when the user changes right index.
     */
    virtual void onRightChanged();

private:
	// Disable copying.
	BitSelectionDialog(BitSelectionDialog const& rhs);

	// Disable assignment.
	BitSelectionDialog& operator=(BitSelectionDialog const& rhs);

	// Setup initial dialog layout.
    void setupLayout();

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    QSpinBox* leftBox_;

    QSpinBox* rightBox_;

    QDialogButtonBox* buttonBox_;

    int portSize_;

    int maxSize_;
};
#endif // BITSELECTIONDIALOG_H

