//-----------------------------------------------------------------------------
// File: ScriptingHistory.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 01.03.2021
//
// Description:
// A container for script command history.
//-----------------------------------------------------------------------------


#include "ScriptingHistory.h"

//-----------------------------------------------------------------------------
// Function: ScriptingHistory::ScriptingHistory()
//-----------------------------------------------------------------------------
ScriptingHistory::ScriptingHistory(QObject* parent /*= nullptr*/) :
QObject(parent),
history_(),
historyPosition_(0)
{

}

//-----------------------------------------------------------------------------
// Function: ScriptingHistory::push()
//-----------------------------------------------------------------------------
void ScriptingHistory::push(QString const& line)
{
   if (line.isEmpty())
   {
       return;
   }

   if ((history_.length() == 0 || history_.at(history_.length() - 1) != line))
   {
       history_.append(line);
       historyPosition_ = history_.length();
       emit commandAdded(line);
   }
}

//-----------------------------------------------------------------------------
// Function: ScriptingHistory::previous()
//-----------------------------------------------------------------------------
QString ScriptingHistory::previous()
{
    if (historyPosition_ > 0)
    {
        --historyPosition_;
    }

    if (historyPosition_ < history_.length())
    {
        return history_.at(historyPosition_);
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ScriptingHistory::next()
//-----------------------------------------------------------------------------
QString ScriptingHistory::next()
{
    if (historyPosition_ < history_.length() - 1)
    {
        ++historyPosition_;
    }

    if (historyPosition_ < history_.length())
    {
        return history_.at(historyPosition_);
    }

    return QString();
}
