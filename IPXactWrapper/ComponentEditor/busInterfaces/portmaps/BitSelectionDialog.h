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
#include <QLabel>
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
	 *      @param [in] logicalPort         Logical port name.
	 *      @param [in] logicalBeginIndex   First logical bit to map.
	 *      @param [in] physicalPort        Physical port name.
	 *      @param [in] portSize            Physical port actual size.
	 *      @param [in] maxBitsToMap        Maximum number of bits that can be mapped to logical.
	 *      @param [in] parent              The parent widget.
	 */
	BitSelectionDialog(QString logicalPort, int logicalBeginIndex, QString physicalPort, 
        int portSize, int maxBitsToMap, QWidget *parent = 0);

	/*!
	 *  The destructor.
	*/
	~BitSelectionDialog();

    /*!
     *  Gets the selected higher bound.
     *
     *      @return The higher bound.
     */
    int getHigherBound() const;

    /*!
     *  Gets the selected lower bound.
     *
     *      @return The lower bound.
     */
    int getLowerBound() const;
	
private slots:

    /*!
     *  Called when the user changes left index.
     */
    virtual void onHigherBoundChanged();

    /*!
     *  Called when the user changes right index.
     */
    virtual void onLowerBoundChanged();

private:
	// Disable copying.
	BitSelectionDialog(BitSelectionDialog const& rhs);

	// Disable assignment.
	BitSelectionDialog& operator=(BitSelectionDialog const& rhs);

	// Setup initial dialog layout.
    void setupLayout();

    void updateLogicalBits();

	//-----------------------------------------------------------------------------
	// Data.
	//-----------------------------------------------------------------------------

    //! Label for logical name.
    QLabel* logicalNameLabel_;

    //! Label for physical name.
    QLabel* physicalNameLabel_;

    //! Editor for displaying higher logical bound.
    QSpinBox* higherLogicalBox_;

    //! Editor for displaying lower logical bound.
    QSpinBox* lowerLogicalBox_;

    //! Editor for higher physical bound.
    QSpinBox* higherPhysicalBox_;

    //! Editor for lower physical bound.
    QSpinBox* lowerPhysicalBox_;

    //! Dialog buttons.
    QDialogButtonBox* buttonBox_;

    //! Size of the physical port.
    int physPortSize_;

    //! Maximum size of the physical port.
    int maxPhysPortSize_;

};
#endif // BITSELECTIONDIALOG_H

