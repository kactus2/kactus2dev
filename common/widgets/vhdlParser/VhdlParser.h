//-----------------------------------------------------------------------------
// File: VhdlParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// VhdlParser reads a given vhdl file, creates ports and generics accordingly
// and displays the file highlighting the created ports and generics. 
//-----------------------------------------------------------------------------
#ifndef VhdlParser_H
#define VhdlParser_H

#include <QMouseEvent>
#include <QList>
#include <QMap>
#include <QPlainTextEdit>

#include <Plugins/VHDLParser/VHDLPortParser.h>
#include <Plugins/VHDLParser/VHDLGenericParser.h>

#include <Plugins/PluginSystem/Importparser.h>

class ModelParameter;
class Port;
class VHDLHighlighter;

//-----------------------------------------------------------------------------
//! Class VhdlParser.
//-----------------------------------------------------------------------------
class VhdlParser : public QObject, public ImportParser
{
    Q_OBJECT
public:

    /*!
     * The constructor.
     *
     *      @param [in] display   The display widget for the parsed source file content.
     *      @param [in] parent    The parent object.
     */
    VhdlParser(QPlainTextEdit* display, QObject* parent);

    /*!
     *  Destructor.
     */
    ~VhdlParser();

    /*!
     *   Scrolls the source display to the beginning of a valid entity declaration.
     */
    void scrollToEntityBegin();

    /*!
     *   Reads a vhdl file and creates generics and ports.
     *
     *      @param [in] absolutePath The absolute path of the file.
	 *
	 *      @return False, if file could not be read, otherwise true.
     */
    virtual void runParser(QString const& absolutePath, QSharedPointer<Component> targetComponent);

    void clear();

public slots:

    //! Called when a model parameter is outside the text editor.
    virtual void editorChangedModelParameter(QSharedPointer<ModelParameter> changedParameter) const;

    void onPortParsed(QSharedPointer<Port> parsedPort, QString const& declaration);   

    void onGenericParsed(QSharedPointer<ModelParameter> parsedGeneric);

signals:
    
    //! Emitted when a port is created or selected.
    void add(QSharedPointer<Port> port);

    //! Emitted when a port is removed or deselected.
    void removePort(QSharedPointer<Port> port);

    //! Emitted when a generic is created or selected.
    void add(QSharedPointer<ModelParameter> modelParam);

    //! Emitted when a generic is removed or deselected.
    void removeGeneric(QSharedPointer<ModelParameter> modelParam);

private:

    //! No copying.
    VhdlParser(const VhdlParser&);

    //! No assignment.
    VhdlParser& operator=(const VhdlParser&);  
  
    //! Position and enable of a port/generic in the document.
    struct SelectionInfo
    {
        int beginPos;
        int endPos;
        bool operator<(const SelectionInfo& other) const{return beginPos < other.beginPos;}
    };


    void loadFileToDisplay(QString const& absolutePath);

	/*!
     *  Signals remove for all ports and empties lists to ports.
     */
    void removePreviousPorts();

	/*!
     *  Signals remove for all model parameters and empties lists to model parameters.
     */
    void removePreviousGenerics();

    /*!
     *   Checks if given vhdl file has a valid entity declaration.
     *
     *      @param [in] fileString The vhdl file as string.
	 *
	 *      @return True if the file has a valid entity, otherwise false.
     */
    bool hasValidEntity(QString const& fileContent) const;

    void grayOutFileContent(QString const& fileContent) const;

    void highlightEntity(QString const& fileContent) const;
    
    void addDependencyOfGenericToPort(QSharedPointer<ModelParameter> modelParameter, 
        QSharedPointer<Port> parsedPort);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! All parsed generics.
     QList< QSharedPointer<ModelParameter> > parsedGenerics_;

    //! Maps a generic to ports depending on it.
    QMap< QSharedPointer<ModelParameter>, QList< QSharedPointer<Port> > > dependedGenerics_;
   
    //! All parsed ports and their declarations in source file.
    QMap<QSharedPointer<Port>, QString> parsedPortDeclarations_;

    //! Display widget for the source file content.
    QPlainTextEdit* display_;

    //! The selection highlighter for the display widget.
    VHDLHighlighter* highlighter_;

    //! Parser for ports.
    VHDLPortParser portParser_;

    //! Parser for generics.
    VHDLGenericParser genericParser_;

    //! The component to which add all parsed elements.
    QSharedPointer<Component> targetComponent_;
};

#endif // VhdlParser_H
