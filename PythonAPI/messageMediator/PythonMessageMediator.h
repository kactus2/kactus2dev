//-----------------------------------------------------------------------------
// File: PythonMessageMediator.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 12.02.2020
//
// Description:
// Output for user messages in python API.
//-----------------------------------------------------------------------------

#ifndef PYTHONMESSAGEMEDIATOR_H
#define PYTHONMESSAGEMEDIATOR_H

#include <common/ui/MessageMediator.h>

//-----------------------------------------------------------------------------
//! Output for user messages in python API.
//-----------------------------------------------------------------------------
class PythonMessageMediator : public MessageMediator
{
public:

    //! The constructor.
    explicit PythonMessageMediator();

    //! The destructor.
    virtual ~PythonMessageMediator() = default;
        
    //! Show the given message to the user.
    virtual void showMessage(QString const& message) const override final;
    
    //! Show the given error to the user.
    virtual void showError(QString const& error) const override final;

    //! Show the given failure message to the user.
    virtual void showFailure(QString const& error) const override final;

    //! Show the given status to the user.
    virtual void showStatusMessage(QString const& status) const override final;

private:

    // Disable copying.
    PythonMessageMediator(PythonMessageMediator const& rhs);
    PythonMessageMediator& operator=(PythonMessageMediator const& rhs);
};

#endif // CONSOLEMEDIATOR_H
