/* 
 *  	Created on: 19.9.2011
 *      Author: Antti Kamppi
 * 		filename: documentgenerator.h
 *		Project: Kactus 2
 */

#ifndef DOCUMENTGENERATOR_H
#define DOCUMENTGENERATOR_H

#include <LibraryManager/libraryinterface.h>
#include <LibraryManager/vlnv.h>

#include <models/component.h>
#include <models/view.h>

#include <QObject>
#include <QTextStream>
#include <QList>
#include <QSharedPointer>
#include <QStringList>
#include <QWidget>

class DocumentGenerator : public QObject {
	Q_OBJECT

public:

	DocumentGenerator(LibraryInterface* handler, 
		QWidget* parent);

	DocumentGenerator(LibraryInterface* handler, 
		const VLNV& vlnv,
		QList<VLNV>& objects,
		DocumentGenerator* parent);
	
	//! \brief The destructor
	virtual ~DocumentGenerator();

	void setComponent(const VLNV& vlnv);

	/*! \brief Write the documentation for the given component.
	 * 
	 * NOTE: Before calling this function the changes made to the component 
	 * (and it's configurations) must be saved to the file system.
	 * 
	 * \param vlnv Identifies the component.
	 * 
	 * \return QString containing the path to the created html-file.
	 *
	*/
	QString writeDocumentation();

signals:

	//! \brief Print an error message to the user.
	void errorMessage(const QString& errorMessage);

	//! \brief Print a notification to the user.
	void noticeMessage(const QString& noticeMessage);

private:
	//! \brief No copying
	DocumentGenerator(const DocumentGenerator& other);

	//! \brief No assignment
	DocumentGenerator& operator=(const DocumentGenerator& other);

	/*! \brief Write the table of contents for the component.
	 * 
	 * This function is called recursively by this class.
	 * 
	 * \param componentNumber The number for this component.
	 * \param stream The text stream to write the table of contents into.
	 *
	*/
	void writeTableOfContents(unsigned int& componentNumber,
		QTextStream& stream);

	/*! \brief Write the documentation for the given component.
	 * 
	 * This function is called recursively by this class.
	 * 
	 * \param stream The text stream to write the documentation into.
	 * \param targetPath File path to the file to be written. This is needed to 
	 * print the relative paths to the files.
	 * \param filesToInclude List of file names that contain pictures needed to display
	 * the generated document.
	 *
	*/
	void writeDocumentation(QTextStream& stream, const QString& targetPath,
		QStringList& filesToInclude);

	/*! \brief Write the model parameters of the component
	 * 
	 * \param stream The text stream to write the documentation into.
	 * \param subHeaderNumber The number that defines the number for the sub-header.
	 * 
	*/
	void writeModelParameters(QTextStream& stream, int& subHeaderNumber);

	/*! \brief Write the parameters and kactus2 parameters of the component
	 *
	 * \param stream The text stream to write the documentation into.
	 * \param subHeaderNumber The number that defines the number for the sub-header.
	*/
	void writeParameters(QTextStream& stream, int& subHeaderNumber);

	/*! \brief Write the ports of the component.
	 *
	 * \param stream The text stream to write the documentation into.
	 * \param subHeaderNumber The number that defines the number for the sub-header.
	*/
	void writePorts(QTextStream& stream, int& subHeaderNumber);

	/*! \brief Write the interfaces of the component.
	 *
	 * \param stream The text stream to write the documentation into.
	 * \param subHeaderNumber The number that defines the number for the sub-header.
	*/
	void writeInterfaces(QTextStream& stream, int& subHeaderNumber);

	/*! \brief Write the file sets of the component.
	 *
	 * \param stream The text stream to write the documentation into.
	 * \param subHeaderNumber The number that defines the number for the sub-header.
	*/
	void writeFileSets(QTextStream& stream, int& subHeaderNumber);

	/*! \brief Write the views of the component.
	 *
	 * \param stream The text stream to write the documentation into.
	 * \param subHeaderNumber The number that defines the number for the sub-header.
	 * \param pictureList List of file names to add the pictures of the designs to.
	 *
	*/
	void writeViews(QTextStream& stream, int& subHeaderNumber, QStringList& pictureList);

	/*! \brief Write the header to the given stream.
	 *
	 * \param headerNumber The number of the sub-header.
	 * \param stream The stream to write into.
	 * \param text The textual part of the header.
	 * \param headerID The id-tag for the header element.
	 *
	*/
	void writeSubHeader(const int headerNumber, QTextStream& stream, 
		const QString& text, const QString& headerID);

	/*! \brief Write the html-table element start into stream
	 *
	 * \param headers The headers for the table
	 * \param title The title for the table.
	 * \param stream The text stream to write into.
	 *
	*/
	void writeTableElement(const QStringList& headers, const QString& title,
		QTextStream& stream);

	/*! \brief Write the details of a file
	 *
	 * \param file Pointer to the file.
	 * \param stream The text stream to write into.
	 *
	*/
	void writeFile(QSharedPointer<File> file,  
		QTextStream& stream);

	/*! \brief Write the documentation for the given view.
	 *
	 * \param view Pointer to the view the document.
	 * \param stream Text stream to write into.
	 * \param subheaderNumber The number of the subheader.
	 * \param viewNumber The number of the view.
	 *
	*/
	void writeView(QSharedPointer<View> view, QTextStream& stream,
		const int subheaderNumber, const int viewNumber);

	/*! \brief Parse the child items for the document generator.
	 *
	 * \param objects List of objects that have already been parsed so there won't
	 * be duplicate components in the generated document.
	 *
	*/
	void parseChildItems(QList<VLNV>& objects);

	/*! \brief Create a picture for the component
	 *
	 * \param pictureList List of file names to add the path of the created picture to.
	 *
	*/
	void createComponentPicture(QStringList& pictureList);

	/*! \brief Create a picture for the design
	 *
	 * \param pictureList List of file names to add the path of the created picture to.
	 * \param viewName Name of the view to create the design picture for.
	*/
	void createDesignPicture(QStringList& pictureList, const QString& viewName);

	//! \brief Pointer to the instance that manages the library.
	LibraryInterface* handler_;

	//! \brief Pointer to the component that's documentation this generator should write.
	QSharedPointer<Component> component_;

	//! \brief The running number that this instance uses for writing the numbered headers.
	unsigned int myNumber_;

	//! \brief List contains pointers to the document generators under this generator.
	QList<QSharedPointer<DocumentGenerator> > childInstances_;

	//! \brief The file path to the html-file being written.
	QString targetPath_;

	//! \brief Pointer to the instance that owns the top document generator.
	QWidget* parent_;
};

#endif // DOCUMENTGENERATOR_H
