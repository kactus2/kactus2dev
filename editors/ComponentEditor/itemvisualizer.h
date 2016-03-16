//-----------------------------------------------------------------------------
// File: itemvisualizer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 22.05.2012
//
// Description:
// The base class for all IP-Xact visualizer classes.
//-----------------------------------------------------------------------------

#ifndef ITEMVISUALIZER_H
#define ITEMVISUALIZER_H

#include <QWidget>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! The base class for all IP-Xact visualizer classes.
//-----------------------------------------------------------------------------
class ItemVisualizer : public QWidget
{
	Q_OBJECT

public:

	/*! The constructor
	 *
	 *      @param [in] parent  The parent widget of the visualizer. 
	*/
	ItemVisualizer(QWidget* parent = 0);
	
	//! The destructor
	virtual ~ItemVisualizer();

signals:

    //! Emitted when the visualizer is shown.
    void displayed();

private:
	//! No copying. No assignment. No regret.
	ItemVisualizer(const ItemVisualizer& other);
	ItemVisualizer& operator=(const ItemVisualizer& other);
};

#endif // ITEMVISUALIZER_H
