/*! \file ezxml.h
 * \brief ezXML - Easy XML Parsing C Library
 *
 * ezXML is a C library for parsing XML documents. It's easy to use.
 * It's ideal for parsing XML configuration files or REST web service responses.
 * It's also fast and lightweight (less than 20k compiled).
 *
 * \section ezxml_usage Usage
 *
 * Given the following example XML string stored in <i>xml_buf</i>:
 * \code
 * <?xml version="1.0"?>
 * <formula1>
 *   <team name="McLaren">
 *       <driver>
 *             <name>Kimi Raikkonen</name>
 *             <points>112</points>
 *       </driver>
 *       <driver>
 *             <name>Juan Pablo Montoya</name>
 *             <points>60</points>
 *       </driver>
 *   </team>
 * </formula1>
 * \endcode
 *
 * This code snippet prints out a list of drivers, which team they drive for,
 * and how many championship points they have:
 *
 * \code
 * ezxml_t f1 = ezxml_parse_str(xml_buf, strlen(xml_buf));
 * const char *teamname, *team, *driver;
 *
 * for (team = ezxml_child(f1, "team"); team; team = team->next) {
 *     teamname = ezxml_attr(team, "name");
 *     for (driver = ezxml_child(team, "driver"); driver; driver = driver->next) {
 *         wmprintf("%s, %s: %s\n", ezxml_child(driver, "name")->txt, teamname,
 *                 ezxml_child(driver, "points")->txt);
 *     }
 * }
 *
 * ezxml_free(f1);
 * \endcode
 *
 * Alternately, the following would print out the name of the second driver on the
 * first team:
 *
 * \code
 * ezxml_t f1 = ezxml_parse_str(xml_buf, strlen(xml_buf));
 *
 * wmprintf("%s\n", ezxml_get(f1, "team", 0, "driver", 1, "name", -1)->txt);
 * ezxml_free(f1);
 *\endcode
 *
 * The -1 indicates the end of the argument list.
 *
 */

/* ezxml.h
 *
 * Copyright 2004-2006 Aaron Voisine <aaron@voisine.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* ezXML -- Easy XML Parsing C Library */

#ifndef _EZXML_H
#define _EZXML_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <wm_os.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Ezxml Error Codes */
enum wm_exml_errno {
	WM_E_EXML_ERRNO_BASE = MOD_ERROR_START(MOD_EXML),
	/** EXML buffer null */
	WM_E_EXML_INBUF,
	/** Invalid length*/
	WM_E_EXML_INLEN,
};

#define EZXML_NAMEM   0x80
#define EZXML_TXTM    0x40
#define EZXML_DUP     0x20

	typedef struct ezxml *ezxml_t;
	struct ezxml {
    /** tag name */
		char *name;
    /** tag attributes { name, value, name, value, ... NULL } */
		char **attr;
    /** tag character content, empty string if none */
		char *txt;
    /** tag offset from start of parent tag character content */
		size_t off;
    /** next tag with same name in this section at this depth */
		ezxml_t next;
    /** next tag with different name in same section and depth */
		ezxml_t sibling;
    /** next tag, same section and depth, in original order */
		ezxml_t ordered;
    /** head of sub tag list, NULL if none */
		ezxml_t child;
    /** parent tag, NULL if current tag is root tag */
		ezxml_t parent;
    /** additional information */
		short flags;
	};

char *ezxml_mem_strdup(const char *s);
/** Given a string of xml data and its length, parses it and creates an ezxml
 * structure. For efficiency, modifies the data by adding null terminators
 * and decoding ampersand sequences. If you don't want this, copy the data and
 * pass the copy. Returns NULL on failure.
 *
 * \param s String of xml data
 * \param len Length of the xml data string
 *
 * \return ezxml node if successful, NULL otherwise
 *
 */
	ezxml_t ezxml_parse_str(char *s, size_t len);

/** Get first child tag with the given name.
 *
 * \param xml ezxml node
 * \param name Name of the child tag
 *
 * \return First child tag (one level deeper) with the given name or NULL if not found
 *
 */
	ezxml_t ezxml_child(ezxml_t xml, const char *name);

/** \return Next tag of the same name in the same section and depth or NULL if not found
 *
 */
#define ezxml_next(xml) ((xml) ? xml->next : NULL)

/**
 * \param[in] xml The xml node.
 * \param[in] idx The current index.
 *
 * \return Nth tag with the same name in the same section at the same
 *   depth or NULL if not found. An index of 0 returns the tag given
 */
	ezxml_t ezxml_idx(ezxml_t xml, int idx);

/** \return Name of the given tag
 */
#define ezxml_name(xml) ((xml) ? xml->name : NULL)

/** \return Given tag's character content or empty string if none
 */
#define ezxml_txt(xml) ((xml) ? xml->txt : "")

/** Traverses the ezxml structure to retrieve the value of requested tag
 * attribute.
 *
 * \param xml ezxml node
 * \param attr Attribute name
 *
 * \return Value of the requested tag attribute, or NULL if not found
 */
	const char *ezxml_attr(ezxml_t xml, const char *attr);

