//-----------------------------------------------------------------------------
// File: GeneralDocumentGenerator.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 27.10.2017
//
// Description:
// Contains general functions for document generation.
//-----------------------------------------------------------------------------

#ifndef GENERALDOCUMENTGENERATOR_H
#define GENERALDOCUMENTGENERATOR_H

#include <QObject>
#include <QTextStream>
#include <QSharedPointer>
#include <QWidget>

class LibraryInterface;
class Component;
class Design;
class Parameter;
class FileBuilder;
class ExpressionFormatterFactory;
class ExpressionFormatter;

//-----------------------------------------------------------------------------
//! Contains general functions for document generation.
//-----------------------------------------------------------------------------
class GeneralDocumentGenerator : public QObject
{
	Q_OBJECT

public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] handler             Library, where the components reside.
	 *      @param [in] formatterFactory    Factory for creating expression formatters.
	 *      @param [in] parent              The parent widget of the generator.
	 */
	GeneralDocumentGenerator(LibraryInterface* handler, ExpressionFormatterFactory* formatterFactory,
        QObject* parent = 0);
	
	/*!
     *  The destructor.
     */
    virtual ~GeneralDocumentGenerator();

signals:

	/*!
     *  Print an error message to the user.
     *
     *      @param [in] errorMessage    Selected error message.
     */
	void errorMessage(const QString& errorMessage);

	/*!
     *  Print a notification to the user.
     *
     *      @param [in] noticeMessage   Selected notification.
     */
	void noticeMessage(const QString& noticeMessage);

protected:

	/*!
     *  Write the html-table element start into stream
	 *
	 *      @param [in] headers     The headers for the table
	 *      @param [in] title       The title for the table.
	 *      @param [in] stream      The text stream to write into.
     *      @param [in] tableTabs   Table indentation in the file.
	 */
    void writeTableElement(const QStringList& headers, const QString& title, QTextStream& stream,
        QString const& tableTabs);

    /*!
     *  Set a new component for the generator.
     *
     *      @param [in] newComponent    The selected component.
     */
    void setComponent(QSharedPointer<Component> newComponent);

    /*!
     *  Get the documented component.
     *
     *      @return The current component.
     */
    QSharedPointer<Component> getComponent() const;

    /*!
     *  Set the header number for the documented component.
     *
     *      @param [in] newNumber   The new header number.
     */
    void setNumber(int const& newNumber);

    /*!
     *  Get the header number of the documented component.
     *
     *      @return The header number.
     */
    int myNumber() const;

    /*!
     *  Set a new target path for document generation.
     *
     *      @param [in] newPath     The new path.
     */
    void setTargetPath(QString const& newPath);

    /*!
     *  Get the path of the document generation.
     *
     *      @return The path of the document generation.
     */
    QString getTargetPath() const;

    /*!
     *  Get the library handler.
     *
     *      @return Access point to handling the library.
     */
    LibraryInterface* getLibraryHandler() const;

    /*!
     *  Get the expression formatter factory.
     *
     *      @return The factory used for creating expression formatters.
     */
    ExpressionFormatterFactory* getExpressionFormatterFactory() const;

    /*!
     *  Create an expression formatter using the parameters of the contained component.
     *
     *      @return The created expression formatter.
     */
    ExpressionFormatter* createExpressionFormatter() const;

    /*!
     *  Create an expression formatter for a component instance contained within a design.
     *
     *      @param [in] design              The design containing the selected component instance.
     *      @param [in] instancedComponent  The selected component instance.
     *
     *      @return Expression formatter for the selected component instance.
     */
    ExpressionFormatter* createDesignInstanceFormatter(QSharedPointer<Design> design,
        QSharedPointer<Component> instancedComponent) const;

    /*!
     *  Write the documentation for the selected parameters.
     *
     *      @param [in] stream          The text stream to write into.
     *      @param [in] tableTooltip    Tooltip for the parameter table.
     *      @param [in] parameterTabs   Parameter table indentation in the file.
     *      @param [in] parameters      The selected list of parameters.
     *      @param [in] formatter       Expression formatter for the parameters.
     */
    void writeParameters(QTextStream& stream, QString const& tableTooltip, QString const& parameterTabs,
        QSharedPointer<QList<QSharedPointer<Parameter> > > parameters, ExpressionFormatter* formatter);

    /*!
     *  Write the documentation for the selected file build commands.
     *
     *      @param [in] stream                      The text stream to write into.
     *      @param [in] buildCommandTabs            File build command table indentation in the file.
     *      @param [in] buildCommands               The selected file build commands.
     *      @param [in] buildExpressionFormatter    Expression formatter for the file build commands.
     */
    void writeFileBuildCommands(QTextStream& stream, QString const& buildCommandTabs,
        QSharedPointer<QList<QSharedPointer<FileBuilder> > > buildCommands,
        ExpressionFormatter* buildExpressionFormatter);

private:

	//! No copying. No assignment.
	GeneralDocumentGenerator(const GeneralDocumentGenerator& other);
	GeneralDocumentGenerator& operator=(const GeneralDocumentGenerator& other);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

	//! Pointer to the instance that manages the library.
	LibraryInterface* libraryHandler_;

	//! Pointer to the component whose documentation this generator writes.
	QSharedPointer<Component> component_;

	//! The running number that used for writing the numbered headers.
	unsigned int componentNumber_;

	//! The file path to the html-file being written.
	QString targetPath_;

    //! The factory for creating expression formatters.
    ExpressionFormatterFactory* expressionFormatterFactory_;
};

#endif // GENERALDOCUMENTGENERATOR_H
