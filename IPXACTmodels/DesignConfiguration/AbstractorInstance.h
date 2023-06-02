//-----------------------------------------------------------------------------
// File: AbstractorInstance.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 04.08.2015
//
// Description:
// Matches the ipxact:abstractorInstance element.
//-----------------------------------------------------------------------------

#ifndef ABSTRACTORINSTANCE_H
#define ABSTRACTORINSTANCE_H

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/Extendable.h>

#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Matches the ipxact:abstractorInstance element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AbstractorInstance : public Extendable
{
public:

    /*!
     *  The constructor.
     */
    AbstractorInstance();

    /*!
     *  Copy constructor.
     *
     *      @param [in] other   Target to copy.
     */
    AbstractorInstance(const AbstractorInstance& other);

    /*!
     *  The destructor.
     */
    ~AbstractorInstance() final = default;

    /*!
     *  Get the VLNV reference to the abstractor.
     *
     *      @return The configurable VLNV of the abstractor.
     */
    QSharedPointer<ConfigurableVLNVReference> getAbstractorRef() const;

    /*!
     *  Get the description.
     *
     *      @return The description.
     */
    std::string getShortDescription() const;

    /*!
	 *  Get the description.
	 *
     *      @return The description.
	 */
	std::string getDescription() const;

    /*!
	 *  Get the display name.
	 *
     *      @return The display name of the abstractor instance.
	 */
	std::string getDisplayName() const;

    /*!
	 *  Get the instance name.
	 *
     *      @return The name of this instance.
	 */
	std::string getInstanceName() const;

    /*!
	 *  Get the name of the view associated with this instance.
	 *
     *      @return The selected view name of the instance.
	 */
	std::string getViewName() const;

    /*!
     *  Set the abstractor reference.
     *
     *      @param [in] abstractorRef   A VLNV tag of the abstractor reference.
     */
    void setAbstractorRef(QSharedPointer<ConfigurableVLNVReference> abstractorRef);

    /*!
     *  Set the short description.
     *
     *      @param [in] shortDescription     The new short description.
     */
    void setShortDescription(std::string const& shortDescription);

    /*!
	 *  Set the description.
	 *
	 *      @param [in] description     The new description.
	 */
	void setDescription(std::string const& description);

    /*!
	 *  Set the display name.
	 *
	 *      @param [in] displayName     The new display name.
	 */
	void setDisplayName(std::string const& displayName);

    /*!
	 *  Set the name for the instance.
	 *
	 *      @param [in] instanceName    The new instance name.
	 */
	void setInstanceName(std::string const& instanceName);

    /*!
	 *  Set the name of the selected view.
	 *
	 *      @param [in] viewName    The name of the selected view.
	 */
	void setViewName(std::string const& viewName);

private:

    //! The name of this instance.
	std::string instanceName_;

    //! Descriptive text of the instance.
	std::string displayName_;

    //! The short description of the instance.
    std::string shortDescription_;

    //! Textual description of the instance.
	std::string description_;

    //! Configurable VLNV of the abstractor reference.
    QSharedPointer<ConfigurableVLNVReference> abstractorRef_ =
        QSharedPointer<ConfigurableVLNVReference>(new ConfigurableVLNVReference());

    //! Defines the selected view.
	std::string viewName_;
};

#endif // ABSTRACTORINSTANCE_H