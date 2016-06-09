//-----------------------------------------------------------------------------
// File: PortMapHeaderView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.06.2016
//
// Description:
// Header view for port map editor.
//-----------------------------------------------------------------------------

#ifndef PORTMAPHEADERVIEW_H
#define PORTMAPHEADERVIEW_H

#include <QHeaderView>

//-----------------------------------------------------------------------------
//! Header view for port map editor.
//-----------------------------------------------------------------------------
class PortMapHeaderView : public QHeaderView
{

public:
    /*!
     *  The constructor.
     *
     *      @param [in] orientation     The headers orientation.
     *      @param [in] parent          The parent of the header.
     */
    PortMapHeaderView(Qt::Orientation orientation, QWidget* parent = 0);

    /*!
     *  The destructor.
     */
    virtual ~PortMapHeaderView();

protected:
    
    /*!
     *  The re-implemented paint section.
     *
     *      @param [in] painter         The given painter.
     *      @param [in] rect            The given rectangle.
     *      @param [in] logicalIndex    Specifies the section.
     */
    virtual void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const;

private:

    //! Disable copying and assignment
    PortMapHeaderView(const PortMapHeaderView& other);
    PortMapHeaderView& operator=(const PortMapHeaderView& other);
};

#endif // PORTMAPHEADERVIEW_H