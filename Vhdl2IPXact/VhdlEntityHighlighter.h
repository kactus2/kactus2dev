//-----------------------------------------------------------------------------
// File: VhdlEntityHighlighter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 13.06.2013
//
// Description:
// Highlights the entity section in an vhdl text.
//-----------------------------------------------------------------------------
#ifndef VHDLENTITYHIGHLIGHTER_H
#define VHDLENTITYHIGHLIGHTER_H

#include <QTextEdit>
#include <QMouseEvent>
#include <QSyntaxHighlighter>
#include <QList>
#include <QMap>
#include <QRegExp>
#include <QTextCharFormat>

//-----------------------------------------------------------------------------
//! Class VhdlEntityHighlighter.
//-----------------------------------------------------------------------------
class VhdlEntityHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:

    enum BlockState{
        OUTSIDE_ENTITY = -1,
        INSIDE_ENTITY,
        PORT_SELECTED,
        PORT_NOT_SELECTED,
        GENERIC_SELECTED,
        GENERIC_NOT_SELECTED
    };

    VhdlEntityHighlighter(QTextDocument* document = 0);

    ~VhdlEntityHighlighter();

protected:

    void highlightBlock(const QString & text);

private:

    QTextCharFormat insideEntity_;

    QTextCharFormat outsideEntity_;

    QTextCharFormat selectedPort_;

    QTextCharFormat selectedGeneric_;

    QTextCharFormat notSelected_;
};

#endif // VHDLENTITYHIGHLIGHTER_H
