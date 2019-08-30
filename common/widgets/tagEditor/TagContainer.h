//-----------------------------------------------------------------------------
// File: TagContainer.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.08.2019
//
// Description:
// Container for tags.
//-----------------------------------------------------------------------------

#ifndef TAGCONTAINER_H
#define TAGCONTAINER_H

#include <QWidget>
#include <QObject>

class TagLabel;
class TagEditor;

//-----------------------------------------------------------------------------
//! Container for tags.
//-----------------------------------------------------------------------------
class TagContainer : public QWidget
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent   The parent widget.
     */
    TagContainer(QWidget* parent);

    /*!
     *  The destructor.
     */
    virtual ~TagContainer() = default;

    /*!
     *  Setup the existing tags.
     *
     *      @param [in] documentTags    The tags of the selected document.
     */
    void setupTags(QVector<QPair<QString, QString> > documentTags);

    /*!
     *  Get the tags.
     *
     *      @return A list of the edited document tags.
     */
    QVector<QPair<QString, QString> > getTags() const;

signals:

    /*!
     *  Signaled when the tags have changed.
     */
    void contentChanged();

private slots:

    /*!
     *  Handler for new tag creation.
     */
    void createNewTag();

    /*!
     *  Handler for selecting a tag.
     *
     *      @param [in] clickedItem     The selected tag label.
     */
    void itemClicked(TagLabel* clickedItem);

    /*!
     *  Handler for accepting changes made to the selected tag label.
     *
     *      @param [in] editedTag   The edited tag label.
     *      @param [in] tagEditor   Editor of the selected tag label.
     */
    void itemChangesAccepted(TagLabel* editedTag, TagEditor* tagEditor);

    /*!
     *  Handler for removing a tag.
     *
     *      @param [in] editedTag   The edited tag label.
     *      @param [in] tagEditor   Editor of the selected tag label.
     */
    void itemDeleted(TagLabel* editedTag, TagEditor* tagEditor);

private:
    // Disable copying.
    TagContainer(TagContainer const& rhs);
    TagContainer& operator=(TagContainer const& rhs);

    /*!
     *  Connect the selected tag label.
     *
     *      @param [in] tagItem The selected tag label.
     */
    void connectTagLabel(QWidget* tagItem);

    /*!
     *  Setup the layout.
     */
    void setupLayout();

    /*!
     *  Construct a tag editor for the selected tag label.
     *
     *      @param [in] editedLabel     The selected tag label.
     *
     *      @return The created tag editor.
     */
    TagEditor* constructTagEditor(TagLabel* editedLabel);

    /*!
     *  Check if the selected tag exists.
     *
     *      @param [in] editedTag   The selected tag.
     *
     *      @return True, if the tag label exists, false otherwise.
     */
    bool tagExists(TagLabel* editedTag) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The button for creating new tags.
    TagLabel* additionTag_;

    //! List of all the constructed tags.
    QVector<QWidget*> tags_;
};

//-----------------------------------------------------------------------------

#endif // TAGEDITOR_H
