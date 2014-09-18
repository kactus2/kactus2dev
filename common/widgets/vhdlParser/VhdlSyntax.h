/* 
 *
 *  Created on: 4.7.2013
 *      Author: Esko Pekkarinen
 * 		filename: VhdlSyntax.h
 * 		
 * 		Description: This file contains common elements of the vhdl language.
 */

#ifndef VHDLSYNTAX_H
#define VHDLSYNTAX_H

#include <QRegExp>


namespace VhdlSyntax {

    //! Whitespace without line end.
    const QString SPACE = "[ \\t]*";    

    //! Platform-independent endline.
    const QString ENDLINE = "(\\r\\n?|\\n\\r?)";

    //! VHDL comment.
    const QString COMMENT = "[-][-]" + SPACE + "([^\\r\\n]*)(?=" + ENDLINE + "|$)";
   
    //! VHDL port and generic names are separated using a colon (,).
    const QString NAMES = "\\w+(?:\\s*[,]?\\s*\\w+)*";

    //! Supported mathematical operations in math expressions.
    const QString OPERATIONS = "[+-/*/]|[/*][/*]";

    //! Supported std_logic values.
    const QString STD_ULOGIC = "['][U|X|0|1|Z|W|L|H|-][']";

    //! Others is used to initialize a group of bits in std_logic_vector.
    const QString OTHERS_EXP = "[(]\\s*others\\s*=>\\s*" + STD_ULOGIC + "\\s*[)]";

    //! Mathematical expressions e.g. x or x + y.
    const QString MATH_EXP = "(?:\\w+)(?:\\s*(?:" + OPERATIONS + ")\\s*(?:\\w+))*";

    //! VHDL port type definition is <typename>[(<left> downto <right>)].
    const QString PORT_TYPE = "(?:\\w+)(?:\\s*[(]\\s*(?:" + VhdlSyntax::MATH_EXP +
        ")\\s+\\w+\\s+(?:" + VhdlSyntax::MATH_EXP + ")\\s*[)])?";

    //! Type definition in port declaration is <typename>[(<left> downto <right>)].
    const QRegExp TYPE_EXP = QRegExp(PORT_TYPE);

    //! VHDL default value definition is mathematical expression, vector assignment with others, logical value
    // or other string value.
    const QString DEFAULT = "[:][=]\\s*((?:" + MATH_EXP + ")|" + OTHERS_EXP + "|[^\\s:;]+)";

    //! VHDL pragmas begin like comments with -- followed by a reserved word.
    const QString PRAGMA = "[-][-]" + SPACE + "(?:PRAGMA|SYNTHESIS|SYNOPSYS)[^\\r\\n]*";

    //! VHDL comment line.
    const QRegExp COMMENT_LINE_EXP = QRegExp(SPACE + COMMENT + ENDLINE);

    /*! All except the last port/generic declaration ends with a semicolon. 
     *  If there is a comment on the same line, it will be captured to the description by VhdlParser.
     *  Additional comments may follow the last declaration before the end of the string.
     */
    const QString DECLARATION_END = "(?:\\s*[;]" + SPACE + "(?:" + COMMENT + "))|"
                                    "(?:(?=\\s*[;]))|"
                                    "(?:" + SPACE + "(?:" + COMMENT + ")?(?=(?:\\s*(?:" + COMMENT + ")*\\s*)*$))";
    
    //! Entity declaration is ENTITY <name> IS ... END [ENTITY] [<name>];
    const QRegExp ENTITY_BEGIN_EXP = QRegExp("(?:ENTITY)\\s+(\\w+)\\s+(?:IS)", Qt::CaseInsensitive);   
    const QRegExp ENTITY_END_EXP = QRegExp("(END)\\s*(ENTITY)?\\s*(\\w+)?\\s*[;]", Qt::CaseInsensitive);



}

#endif // VHDLSYNTAX_H
