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

#include <common/widgets/vhdlParser/VhdlEntityHighlighter.h>
#include <common/widgets/vhdlParser/VhdlSyntax.h>

class ModelParameter;
class Port;


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
    bool checkEntityStructure(QString const& fileString) const;


    /*!
     *   Sets the entity name in the ending expression.
     *
     *      @param [in] fileString The vhdl file as string.
     */
    void setEntityEndExp(QString const& fileString);

    /*!
     *   Cuts out a section of a text leaving delimiting expressions.
     *
     *      @param [in] begin The beginning of the cut section.
	 *
     *      @param [in] end The end of the cut section.
	 *
     *      @param [in] text The text where to cut from.
	 *
	 *      @return The text section inside begin and end. 
     *              The beginning and the end are included.
     */
    QString parseSection(QRegExp const& begin, QRegExp const& end, 
        QString const& text) const;

    /*!
     *   Cuts out a section of a text omitting delimiting expressions.
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
        QString const& text) const;

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
     *   Parses the values for vector bounds.
     *
     *      @param [in] rangeDeclaration The range declaration e.g "(32 downto 0)".
     *
     *      @param [out] leftBound The left bound of the vector.
     *
     *      @param [out] rightBound The right bound of the vector.
     */
    void parseBounds(QString const& rangeDeclaration, int& leftBound, int& rightBound) const;

    /*!
     *  Parses the value of a simple equation. The equation may contain literals and
     *  generics but not parathesis.
     *
     *      @param [in] equation The equation to solve.
     *
     *      @return The result of the equation.
     */
    int parseEquation(QString const& equation) const;

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
    
    /*!
     *   Assigns the generic values used in port declaration for left and right bound
     *   and default value in a port.
	 *
     *      @param [in] port The port to assign to.
     */
    void assignGenerics(QSharedPointer<Port> port);
   
    /*!
     *   Assigns the generic values used in parameter declaration for default value.
	 *
     *      @param [in] param The parameter to assign to.
     */
    void assignGenerics(QSharedPointer<ModelParameter> param);

    /*!
     *   Inserts a section of text into the editor's document with given state.
	 *
     *      @param [in] text The text to insert into document.
	 *
     *      @param [in] state The state set for all blocks in the inserted text.
     */
    void insertExtraText(QString const& text, VhdlEntityHighlighter::BlockStyle style);

    /*!
     *   Changes the state of text block from selected to not selected and vice versa.
     *
     *      @param [in] block The block whose state to change.
     */
    void toggleBlock(QTextBlock& block);

    /*!
     *   Converts a generic or a number to an integer.
	 *
     *      @param [in] value The string to convert.
	 *
  	 *      @return The value as integer or -1 if value cannot be converted.
     */
    int valueForString(QString& string) const;
    

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
    QMap< QSharedPointer<ModelParameter>,QList< QSharedPointer<Port> > > genericsInPorts_;

    //! Maps a generic to another generic using it.
    QMap< QSharedPointer<ModelParameter>,QList< QSharedPointer<ModelParameter> > > genericsInGenerics_;

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
    QRegExp portExp_;

    //! The type declaration definition in vhdl.
    QRegExp typeExp_;

    //! The generic declaration definition in vhdl.
    QRegExp genericsBegin_;

    //! The generics section beginning definition in vhdl.
    QRegExp genericsEnd_;

    //! The generics section end definition in vhdl.
    QRegExp genericExp_;

    //! The comment definition in vhdl.
    QRegExp commentExp_;

    //! Pattern for equations in default values and vector bounds.
    QRegExp equationExp_;

    //! Pattern for newlines.
    QRegExp newline_;

};

#endif // VhdlParser_H
