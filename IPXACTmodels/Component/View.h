//-----------------------------------------------------------------------------
// File: View.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 03.09.2015
//
// Description:
// Describes the ipxact:view element of a component.
//-----------------------------------------------------------------------------

#ifndef VIEW_H
#define VIEW_H

#include <IPXACTmodels/common/NameGroup.h>
#include <IPXACTmodels/common/Extendable.h>

#include <IPXACTmodels/ipxactmodels_global.h>

#include <QString>
#include <QSharedPointer>
#include <QStringList>
#include <QList>

//-----------------------------------------------------------------------------
//! Represents the ipxact:view element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT View : public NameGroup, public Extendable
{
public:

    using List = QSharedPointer<QList<QSharedPointer<View> > >;

	// Represents the ipxact:envIdentifier element, broken down to separate fields.
	struct EnvironmentIdentifier
	{
		// The fields of ipxact:environment element.
		QString language;
		QString tool;
		QString vendorSpecific;
		
        EnvironmentIdentifier(): language(), tool(), vendorSpecific() {}

        EnvironmentIdentifier(QString const& IdentifierLanguage, QString const& identifierTool = QString(),
            QString const& identifierVendorSpecific = QString()):
        language(IdentifierLanguage), tool(identifierTool), vendorSpecific(identifierVendorSpecific)
        {

        }

		/*!
		 *  Returns the environment identifier in its ipxact format: language:tool:vendorSpecific.
		 */
		QString toString()
		{
			return language + QLatin1Char(':') + tool + QLatin1Char(':') + vendorSpecific;
		}
	};

	/*!
	 *  The constructor.
	 *
	 *    @param [in] name            Name of the view.
	 *    @param [in] displayName     Display name of the view.
	 *    @param [in] description     Description of the view.
	 */
	View(QString const& name = QString(), QString const& displayName = QString(),
        QString const& description = QString());

	/*!
	 *  Copy constructor.
	 */
	View(const View &other);

	/*!
	 *  Assignment operator.
	 */
	View& operator=(const View &other);

	/*!
	 *  The destructor.
	 */
	~View();

	/*!
	 *  Get a list of envIdentifiers.
	 *
     *    @return A list containing the envIdentifiers.
	 */
	QSharedPointer<QList<QSharedPointer<EnvironmentIdentifier> > > getEnvIdentifiers() const;

	/*!
	 *  Add a new envIdentifier for this view.
	 *
	 *    @param [in] envIdentifier   The envIdentifier to add.
	 */
	void addEnvIdentifier(QSharedPointer<EnvironmentIdentifier> envIdentifier);
	
    /*!
     *  Returns true, if an identifier with the exactly same fields exists within the view.
     */
    bool hasEnvIdentifier(QSharedPointer<EnvironmentIdentifier> envIdentifier) const;

    /*!
     *  Gets the presence.
     *
     *    @return The presence value.
     */
    QString getIsPresent() const;

    /*!
     *  Set the presence.
     *
     *    @param [in] newIsPresent    The new presence value.
     */
    void setIsPresent(QString const& newIsPresent);

    /*!
     *  Get the name of the referenced component instantiation.
     *
     *    @return The name of the referenced component instantiation.
     */
    QString getComponentInstantiationRef() const;

    /*!
     *  Set a component instantiation reference.
     *
     *    @param [in] newInstantiationRef     Name of the referenced component instantiation.
     */
    void setComponentInstantiationRef(QString const& newInstantiationRef);

    /*!
     *  Get the name of the referenced design instantiation.
     *
     *    @return The name of the referenced design instantiation.
     */
    QString getDesignInstantiationRef() const;

    /*!
     *  Set a design instantiation reference.
     *
     *    @param [in] newInstantiationRef     Name of the referenced design instantiation.
     */
    void setDesignInstantiationRef(QString const& newInstantiationRef);

    /*!
     *  Get the name of the referenced design configuration instantiation.
     *
     *    @return The name of the referenced design configuration instantiation.
     */
    QString getDesignConfigurationInstantiationRef() const;

    /*!
     *  Set a design configuration instantiation reference.
     *
     *    @param [in] newInstantiationRef     Name of the referenced design configuration instantiation.
     */
    void setDesignConfigurationInstantiationRef(QString const& newInstantiationRef);

    /*!
     *  Check if the view is hierarchical.
     *
     *    @return True, if the view is hierarchical, false otherwise.
     */
    bool isHierarchical() const;

private:

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Presence of the view.
    QString isPresent_;

    //! Designates and qualifies information of deployment in a particular tool environment.
	QSharedPointer<QList<QSharedPointer<EnvironmentIdentifier> > > envIdentifiers_;

    //! Name of the referenced component instantiation.
    QString componentInstantiationRef_;

    //! Name of the referenced design instantiation.
    QString designInstantiationRef_;

    //! Name of the referenced design configuration instantiation.
    QString designConfigurationInstantiationRef_;
};

#endif // VIEW_H
