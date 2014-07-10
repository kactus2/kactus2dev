//-----------------------------------------------------------------------------
// File: SWView.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 22.5.2012
//
// Description:
// SW view class for making VLNV references to SW designs.
//-----------------------------------------------------------------------------

#ifndef SWVIEW_H
#define SWVIEW_H

#include <library/LibraryManager/vlnv.h>

#include <IPXACTmodels/NameGroup.h>

#include <IPXACTmodels/filebuilder.h>
#include <common/Global.h>
#include <IPXACTmodels/swbuildcommand.h>
#include <IPXACTmodels/bspbuildcommand.h>

#include <QString>
#include <QSharedPointer>
#include <QDomNode>
#include <QXmlStreamWriter>
#include <QStringList>
#include <QList>

//-----------------------------------------------------------------------------
//! SW view class for making VLNV references to SW designs.
//-----------------------------------------------------------------------------
class KACTUS2_API SWView
{
public:
    /*!
     *  Constructor.
	 *
	 *      @param [in] viewNode A reference to a QDomNode to parse the information from.
	 */
	SWView(QDomNode &viewNode);

	/*!
     *  The constructor
     *
	 *      @param [in] name The name of the view.
	 */
	SWView(const QString& name);

	/*!
     *  Default constructor.
	 */
	SWView();

	//! Copy constructor.
	SWView(const SWView &other);

	//! Assignment operator.
	SWView &operator=(const SWView &other);

	/*!
     *  Destructor.
	 */
	~SWView();

	/*! 
     *  Writes the contents of the class using the writer.
	 *
	 *  Uses the specified writer to write the class contents into file as valid
	 *  IP-Xact.
	 *
	 *			@param [in] withinHWComp Specifies if the SW view is within HW or SW component.
	 *      @param [in] writer A reference to a QXmlStreamWriter instance that is used to
	 *                         write the document into file.
	 */
	void write(QXmlStreamWriter& writer, bool withinHWComp);

	/*!
     *  Checks if the view is in a valid state.
	 * 
	 *		@param [in] fileSetNames	  Contains the names of the component's file sets.
	 *		@param [in] cpuNames			  Contains the names of the component's CPU elements.
	 *		@param [in] errorList        The list to add the possible error messages to.
	 *    @param [in] parentIdentifier String from parent to help to identify the location of the error.
	 *
	 *      @return True if contents are valid.
	 */
	bool isValid(const QStringList& fileSetNames,
		const QStringList& cpuNames,
		QStringList& errorList, 
		QString const& parentIdentifier) const;

	/*!
     *  Checks if the view is in a valid state.
	 * 
	 * \param fileSetNames Contains the names of the component's file sets.
	 * \param cpuNames Contains the names of the component's CPU elements.
	 * 
	 *      @return True if contents are valid.
	*/
	bool isValid(const QStringList& fileSetNames,
		const QStringList& cpuNames) const;

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

	/*! \brief Get the name group of the software view.
	 *
	 * \return Reference to the name group.
	*/
	NameGroup& getNameGroup();

	/*! \brief Get the name group of the software view.
	 *
	 * \return Reference to the name group.
	*/
	const NameGroup& getNameGroup() const;

	/*! \brief Get the file set references of the SW view.
	 *
	 * Method: 		getFileSetRefs
	 * Full name:	SWView::getFileSetRefs
	 * Access:		public 
	 *
	 *
	 * \return QStringList containing the file set names.
	*/
	const QStringList& getFileSetRefs() const;

	/*! \brief Get the file set references of the SW view.
	 *
	 * Method: 		getFileSetRefs
	 * Full name:	SWView::getFileSetRefs
	 * Access:		public 
	 *
	 *
	 * \return QStringList containing the file set names.
	*/
	QStringList getFileSetRefs();

	/*! \brief Set the file set references for the SW view.
	 *
	 * Method: 		setFileSetRefs
	 * Full name:	SWView::setFileSetRefs
	 * Access:		public 
	 *
	 * \param fileSetRefs Contains the file set names to set.
	 *
	*/
	void setFileSetRefs(const QStringList& fileSetRefs);

	/*! \brief Add a file set reference to the SW view.
	 *
	 * Method: 		addFileSetRef
	 * Full name:	SWView::addFileSetRef
	 * Access:		public 
	 *
	 * \param fileSetName The name of the file set to refer to.
	 *
	*/
	void addFileSetRef(const QString& fileSetName);

	//! \brief Get the SW build commands of the SW view.
	QList<QSharedPointer<SWBuildCommand> >& getSWBuildCommands();

	//! \brief Get the SW build commands of the SW view.
	const QList<QSharedPointer<SWBuildCommand> >& getSWBuildCommands() const;

	//! \brief Get the build command for the board support package.
	QSharedPointer<BSPBuildCommand> getBSPBuildCommand();

	//! \brief Get the build command for the board support package.
	const QSharedPointer<BSPBuildCommand> getBSPBuildCommand() const;

private:
	//! \brief Contains the name, display name and description of view.
	NameGroup nameGroup_;

	/*!
	 * OPTIONAL spirit:hierarchyRef
	 * References a hierarchical design, mandatory only if the view is used to
	 * reference a hierarchical design.
	 */
	VLNV hierarchyRef_;

	//! \brief The file set references used by this SW view.
	QStringList filesetRefs_;

	//! \brief Contains the SW build commands for SW view.
	QList<QSharedPointer<SWBuildCommand> > swBuildCommands_;

	//! \brief Contains the command options to build the BSP package for HW component.
	QSharedPointer<BSPBuildCommand> bspCommand_;
};

//-----------------------------------------------------------------------------

#endif // SWVIEW_H
