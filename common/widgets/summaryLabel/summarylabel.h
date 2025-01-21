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
	static const int POINTSIZE_INCREMENT = 6;

	/*!
	 * The constructor
	 *
	 *    @param [in] text                 The text to display in the label.
	 *    @param [in] parent               The owner of this label.
	 *    @param [in] showExtensionIcon    If true, an extension icon is shown next to the label.
	 */
	 SummaryLabel(const QString& text, QWidget *parent, bool showExtensionIcon = false);
	
	//! \brief The destructor
	virtual ~SummaryLabel() = default;

    //! \brief No copying
    SummaryLabel(const SummaryLabel& other) = delete;

    //! \brief No assignment
    SummaryLabel& operator=(const SummaryLabel& other) = delete;


};

#endif // SUMMARYLABEL_H
