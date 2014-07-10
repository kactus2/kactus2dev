//-----------------------------------------------------------------------------
// File: SystemView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.5.2012
//
// Description:
// System view class for making VLNV references to System designs.
//-----------------------------------------------------------------------------

#ifndef SYSTEMVIEW_H
#define SYSTEMVIEW_H

#include <library/LibraryManager/vlnv.h>

#include <IPXACTmodels/NameGroup.h>

#include <common/Global.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QStringList>

//-----------------------------------------------------------------------------
//! System view class for making VLNV references to system designs.
//-----------------------------------------------------------------------------
class KACTUS2_API SystemView
{
public:
    /*!
     *  Constructor.
	 *
	 *      @param [in] viewNode A reference to a QDomNode to parse the information from.
	 */
	SystemView(QDomNode &viewNode);

	/*!
     *  The constructor
     *
	 *      @param [in] name The name of the view.
	 */
	SystemView(const QString name);

	/*!
     *  Default constructor.
	 */
	SystemView();

	//! Copy constructor.
	SystemView(const SystemView &other);

	//! Assignment operator.
	SystemView &operator=(const SystemView &other);

	/*!
     *  Destructor.
	 */
	~SystemView();

	/*! 
     *  Writes the contents of the class using the writer.
	 *
	 *  Uses the specified writer to write the class contents into file as valid
	 *  IP-Xact.
	 *
	 *      @param [in] writer A reference to a QXmlStreamWriter instance that is used to
	 *                         write the document into file.
	 */
	void write(QXmlStreamWriter& writer);

	/*!
     *  Checks if the view is in a valid state.
	 * 
	 *		  @param [in] fileSetNames		 File set names of the containing component.
	 *		  @param [in] HWViewNames		 Names of the HW views in the containing component.
	 *      @param [in] errorList        The list to add the possible error messages to.
	 *      @param [in] parentIdentifier String from parent to help to identify the location of the error.
	 *
	 *      @return True if contents are valid.
	 */
	bool isValid(const QStringList& fileSetNames,
		const QStringList& HWViewNames,
		QStringList& errorList,
		QString const& parentIdentifier) const;

	/*!
     *  Checks if the view is in a valid state.
	 * 
	 *		  @param [in] fileSetNames File set names of the containing component.
	 *		  @param [in] HWViewNames Names of the HW views in the containing component.
	 * 
	 *      @return True if contents are valid.
	*/
	bool isValid(const QStringList& fileSetNames, const QStringList& HWViewNames) const;

    /*! \brief Set the hierarchyRef for this view
	 *
	 * Calling this function will delete the old hierarchyRef
	 *
	 * \param hierarchyRef A pointer to the new hierarchyRef
	 */
	void setHierarchyRef(const VLNV& hierarchyRef);

	/*! \brief Set the name for the view
	 *
	 * \param name QString containing the new name.
	 */
	void setName(const QString &name);

    /*! \brief Set the display name for the view.
	 *
	 * \param displayName Contains the display name to set.
	 *
	*/
	void setDisplayName(const QString& displayName);

	/*! \brief Set the description for the view.
	 *
	 * \param description Contains the description of the view.
	 *
	*/
	void setDescription(const QString& description);

    /*!
     *  Sets the reference to HW view to which the system is mapped.
     *
     *      @param [in] viewName The name of the HW view.
     */
    void setHWViewRef(QString const& viewName);

	/*! \brief Get pointer to the hierarchical design for this view
	 *
	 * \return A pointer to the vlnv of a hierarchical design if the view is
	 * used to reference a hierarchical design. If not a null pointer is
	 * returned.
	 */
	VLNV getHierarchyRef() const;

	/*! \brief Get the name of the view
	 *
	 * \return QString containing the name
	 */
	QString getName() const;

	/*! \brief Get the display name of the view
	 *
	 * \return QString containing the display name.
	*/
	QString getDisplayName() const;

	/*! \brief Get the description of the view.
	 *
	 * \return QString containing the description.
	*/
	QString getDescription() const;

    /*!
     *  Returns the reference to HW view to which the system is mapped.
     */
    QString const& getHWViewRef() const;

	/*! \brief Get the name group of the software view.
	 *
	 * \return Reference to the name group.
	*/
	NameGroup& getNameGroup();

	/*! \brief Get the file set references of the system view.
	 *
	 * Method: 		getFileSetRefs
	 * Full name:	SystemView::getFileSetRefs
	 * Access:		public 
	 *
	 *
	 * \return QStringList containing the file set names.
	*/
	QStringList getFileSetRefs() const;

	/*! \brief Set the file set references for the system view.
	 *
	 * Method: 		setFileSetRefs
	 * Full name:	SystemView::setFileSetRefs
	 * Access:		public 
	 *
	 * \param fileSetRefs Contains the file set names.
	 *
	*/
	void setFileSetRefs(const QStringList& fileSetRefs);

private:
	//! \brief Contains the name, display name and description of view.
	NameGroup nameGroup_;

	/*!
	 * MANDATORY spirit:hierarchyRef
	 * References a hierarchical design, mandatory only if the view is used to
	 * reference a hierarchical design.
	 */
	VLNV hierarchyRef_;

    //! The name of the HW view to which the system references.
    QString hwViewRef_;

	//! \brief Contains the references to file sets of the containing component.
	 QStringList fileSetRefs_;
};

//-----------------------------------------------------------------------------

#endif // SYSTEMVIEW_H
