//-----------------------------------------------------------------------------
// File: filebuildcommand.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileBuildCommand is a group box for editing buildCommand of a file.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDCOMMAND_H
#define FILEBUILDCOMMAND_H

#include <QCheckBox>
#include <QFileInfo>
#include <QGridLayout>
#include <QGroupBox>
#include <QLineEdit>
#include <QPushButton>
#include <QSharedPointer>

class LibraryInterface;
class BuildCommand;
class File;
class Component;
class ExpressionEditor;
class ParameterFinder;
class ExpressionParser;
class FileInterface;

//-----------------------------------------------------------------------------
//! FileBuildCommand is a group box for editing buildCommand of a file.
//-----------------------------------------------------------------------------
class FileBuildCommand : public QGroupBox
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] fileName            The file that is being edited.
     *    @param [in] fileInterface       Interface for files.
     *    @param [in] componentPath       The path to the component containing the file.
     *    @param [in] parameterFinder     The used parameter finder.
     *    @param [in] expressionParser    Parser for calculating expressions.
     *    @param [in] parent              The owner of this widget.
	 */
    FileBuildCommand(std::string fileName,
        FileInterface* fileInterface,
        QString const& componentPath,
        QSharedPointer<ParameterFinder> parameterFinder,
        QSharedPointer<ExpressionParser> expressionParser,
        QWidget *parent);

	/*!
     *  The destructor.
     */
	virtual ~FileBuildCommand() = default;

	/*!
     *  Restore the changes from the model to the editor.
	 */
	void refresh();

    /*!
     *  Handle the name change of the containing file.
     *
     *    @param [in] newName     The new name of the file.
     */
    void fileRenamed(std::string const& newName);

signals:

	/*!
     *  Emitted when contents of the widget change.
     */
	void contentChanged();

    /*!
     *  Increase the number of references made to the selected parameter.
     *
     *    @param [in] id  ID of the selected parameter.
     */
    void increaseReferences(QString id);

    /*!
     *  Decrease the number of references made to the selected parameter.
     *
     *    @param [in] id  ID of the selected parameter.
     */
    void decreaseReferences(QString id);

private slots:

	/*!
     *  Handler for command changes.
     */
    void onCommandChanged();

    /*!
     *  Handler for flag changes.
     */
    void onFlagsChanged();

    /*!
     *  Handler for target changes.
     */
    void onTargetChanged();

    /*!
     *  Handler for changes in the replace default flags.
     */
    void onReplaceDefaultChanged();

    /*!
     *  Handler for target browsing.
     */
    void onBrowseTarget();

private:

	/*!
     *  No copying. No assignment.
     */
	FileBuildCommand(const FileBuildCommand& other);

    /*!
     *  Equals operator.
     */
    FileBuildCommand& operator=(const FileBuildCommand& other);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Name of the file whose build command is being edited.
    std::string fileName_;

    //! Interface for files.
    FileInterface* fileInterface_;

    //! Path to the containing component.
    QString componentPath_;

    //! Editor to set build command's target file.
    QLineEdit targetEditor_;

    //! Button for browsing the build command target file.
    QPushButton browseTargetButton_;

	//! Editor to set file's build command.
	QLineEdit commandEditor_;

	//! Editor to set build command's flags.
	QLineEdit flagsEditor_;

	//! Expression editor to set build command's replaceDefaultFlags setting.
    ExpressionEditor* replaceDefaultEditor_;

    //! The used expression parser.
    QSharedPointer<ExpressionParser> expressionParser_;
};

#endif // FILEBUILDCOMMAND_H
