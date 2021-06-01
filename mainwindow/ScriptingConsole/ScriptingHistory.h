//-----------------------------------------------------------------------------
// File: ScriptingHistory.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 01.02.2021
//
// Description:
// A container for script command history.
//-----------------------------------------------------------------------------

#ifndef SCRIPTING_HISTORY_H
#define SCRIPTING_HISTORY_H

#include <QObject>

#include <QString>

//-----------------------------------------------------------------------------
//! A container for script command history.
//-----------------------------------------------------------------------------
class ScriptingHistory : public QObject
{
    Q_OBJECT
public:

    //! The constructor.
    explicit ScriptingHistory(QObject* parent = nullptr);

    virtual ~ScriptingHistory() = default;

    void push(QString const& line);

    QString previous();

    QString next();

signals:
    
    void commandAdded(QString const& command);

private:

    //! The scripting editor for which to show side information.
    QStringList history_;

    int historyPosition_;
};

#endif // SCRIPTING_HISTORY_H
