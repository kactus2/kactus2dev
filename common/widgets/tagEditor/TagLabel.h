//-----------------------------------------------------------------------------
// File: TagLabel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.08.2019
//
// Description:
// The tag label.
//-----------------------------------------------------------------------------

#ifndef TAGLABEL_H
#define TAGLABEL_H

#include <QLabel>

//-----------------------------------------------------------------------------
//! The tag label.
//-----------------------------------------------------------------------------
class TagLabel : public QLabel
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] tagName     Name of the tag.
     *      @param [in] parent      The parent widget.
     *      @param [in] labelColor  Color of the tag.
     */
    TagLabel(QString const& tagName, QWidget* parent = 0, QString const& labelColor = QLatin1String("#DAE1DF"));

    /*!
     *  The destructor.
     */
    virtual ~TagLabel() = default;

signals:

    /*!
     *  Signal for informing tag label click.
     *
     *      @param [in] labelItem   This label.
     */
    void clicked(TagLabel* labelItem);

protected:

    /*!
     *  Event handling for mouse release.
     *
     *      @param [in] ev  The mouse event.
     */
    virtual void mouseReleaseEvent(QMouseEvent *ev) final;

private:
    // Disable copying.
    TagLabel(TagLabel const& rhs);
    TagLabel& operator=(TagLabel const& rhs);
};

//-----------------------------------------------------------------------------

#endif // TAGLABEL_H
