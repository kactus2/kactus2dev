/* 
 *  	Created on: 19.9.2011
 *      Author: Antti Kamppi
 * 		filename: documentgenerator.h
 *		Project: Kactus 2
 */

#ifndef DOCUMENTGENERATOR_H
#define DOCUMENTGENERATOR_H

#include <designEditors/common/DesignWidgetFactory.h>

#include <editors/ComponentEditor/common/ExpressionFormatterFactory.h>
#include <editors/ComponentEditor/common/ExpressionFormatter.h>

#include <library/LibraryManager/libraryinterface.h>

#include <IPXACTmodels/vlnv.h>
#include <IPXACTmodels/component.h>
#include <IPXACTmodels/view.h>

#include <QObject>
#include <QTextStream>
#include <QList>
#include <QSharedPointer>
#include <QStringList>
#include <QWidget>

class DocumentGenerator : public QObject {
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] handler                     Library, where the components reside.
	 *      @param [in] vlnv                        VLNV of the component or design.
	 *      @param [in] designWidgetFactory         Factory for making design widgets.
	 *      @param [in] expressionFormatterFactory  Factory for making expression formatters.
	 *      @param [in] parent                      The parent widget of the generator.
	 */
	DocumentGenerator(LibraryInterface* handler,
        const VLNV& vlnv,
        DesignWidgetFactory* designWidgetFactory,
        ExpressionFormatterFactory* expressionFormatterFactory,
		QWidget* parent);

	/*!
	 *  The constructor for child generators.
	 *
	 *      @param [in] handler                     Library, where the components reside.
	 *      @param [in] vlnv                        VLNV of the component or design.
	 *      @param [in] objects                     The vlnvs that have been gone through already.
	 *      @param [in] designWidgetFactory         The factory for making design widgets.
	 *      @param [in] expressionFormatterFactory  The factory for making expression formatters.
	 *      @param [in] parent                      The parent generator.
	 */
	DocumentGenerator(LibraryInterface* handler, 
		const VLNV& vlnv,
		QList<VLNV>& objects,
        DesignWidgetFactory* designWidgetFactory,
        ExpressionFormatterFactory* expressionFormatterFactory,
		DocumentGenerator* parent);
	
	//! \brief The destructor
	virtual ~DocumentGenerator();

    /*!
     *  Write the documentation to the selected stream.
     *
     *      @param [in] stream          The stream where to write.
     *      @param [in] targetPath      The path where the document will be placed.
     */
    void writeDocumentation(QTextStream& stream, QString targetPath);

    /*!
     *  Write the html header.
     *
     *      @param [in] stream  The text stream to write the table of contents into.
     */
    void writeHtmlHeader(QTextStream& stream);

	/*!
	 *  Write the table of contents for the component.
	 *
	 *      @param [in] componentNumber     The number for this component.
	 *      @param [in] stream              The text stream to write the table of contents into.
	 */
	void writeTableOfContents(unsigned int& componentNumber, QTextStream& stream);

	/*!
	 *  Write the documentation for the given component.
	 *
	 *      @param [in] stream          The text stream to write the documentation into.
	 *      @param [in] targetPath      File path to the file to be written.
	 *      @param [in] filesToInclude  List of file names that contain pictures needed to display the document.
	 */
	void writeDocumentation(QTextStream& stream, const QString& targetPath, QStringList& filesToInclude);

	/*!
	 *  Write the model parameters of the component.
	 *
	 *      @param [in] stream              The text stream to write the documentation into.
	 *      @param [in] subHeaderNumber     The number that defines the number for the sub-header.
	 */
	void writeModelParameters(QTextStream& stream, int& subHeaderNumber);

    /*!
	 *  Write the parameters and kactus2 parameters of the component.
	 *
	 *      @param [in] stream              The text stream to write the documentation into.
	 *      @param [in] subHeaderNumber     The number that defines the sub-header.
	 */
	void writeParameters(QTextStream& stream, int& subHeaderNumber);

	/*!
	 *  Write the ports of the component.
	 *
	 *      @param [in] stream              The text stream to write the documentation into.
	 *      @param [in] subHeaderNumber     The number that defines the sub header.
	 */
	void writePorts(QTextStream& stream, int& subHeaderNumber);

    /*!
	 *  Write the interfaces of the component.
	 *
	 *      @param [in] stream              The text stream to write the documentation into.
	 *      @param [in] subHeaderNumber     The number that defines the sub header.
	 */
	void writeInterfaces(QTextStream& stream, int& subHeaderNumber);

	/*!
	 *  Write the file sets of the component.
	 *
	 *      @param [in] stream              The text stream to write the documentation into.
	 *      @param [in] subHeaderNumber     The number that defines the sub header.
	 */
	void writeFileSets(QTextStream& stream, int& subHeaderNumber);

	/*!
	 *  Write the views of the component.
	 *
	 *      @param [in] stream              The text stream to write the documentation into.
	 *      @param [in] subHeaderNumber     The number that defines the sub header.
	 *      @param [in] pictureList         List of file names to add the pictures of the designs to.
	 */
	void writeViews(QTextStream& stream, int& subHeaderNumber, QStringList& pictureList);

	/*! \brief Write the documentation for the given view.
	 *
	 * \param view Pointer to the view the document.
	 * \param stream Text stream to write into.
	 * \param subheaderNumber The number of the subheader.
	 * \param viewNumber The number of the view.
	 *
	*/
	void writeView(QSharedPointer<View> view, QTextStream& stream, const int subheaderNumber, const int viewNumber);

    /*!
     *  Write the end of the document.
     *
     *      @param [in] stream  The text stream to write the documentation into.
     */
    void writeEndOfDocument(QTextStream& stream);

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

    /*!
     *  Write the port table for the selected ports.
     *
     *      @param [in] stream  The stream to write into.
     *      @param [in] title   The title of the port table.
     *      @param [in] ports   The selected ports.
     */
    void writePortTable(QTextStream& stream, QString const& title, QList <QSharedPointer <Port> > ports);

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
	QWidget* parentWidget_;

    //! The factory for creating design widgets.
    DesignWidgetFactory* designWidgetFactory_;

    //! The factory for creating expression formatters.
    ExpressionFormatterFactory* expressionFormatterFactory_;

    //! The expression formatter, used to change parameter IDs into names.
    ExpressionFormatter* expressionFormatter_;
};

#endif // DOCUMENTGENERATOR_H
