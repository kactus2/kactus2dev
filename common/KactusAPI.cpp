//-----------------------------------------------------------------------------
// File: KactusAPI.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.02.2021
//
// Description:
// Defines the operations available for accessing Kactus2 core.
//-----------------------------------------------------------------------------

#include "KactusAPI.h"

#include <VersionHelper.h>
#include <common/ui/ConsoleMediator.h>

LibraryInterface* KactusAPI::library_ = nullptr;
MessageMediator* KactusAPI::messageChannel_ = new ConsoleMediator();

//-----------------------------------------------------------------------------
// Function: KactusAPI::KactusAPI()
//-----------------------------------------------------------------------------
KactusAPI::KactusAPI(LibraryInterface* library, MessageMediator* messageChannel)
{
    if (library_ == nullptr)
    {
        library_ = library;        
    }

    if (messageChannel != nullptr)
    {
        delete messageChannel_;
        messageChannel_ = messageChannel;
    }
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getLibrary()
//-----------------------------------------------------------------------------
LibraryInterface* KactusAPI::getLibrary()
{
    Q_ASSERT(library_ != nullptr);

    return library_;
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getMessageChannel()
//-----------------------------------------------------------------------------
MessageMediator* KactusAPI::getMessageChannel()
{
    return messageChannel_;
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getVersion()
//-----------------------------------------------------------------------------
std::string KactusAPI::getVersion()
{
    return "Kactus2 " + VersionHelper::versionFull().toStdString();
}
