//-----------------------------------------------------------------------------
// File: BuildModel.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 08.09.2015
//
// Description:
// Base class for file builder.
//-----------------------------------------------------------------------------

#ifndef BUILDMODEL_H
#define BUILDMODEL_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QMap>
#include <QString>

//-----------------------------------------------------------------------------
//! Base class for file builder.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT BuildModel
{

public:

	/*!
	 *  The constructor.
	 */
	BuildModel();

	/*!
	 *  Copy constructor.
	 */
	BuildModel(const BuildModel &other);

	/*!
	 *  Assignment operator.
	 */
	BuildModel &operator=(const BuildModel &other);

	/*!
	 *  The destructor.
	 */
	virtual ~BuildModel();

    /*!
     *  Get the command.
     *
     *      @return The command.
     */
    QString getCommand() const;

    /*!
     *  Get the flags.
     *
     *      @return The flags.
     */
    QString getFlags() const;

    /*!
     *  Get the replace default flags expression.
     *
     *      @return An expression determining should the default flags be replaced or not.
     */
    QString getReplaceDefaultFlags() const;

    /*!
     *  Set the command.
     *
     *      @param [in] newCommand  The new command.
     */
    void setCommand(QString const& newCommand);

    /*!
     *  Set the flags.
     *
     *      @param [in] newFlags    The new flags.
     */
    void setFlags(QString const& newFlags);

    /*!
     *  Set the expression for default flags replacement.
     *
     *      @param [in] newReplaceDefaultFlags  The new replacing expression.
     */
    void setReplaceDefaultFlags(QString const& newReplaceDefaultFlags);

private:

    //! Defines a compiler or assembler tool for processing files.
	QString command_;

    //! Documents flags to be passed along with the software tool.
	QString flags_;

    //! Indicates the replacing of default flags.
    QString replaceDefaultFlags_;
};

#endif // BUILDMODEL_H
