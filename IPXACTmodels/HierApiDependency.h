//-----------------------------------------------------------------------------
// File: HierApiDependency.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 28.5.2012
//
// Description:
// Hierarchical API dependency.
//-----------------------------------------------------------------------------

#ifndef HIERAPIDEPENDENCY_H
#define HIERAPIDEPENDENCY_H

#include "ApiConnection.h"

#include "ipxactmodels_global.h"

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QPointF>
#include <QVector2D>
#include <QStringList>

//-----------------------------------------------------------------------------
//! Class encapsulating API dependency connection data.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT HierApiDependency
{
public:
    /*!
     *  Default constructor.
     */
    HierApiDependency();

    /*!
     *  Constructor which sets all the values as given.
     *
     *      @param [in] name          The name to set.
     *      @param [in] displayName   The display name to set.
     *      @param [in] description   The description to set.
     *      @param [in] interfaceRef  Name reference to an API interface in the top-level component.
     *      @param [in] ref           The interface reference to a contained SW component instance.
     *      @param [in] position      The position of the top-level interface in the design diagram.
     *      @param [in] direction     The direction of the top-level interface in the design diagram.
     *      @param [in] route         The connection route.
     */
    HierApiDependency(QString const& name, QString const& displayName, QString const& description,
                      QString const& interfaceRef, ApiInterfaceRef const& ref,
                      QPointF const& position, QVector2D const& direction,
                      QList<QPointF> const& route);

    /*!
     *  Copy constructor.
     */
    HierApiDependency(HierApiDependency const& rhs);

    /*!
     *  Constructor which reads the API dependency from an XML node.
     *
     *      @param [in] node The source XML node.
     */
    HierApiDependency(QDomNode& node);

    /*!
     *  Destructor.
     */
    ~HierApiDependency();

    /*!
     *  Writes the contents to an XML stream.
     */
    void write(QXmlStreamWriter& writer) const;

	/*! \brief Check if the hierarchical API dependency is in valid state.
	 *
	 * \param errorList The string list to add the possible error messages to.
	 * \param instanceNames The HW/SW instance names contained in the containing design.
	 * \param parentId The identifier for the design containing the dependencies.
	 *
	 * \return True if the API dependency is in valid state.
	*/
	bool isValid(QStringList& errorList, QStringList const& instanceNames,
		QString const& parentId) const;

	/*! \brief Check if the hierarchical API dependency is in valid state.
	 *
	 * \param instanceNames The HW/SW instance names contained in the containing design.
	 *
	 * \return True if the API dependency is in valid state.
	*/
	bool isValid(const QStringList& instanceNames) const;

    /*!
     *  Sets the name of the dependency.
     *
     *      @param [in] name The name to set.
     */
    void setName(QString const& name);

    /*!
     *  Sets the display name of the dependency.
     *
     *      @param [in] displayName The display name to set.
     */
    void setDisplayName(QString const& displayName);

    /*!
     *  Sets the description of the dependency.
     *
     *      @param [in] description The description to set.
     */
    void setDescription(QString const& description);

    /*!
     *  Sets the top-level API interface reference.
     *
     *      @param [in] interfaceRef Name reference to an API interface in the top-level component.
     */
    void setInterfaceRef(QString const& interfaceRef);

    /*!
     *  Sets the interface reference to an API interface in a contained SW component instance.
     *
     *      @param [in] ref The interface reference.
     */
    void setInterface(ApiInterfaceRef const& ref);

    /*!
     *  Sets the flag whether the connection is off-page or not.
     *
     *      @param [in] offPage If true, the connection is set off-page.
     */
    void setOffPage(bool offPage);

    /*!
     *  Returns the name of the dependency.
     */
    QString const& getName() const;

    /*!
     *  Returns the display name of the SW instance.
     */
    QString const& getDisplayName() const;

    /*!
     *  Returns the description of the SW instance.
     */
    QString const& getDescription() const;

    /*!
     *  Returns the name reference to an API interface in the top-level component.
     */
    QString const& getInterfaceRef() const;

    /*!
     *  Returns the interface reference to an API interface in a contained SW component instance.
     */
    ApiInterfaceRef const& getInterface() const;

    /*!
     *  Returns the position of the interface in the top-level design diagram.
     */
    QPointF const& getPosition() const;

    /*!
     *  Returns the direction of the interface in the top-level design diagram.
     */
    QVector2D const& getDirection() const;

    /*!
     *  Returns the connection route.
     */
    QList<QPointF> const& getRoute() const;

    /*!
     *  Returns true if the connection is off-page.
     */
    bool isOffPage() const;

    /*!
     *  Assignment operator.
     */
    HierApiDependency& operator=(HierApiDependency const& rhs);

private:
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The name of the dependency.
    QString name_;

    //! The display name of the dependency.
    QString displayName_;

    //! The description of the dependency.
    QString desc_;

    //! The interface reference in the top-level component.
    QString interfaceRef_;

    //! The other reference to an SW component instance.
    ApiInterfaceRef interface_;

    //! The position of the top-level interface in the design diagram.
    QPointF position_;

    //! The direction of the top-level interface in the design diagram.
    QVector2D direction_;

    //! The connection route.
    QList<QPointF> route_;

    //! If true, the connection is off-page.
    bool offPage_;
};

//-----------------------------------------------------------------------------

#endif // HIERAPIDEPENDENCY_H