/** Traverses the ezxml structure to retrieve a specific subtag. Takes a
 * variable length list of tag names and indices. The argument list must be
 * terminated by either an index of -1 or an empty string tag name.
 *
 * \note Example: title = ezxml_get(library, "shelf", 0, "book", 2, "title", -1);
 * This retrieves the title of the 3rd book on the 1st shelf of library.
 *
 * \param xml ezxml node
 *
 * \return Specific subtag, NULL if not found
 */
	ezxml_t ezxml_get(ezxml_t xml, ...);

/** Converts an ezxml structure back to xml.
 *
 * \param xml ezxml node
 *
 * \return A string of xml data that must be freed
 *
 */
	char *ezxml_toxml(ezxml_t xml);

/**
 * \param[in] xml The xml node
 * \param[in] target The target string.
 *
 * \return A NULL terminated array of processing instructions for the given
 * target.
 */
	const char **ezxml_pi(ezxml_t xml, const char *target);

/** Frees the memory allocated to an ezxml node and it's children.
 *
 * \param xml ezxml node
 *
 * \return No value
 *
 */
	void ezxml_free(ezxml_t xml);

/**
 * \param[in] xml The xml node
 *
 * \return Parser error message or empty string if none
 */
	const char *ezxml_error(ezxml_t xml);

/**
 * Creates a new empty ezxml node with the given root tag name.
 *
 * \param[in] name Name for the ezxml node.
 *
 * \return A new empty ezxml node
 */
	ezxml_t ezxml_new(const char *name);

/** Wrapper for ezxml_new() that strdups name.
 *
 */
#define ezxml_new_d(name) \
	ezxml_set_flag(ezxml_new(ezxml_mem_strdup(name)), EZXML_NAMEM)

/** Adds a child ezxml node.
 *
 * \param xml parent ezxml node
 * \param name Name of the child node
 * \param off Offset of the child tag relative to the start of the parent tag's character content
 * \return Added ezxml node
 *
 */
	ezxml_t ezxml_add_child(ezxml_t xml, const char *name, size_t off);

/** Wrapper for ezxml_add_child() that strdups name.
 *
 */
#define ezxml_add_child_d(xml, name, off) \
	ezxml_set_flag(ezxml_add_child(xml, ezxml_mem_strdup(name), off), \
		   EZXML_NAMEM)

/** Sets the character content for the given ezxml node.
 *
 * \param xml ezxml node
 * \param txt Character content to be set
 *
 * \return Modified ezxml node
 *
 */
	ezxml_t ezxml_set_txt(ezxml_t xml, const char *txt);

/** Wrapper for ezxml_set_txt() that strdups txt.
 *
 */
#define ezxml_set_txt_d(xml, txt) \
	ezxml_set_flag(ezxml_set_txt(xml, ezxml_mem_strdup(txt)), EZXML_TXTM)

/** Sets the given tag attribute or adds a new attribute if not found. A value
 *  of NULL will remove the specified attribute.
 *
 * \param xml ezxml node
 * \param name Attribute name
 * \param value Attribute value
 *
 * \return ezxml node with modified attribute
 *
 */
ezxml_t ezxml_set_attr(ezxml_t xml, const char *name, const char *value);

/** Wrapper for ezxml_set_attr() that strdups name/value. value cannot be NULL.
 *
 */
#define ezxml_set_attr_d(xml, name, value) \
	ezxml_set_attr(ezxml_set_flag(xml, EZXML_DUP), \
		       ezxml_mem_strdup(name), ezxml_mem_strdup(value))

/** Sets a flag for the given ezxml node.
 *
 * \param xml ezxml node
 * \param flag Flag to be set
 *
 * \return Modified ezxml node
 *
 */
	ezxml_t ezxml_set_flag(ezxml_t xml, short flag);

/** Removes a tag along with its subtags without freeing its memory.
 *
 * \param xml ezxml node
 *
 * \return Removed ezxml node
 *
 */
	ezxml_t ezxml_cut(ezxml_t xml);

/** Inserts an existing tag into an ezxml structure.
 *
 * \param xml ezxml node
 * \param dest ezxml node to be inserted
 * \param off Offset of the tag relative to the start of the dest tag's character content
 *
 * \return Inserted ezxml node
 *
 */
	ezxml_t ezxml_insert(ezxml_t xml, ezxml_t dest, size_t off);

/** Moves an existing tag to become a subtag of dest at the given offset from
 * the start of dest's character content.
 *
 * \param xml ezxml node
 * \param dest ezxml node to be moved
 * \param off Offset of the tag relative to the start of the dest tag's character content
 *
 * \return Moved ezxml node
 *
 */
#define ezxml_move(xml, dest, off) ezxml_insert(ezxml_cut(xml), dest, off)

/** Removes a tag along with all its subtags.
 *
 */
#define ezxml_remove(xml) ezxml_free(ezxml_cut(xml))

	int ezxml_cli_init(void);

#ifdef __cplusplus
}
#endif
#endif				// _EZXML_H
