//-----------------------------------------------------------------------------
// File: filetypeeditor.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Antti Kamppi
// Date: 7.2.2011
//
// Description:
// FileTypeEditor is used to specify a file type for a file.
//-----------------------------------------------------------------------------

#ifndef FILETYPEEDITOR_H
#define FILETYPEEDITOR_H

#include <common/widgets/listManager/listmanager.h>

#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>

class File;
class FileInterface;

//-----------------------------------------------------------------------------
//! FileTypeEditor is used to specify a file type for a file.
//-----------------------------------------------------------------------------
class FileTypeEditor : public ListManager
{
	Q_OBJECT

public:

	/*!
     *  The constructor.
	 *
	 *    @param [in] parent          Pointer to the owner of this widget
	 *    @param [in] fileName        Name of the file being edited
     *    @param [in] fileInterface   Interface for accessing files.
	 */
    FileTypeEditor(Document::Revision docRevision, QWidget *parent, std::string const& fileName, FileInterface* fileInterface);

	/*!
     *  The destructor.
     */
	virtual ~FileTypeEditor() = default;

	/*!
     *  Initialize the file type editor.
	 *
	 * This function must be called after creating the file type editor and before using it.
	 * 
	 *    @param [in] items   QStringList that contains the items to add to the widget.
	 */
	virtual void initialize(const QStringList& items = QStringList());

	/*!
     *  Apply the changes from the editor to the model.
	 */
	void apply();

	/*!
     *  Restore the changes from the model to the editor.
	 */
	void restore();

	/*!
     *  Check if the editor is in a valid state or not.
	 *
	 *    @return True if the editor is in valid state
	 */
	bool isValid() const;

    /*!
     *  Handle the name change of the containing file.
     *
     *    @param [in] newName     The new name of the file.
     */
    void fileRenamed(std::string const& newName);

private:

	//! No copying. No assignment. No quarter.
	FileTypeEditor(const FileTypeEditor& other);
	FileTypeEditor& operator=(const FileTypeEditor& other);

	//! Name of the file-model being edited.
    std::string fileName_;

    //! Interface for accessing files.
    FileInterface* fileInterface_;

	Document::Revision revision_;
};

#endif // FILETYPEEDITOR_H
