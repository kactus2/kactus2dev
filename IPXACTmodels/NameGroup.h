//-----------------------------------------------------------------------------
// File: NameGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 9.7.2014
//
// Description:
// NameGroup matches the spirit:nameGroup element in IP-Xact.
//-----------------------------------------------------------------------------

#ifndef NAMEGROUP_H
#define NAMEGROUP_H

#include <QDomNode>
#include <QString>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! NameGroup matches the spirit:nameGroup element in IP-Xact.
//-----------------------------------------------------------------------------
class NameGroup
{

public:

	//! The default constructor	
	NameGroup();

	/*! The constructor
	*		
	*      @param [in] nameNode Reference to the QDomNode to parse the information from.
	*/
	NameGroup(QDomNode& node);

	/*! The constructor
	 *
	 *      @param [in] name            The initial name.
     *      @param [in] displayName     The initial displayName.
     *      @param [in] description     The initial description.
	*/
	NameGroup(QString const& name, QString const& displayName = QString(), QString const& description = QString());

	/*! The copy constructor
	 *
	 *      @param [in] other    Reference to the NameGroup to copy.
	*/
	NameGroup(const NameGroup& other);

	/*! The assignment operator
	 *
	 *      @param [in] other   Reference to the NameGroup to assign.
	 *
	 *      @return Reference to this NameGroup.
	*/
	NameGroup& operator=(const NameGroup& other);

    /*!
     *  Gets the name element in the name group.
     *
     *      @return The name element.
     */
    QString name() const;

    /*!
     *  Sets the name element in the group.
     *
     *      @param [in] name   The name to set.     
     */
    void setName(QString const& name);

    /*!
     *  Gets the displayName element in the name group.
     *
     *      @return The displayName element.
     */
    QString displayName() const;

    /*!
     *  Sets the displayName element in the group.
     *
     *      @param [in] displayName   The display name to set.     
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Gets the description element in the name group.
     *
     *      @return The description element.
     */
    QString description() const;

    /*!
     *  Sets the description element in the group.
     *
     *      @param [in] description   The description to set.     
     */
    void setDescription(QString const& description);

    /*!
     *  Writes the NameGroup to XML.
     *
     *      @param [in] writer   The writer used for writing the XML.
     */
    void write(QXmlStreamWriter& writer) const;

private:

    //! The name matches the spirit:name field within nameGroup.
    QString name_;

    //! The displayName matches the spirit:displayName field within nameGroup.
    QString displayName_;

    //! description matches the spirit:description field within nameGroup.
    QString description_;
};

#endif /* NAMEGROUP_H */