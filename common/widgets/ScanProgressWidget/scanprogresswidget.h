//-----------------------------------------------------------------------------
// File: scanprogresswidget.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 08.02.2012
//
// Description:
// Displays a progress bar and a message of what is being scanned.
//-----------------------------------------------------------------------------
#ifndef SCANPROGRESSWIDGET_H
#define SCANPROGRESSWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QProgressBar>
#include <QString>
#include <QWidget>

//-----------------------------------------------------------------------------
//! Displays a progress bar and a message of what is being scanned.
//-----------------------------------------------------------------------------
class ScanProgressWidget : public QDialog
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent  The owner of this widget.
	*/
	ScanProgressWidget(QWidget *parent = 0);
	
	//! The destructor
	virtual ~ScanProgressWidget();

	/*! Set the range of the progress bar.
	 *
	 *      @param [in] min     The minimum value to display.
	 *      @param [in] max     The maximum value to display.
	*/
	void setRange(int min, int max);

	/*! Set the scan message.
	 *
	 *      @param [in] message     The message to be displayed.
	*/
	void setMessage(const QString& message);

	/*! Set current value for the progress bar.
	 *
	 *      @param [in] value   The value to set.
	*/
	void setValue(int value);

private:

	//! No copying
	ScanProgressWidget(const ScanProgressWidget& other);

	//! No assignment
	ScanProgressWidget& operator=(const ScanProgressWidget& other);

	//! The vertical bar to display the progress of scan.
	QProgressBar progressBar_;

	//! Contains the currently scanned message.
	QLabel messageLabel_;
};

#endif // SCANPROGRESSWIDGET_H
