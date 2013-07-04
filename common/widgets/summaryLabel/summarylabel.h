/* 
 *  	Created on: 31.5.2012
 *      Author: Antti Kamppi
 * 		filename: summarylabel.h
 *		Project: Kactus 2
 */

#ifndef SUMMARYLABEL_H
#define SUMMARYLABEL_H

#include <QLabel>

/*! \brief The label used to display summary headers in component editor.
 *
 */
class SummaryLabel : public QLabel {
	Q_OBJECT

public:

	//! \brief The increment amount for normal font point size.
	static const int POINTSIZE_INCREMENT = 0;

	/*! \brief The constructor
	 *
	 * \param text The text to display in the label.
	 * \param parent Pointer to the owner of this label.
	 *
	*/
	SummaryLabel(const QString& text, QWidget *parent);
	
	//! \brief The destructor
	virtual ~SummaryLabel();

private:
	//! \brief No copying
	SummaryLabel(const SummaryLabel& other);

	//! \brief No assignment
	SummaryLabel& operator=(const SummaryLabel& other);
};

#endif // SUMMARYLABEL_H
