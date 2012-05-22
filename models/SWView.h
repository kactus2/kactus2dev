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

#include <LibraryManager/vlnv.h>

#include <models/generaldeclarations.h>

#include <QString>
#include <QDomNode>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! SW view class for making VLNV references to SW designs.
//-----------------------------------------------------------------------------
class SWView
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
	SWView(const QString name);

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
	 *      @param [in] writer A reference to a QXmlStreamWriter instance that is used to
	 *                         write the document into file.
	 */
	void write(QXmlStreamWriter& writer);

	/*!
     *  Checks if the view is in a valid state.
	 * 
	 *      @param [in] errorList        The list to add the possible error messages to.
	 *      @param [in] parentIdentifier String from parent to help to identify the location of the error.
	 *
	 *      @return True if contents are valid.
	 */
	bool isValid(QStringList& errorList, QString const& parentIdentifier) const;

	/*!
     *  Checks if the view is in a valid state.
	 * 
	 *      @return True if contents are valid.
	*/
	bool isValid() const;

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

private:
	//! \brief Contains the name, display name and description of view.
	General::NameGroup nameGroup_;

	/*!
	 * MANDATORY spirit:hierarchyRef
	 * References a hierarchical design, mandatory only if the view is used to
	 * reference a hierarchical design.
	 */
	VLNV hierarchyRef_;
};

//-----------------------------------------------------------------------------

#endif // SWVIEW_H
