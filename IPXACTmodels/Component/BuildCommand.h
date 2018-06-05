//-----------------------------------------------------------------------------
// File: BuildCommand.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.09.2015
//
// Description:
// Describes the ipxact:buildCommand element.
//-----------------------------------------------------------------------------

#ifndef BUILDCOMMAND_H
#define BUILDCOMMAND_H

#include <IPXACTmodels/common/BuildModel.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/BooleanValue.h>

#include <QString>
#include <QMap>
//-----------------------------------------------------------------------------
//! Contains flags or commands for building the containing source file types.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BuildCommand : public BuildModel
{

public:

    //! Default constructor.
	BuildCommand();

	//! The copy constructor.
	BuildCommand(const BuildCommand &other);

	//! The assignment operator.
	BuildCommand &operator=(const BuildCommand &other);

    //! The destructor.
	virtual ~BuildCommand();

    /*!
     *  Get the target name.
     *
     *      @return The target name.
     */
    QString getTargetName() const;

	/*!
	 *  Set the target name.
	 *
	 *      @param [in] targetName  The new name of the target.
	 */
	void setTargetName(const QString &targetName);

    /*!
     *  Get the flags append value.
     *
     *      @return QString containing true for boolean true, false for false and empty for an unspecified.
     */
    QString getFlagsAppend() const;

    /*!
     *  Set a new boolean value for the flags append.
     *
     *      @param [in] newAppend   The new flags append value.
     */
    void setFlagsAppend(bool newAppend);

    /*!
     *  Set the flags append to unspecified.
     */
    void clearFlagsAppend();

private:

    //! Defines the path to the file derived from the source file.
	QString targetName_;

    //! Indicates whether the flags shall be appended to the current flags or replace them.
    BooleanValue flagsAppend_;
};

#endif // BUILDCOMMAND_H
