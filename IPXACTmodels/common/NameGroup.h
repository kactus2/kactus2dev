//-----------------------------------------------------------------------------
// File: NameGroup.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 03.08.2015
//
// Description:
// Implementation for ipxact:nameGroup element.
//-----------------------------------------------------------------------------

#ifndef NAMEGROUP_H
#define NAMEGROUP_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Implementation for ipxact:nameGroup element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT NameGroup
{

public:

	//! The default constructor	
	NameGroup() = default;

	/*! The constructor
	 *
	 *      @param [in] name            The initial name.
     *      @param [in] displayName     The initial displayName.
     *      @param [in] description     The initial description.
	*/
	NameGroup(QString const& name, QString const& displayName = QString(), QString const& description = QString());


    /*! The constructor
     *
     *      @param [in] name            The initial name.
     *      @param [in] displayName     The initial displayName.
     *      @param [in] description     The initial description.
    */
    NameGroup(std::string_view name, std::string_view displayName = std::string(), 
        std::string_view description = std::string());

	/*! The copy constructor
	 *
	 *      @param [in] other    Reference to the NameGroup to copy.
	*/
	NameGroup(const NameGroup& other) = default;

    //! The destructor.
    virtual ~NameGroup() = default;

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
     *  Gets the name element in the name group.
     *
     *      @return The name element.
     */
    std::string nameStd() const;

    /*!
     *  Sets the name element in the group.
     *
     *      @param [in] name   The name to set.     
     */
    void setName(QString const& name);

    /*!
     *  Sets the name element in the group.
     *
     *      @param [in] name   The name to set.
     */
    void setName(std::string_view name);

    /*!
     *  Gets the displayName element in the name group.
     *
     *      @return The displayName element.
     */
    QString displayName() const;

    /*!
     *  Gets the displayName element in the name group.
     *
     *      @return The displayName element.
     */
    std::string displayNameStd() const;

    /*!
     *  Sets the displayName element in the group.
     *
     *      @param [in] displayName   The display name to set.     
     */
    void setDisplayName(QString const& displayName);

    void setDisplayName(std::string_view displayName);

    /*!
     *  Gets the short description element in the name group.
     *
     *      @return The short description element.
     */
    QString shortDescription() const;

    /*!
     *  Gets the displayName element in the name group.
     *
     *      @return The displayName element.
     */
    std::string shortDescriptionStd() const;

    /*!
     *  Sets the short description element in the group.
     *
     *      @param [in] description   The description to set.
     */
    void setShortDescription(QString const& description);

    void setShortDescription(std::string_view description);
    /*!
     *  Gets the description element in the name group.
     *
     *      @return The description element.
     */
    QString description() const;

    /*!
     *  Gets the description element in the name group.
     *
     *      @return The description element.
     */
    std::string descriptionStd() const;

    /*!
     *  Sets the description element in the group.
     *
     *      @param [in] description   The description to set.     
     */
    void setDescription(QString const& description);

    void setDescription(std::string_view description);

private:

    //! The name matches the ipxact:name field within nameGroup.
    std::string name_;

    //! The displayName matches the ipxact:displayName field within nameGroup.
    std::string displayName_;

    //! The shortDescription matches the ipxact:shortDescription field within nameGroup.
    std::string shortDescription_;

    //! description matches the ipxact:description field within nameGroup.
    std::string description_;
};

#endif /* NAMEGROUP_H */