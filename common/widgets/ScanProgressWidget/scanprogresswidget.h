/* 
 *  	Created on: 8.2.2012
 *      Author: Antti Kamppi
 * 		filename: scanprogresswidget.h
 *		Project: Kactus 2
 */

#ifndef SCANPROGRESSWIDGET_H
#define SCANPROGRESSWIDGET_H

#include <QWidget>
#include <QDialog>
#include <QProgressBar>
#include <QLabel>
#include <QString>

/*! \brief Displays a progress bar and a message of what is being scanned.
 *
 */
class ScanProgressWidget : public QDialog {
	Q_OBJECT

public:

	/*! \brief The constructor
	 *
	 * \param parent Pointer to the owner of this widget.
	 *
	*/
	ScanProgressWidget(QWidget *parent = 0);
	
	//! \brief The destructor
	virtual ~ScanProgressWidget();

	/*! \brief Set the range of the progress bar.
	 *
	 * \param min The minimum value to display.
	 * \param max The maximum value to display.
	 *
	*/
	void setRange(int min, int max);

	/*! \brief Set the scan message.
	 *
	 * \param message The message to be displayed.
	 *
	*/
	void setMessage(const QString& message);

	/*! \brief Set current value for the progress bar.
	 *
	 * \param value The value to set.
	 *
	*/
	void setValue(int value);

private:

	//! \brief No copying
	ScanProgressWidget(const ScanProgressWidget& other);

	//! \brief No assignment
	ScanProgressWidget& operator=(const ScanProgressWidget& other);

	//! \brief The vertical bar to display the progress of scan.
	QProgressBar progBar_;

	//! \brief Contains the currently scanned message.
	QLabel messageLabel_;
};

#endif // SCANPROGRESSWIDGET_H
