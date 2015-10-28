//-----------------------------------------------------------------------------
// File: BuildModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Base class for file builder.
//-----------------------------------------------------------------------------

#include "BuildModel.h"

//-----------------------------------------------------------------------------
// Function: BuildModel::BuildModel()
//-----------------------------------------------------------------------------
BuildModel::BuildModel() :
command_(),
flags_(),
replaceDefaultFlags_()
{

}

//-----------------------------------------------------------------------------
// Function: BuildModel::BuildModel()
//-----------------------------------------------------------------------------
BuildModel::BuildModel( const BuildModel &other ):
command_(other.command_),
flags_(other.flags_),
replaceDefaultFlags_(other.replaceDefaultFlags_)
{

}

//-----------------------------------------------------------------------------
// Function: BuildModel::operator=()
//-----------------------------------------------------------------------------
BuildModel & BuildModel::operator=( const BuildModel &other )
{
	if (this != &other)
    {
		command_ = other.command_;
		flags_ = other.flags_;
		replaceDefaultFlags_ = other.replaceDefaultFlags_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: BuildModel::~BuildModel()
//-----------------------------------------------------------------------------
BuildModel::~BuildModel()
{

}

//-----------------------------------------------------------------------------
// Function: BuildModel::getCommand()
//-----------------------------------------------------------------------------
QString BuildModel::getCommand() const
{
    return command_;
}

//-----------------------------------------------------------------------------
// Function: BuildModel::getFlags()
//-----------------------------------------------------------------------------
QString BuildModel::getFlags() const
{
    return flags_;
}

//-----------------------------------------------------------------------------
// Function: BuildModel::getReplaceDefaultFlags()
//-----------------------------------------------------------------------------
QString BuildModel::getReplaceDefaultFlags() const
{
    return replaceDefaultFlags_;
}

//-----------------------------------------------------------------------------
// Function: BuildModel::setCommand()
//-----------------------------------------------------------------------------
void BuildModel::setCommand(const QString &command)
{
    command_ = command;
}

//-----------------------------------------------------------------------------
// Function: BuildModel::setFlags()
//-----------------------------------------------------------------------------
void BuildModel::setFlags(const QString &flags)
{
    flags_ = flags;
}

//-----------------------------------------------------------------------------
// Function: BuildModel::setReplaceDefaultFlags()
//-----------------------------------------------------------------------------
void BuildModel::setReplaceDefaultFlags(QString const& newReplaceDefaultFlags)
{
    replaceDefaultFlags_ = newReplaceDefaultFlags;
}
