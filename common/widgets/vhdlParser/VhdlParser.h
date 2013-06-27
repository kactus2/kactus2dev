//-----------------------------------------------------------------------------
// File: VhdlParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// VhdlParser for Component Wizard.
//-----------------------------------------------------------------------------
#ifndef VhdlParser_H
#define VhdlParser_H

#include <QTextEdit>
#include <QTextBlock>
#include <QMouseEvent>
#include <QList>
#include <QMap>

class ModelParameter;
class Port;
class VhdlEntityHighlighter;

//-----------------------------------------------------------------------------
//! Class VhdlParser.
//-----------------------------------------------------------------------------
class VhdlParser : public QTextEdit
{
    Q_OBJECT
public:

     /*!
     *  Constructor.
	 *
     *      @param [in] parent The parent widget.
     */
    VhdlParser(QWidget* parent = 0);

    /*!
     *  Destructor.
     */
    ~VhdlParser();

    /*!
     *   Reads a vhdl file and creates generics and ports.
     *
     *      @param [in] absolutePath The absolute path of the file.
	 *
	 *      @return False, if file could not be read, otherwise true.
     */
     virtual void  parseFile(QString absolutePath);

    /*!
     *   Scrolls the texteditor to the beginning of a valid entity declaration.
     */
    void scrollToEntityBegin();

public slots:

    //! Called when a model parameter is outside the text editor.
    virtual void editorChangedModelParameter(QSharedPointer<ModelParameter> changedParameter);

    //! Called when a model parameter is removed outside the text editor.
    virtual void editorRemovedModelParameter(QSharedPointer<ModelParameter> removedParameter);

    //! Called when a port is removed outside the text editor.
    virtual void editorRemovedPort(QSharedPointer<Port> removedPort);

signals:
    
    //! Emitted when a port is created or selected.
    void addPort(QSharedPointer<Port> port);

    //! Emitted when a port is removed or deselected.
    void removePort(QSharedPointer<Port> port);

    //! Emitted when a generic is created or selected.
    void addGeneric(QSharedPointer<ModelParameter> modelParam);

    //! Emitted when a generic is removed or deselected.
    void removeGeneric(QSharedPointer<ModelParameter> modelParam);

protected:
 
    //! Called when mouse is double clicked.
    virtual void mouseDoubleClickEvent(QMouseEvent *e);

private:

    //! No copying.
    VhdlParser(const VhdlParser&);

    //! No assignment.
    VhdlParser& operator=(const VhdlParser&);  

	/*!
     *  Signals add for all ports.
     */
    void importPorts();

	/*!
     *  Signals remove for all ports and empties lists to ports.
     */
    void removePorts();

	/*!
     *  Signals add for all model parameters.
     */
    void importGenerics();

	/*!
     *  Signals remove for all model parameters and empties lists to model parameters.
     */
    void removeGenerics();

	/*!
     *  Parses, formats, checks and displays a vhdl file.
     */
    void createDocument(QFile& vhdlFile);

    /*!
     *   Checks if given vhdl file is valid.
     *
     *      @param [in] fileString The vhdl file as string.
	 *
	 *      @return True if the file is valid, otherwise false.
     */
    bool checkEntityStructure(QString const& fileString);

    /*!
     *   Cuts out a section of a text.
     *
     *      @param [in] begin The beginning of the cut section.
	 *
     *      @param [in] end The end of the cut section.
	 *
     *      @param [in] text The text where to cut from.
	 *
	 *      @return The text section inside begin and end. 
     *              The beginning and the end are omitted.
     */
    QString parseSectionContent(QRegExp const& begin, QRegExp const& end, 
        QString const& text);

    /*!
     *   Finds the port declarations in a vhdl file and creates ports accordingly 
     *   with createPort().
     *
     *      @param [in] fileString The vhdl file as string.
     */
    void parsePorts(QString const& fileString);

    /*!
     *   Finds the generic declarations in a vhdl file and creates model parameters
     *   accordingly with createGeneric().
     *
     *      @param [in] fileString The vhdl file as string.
     */
    void parseGenerics(QString const& fileString);

    /*!
     *   Creates a port and maps it to corresponding text block.
     *
     *      @param [in] portDeclaration The port declaration clause.
	 *
     *      @param [in] portBlock The textblock the clause resides in.
     */
    void createPort(QString const& portDeclaration, QTextBlock const& portBlock);

    /*!
     *   Creates a model parameter and maps it to corresponding text block.
     *
     *      @param [in] genericDeclaration The generic declaration clause.
	 *
     *      @param [in] portBlock The textblock the clause resides in.
     */
    void createGeneric(QString const& genericDeclaration, QTextBlock const& genericBlock);
    
    void assignGenerics(QSharedPointer<Port> port);

    /*!
     *   Changes the state of text block from selected to not selected and vice versa.
     *
     *      @param [in] block The block whose state to change.
     */
    void toggleBlock(QTextBlock& block);
    
    /*!
     *   Converts the value of a generic to an integer.
     *        
     *      @param [in] parameter The generic whose value is converted.
     *
	 *      @return The value as integer or -1 if value cannot be converted.
     */
    int genericToInt(QSharedPointer<ModelParameter> parameter);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Maps all ports on a line to a text block.
    QMap< QTextBlock,QList< QSharedPointer<Port> > > portsMap_;

    //! Maps all generics on a line to a text block.
    QMap<QTextBlock,QList< QSharedPointer< ModelParameter> > > genericsMap_;

    //! Text blocks containing port declarations.
    QList<QTextBlock> portBlocks_;

    //! Text blocks containing generic declarations.
    QList<QTextBlock> genericBlocks_;

    //! Maps a generic name and its value.
    QList< QSharedPointer<ModelParameter> > generics_;

    //! Maps a generic to ports using it.
    QMap< QSharedPointer<ModelParameter>,QList< QSharedPointer<Port> > > genericUsage_;

    //! The syntax highlighter for display.
    VhdlEntityHighlighter* highlighter_;

    //! The entity beginning definition in vhdl.
    QRegExp entityBegin_;

    //! The entity end definition in vhdl.
    QRegExp entityEnd_;

    //! The ports section beginning definition in vhdl.
    QRegExp portsBegin_;

    //! The ports section end definition in vhdl.
    QRegExp portsEnd_;

    //! The ports declaration definition in vhdl.
    QRegExp portPattern_;

    //! The type declaration definition in vhdl.
    QRegExp typePattern_;

    //! The generic declaration definition in vhdl.
    QRegExp genericsBegin_;

    //! The generics section beginning definition in vhdl.
    QRegExp genericsEnd_;

    //! The generics section end definition in vhdl.
    QRegExp genericPattern_;

    //! The comment definition in vhdl.
    QRegExp commentPattern_;

    //! The default value definition in vhdl.
    QRegExp defaultPattern_;
};

#endif // VhdlParser_H
