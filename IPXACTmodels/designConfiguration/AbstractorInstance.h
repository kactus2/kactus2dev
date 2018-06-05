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

#include <QString>
#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Matches the ipxact:abstractorInstance element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT AbstractorInstance
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
    ~AbstractorInstance();

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
	QString getDescription() const;

    /*!
	 *  Get the display name.
	 *
     *      @return The display name of the abstractor instance.
	 */
	QString getDisplayName() const;

    /*!
	 *  Get the instance name.
	 *
     *      @return The name of this instance.
	 */
	QString getInstanceName() const;

    /*!
	 *  Get the name of the view associated with this instance.
	 *
     *      @return The selected view name of the instance.
	 */
	QString getViewName() const;

    /*!
     *  Set the abstractor reference.
     *
     *      @param [in] abstractorRef   A VLNV tag of the abstractor reference.
     */
    void setAbstractorRef(QSharedPointer<ConfigurableVLNVReference> abstractorRef);

    /*!
	 *  Set the description.
	 *
	 *      @param [in] description     The new description.
	 */
	void setDescription(const QString& description);

    /*!
	 *  Set the display name.
	 *
	 *      @param [in] displayName     The new display name.
	 */
	void setDisplayName(const QString& displayName);

    /*!
	 *  Set the name for the instance.
	 *
	 *      @param [in] instanceName    The new instance name.
	 */
	void setInstanceName(const QString& instanceName);

    /*!
	 *  Set the name of the selected view.
	 *
	 *      @param [in] viewName    The name of the selected view.
	 */
	void setViewName(const QString& viewName);

private:

    //! The name of this instance.
	QString instanceName_;

    //! Descriptive text of the instance.
	QString displayName_;

    //! Textual description of the instance.
	QString description_;

    //! Configurable VLNV of the abstractor reference.
    QSharedPointer<ConfigurableVLNVReference> abstractorRef_;

    //! Defines the selected view.
	QString viewName_;
};

#endif // ABSTRACTORINSTANCE_H
