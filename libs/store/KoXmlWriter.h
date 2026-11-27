/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2004 David Faure <faure@kde.org>
   SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2011 Lukáš Tvrdý <lukas.tvrdy@ixonos.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef XMLWRITER_H
#define XMLWRITER_H

#include <QDebug>
#include <QMap>
#include <QIODevice>

#include "kritastore_export.h"

class KRITASTORE_EXPORT KoXmlWriter
{
public:
    explicit KoXmlWriter(QIODevice* dev, int indentLevel = 0);

    ~KoXmlWriter();

    void startDocument(const char* rootElemName, const char* publicId = 0, const char* systemId = 0);

    void endDocument();

    void startElement(const char* tagName, bool indentInside = true);

    inline void addAttribute(const char* attrName, const QString& value) {
        addAttribute(attrName, value.toUtf8());
    }
    inline void addAttribute(const char* attrName, int value) {
        addAttribute(attrName, QByteArray::number(value));
    }
    inline void addAttribute(const char* attrName, uint value) {
        addAttribute(attrName, QByteArray::number(value));
    }
    inline void addAttribute(const char* attrName, bool value) {
        addAttribute(attrName, value ? "true" : "false");
    }
    void addAttribute(const char* attrName, double value);

    void addAttribute(const char* attrName, float value);

    void addAttribute(const char* attrName, const QByteArray& value);

    void addAttribute(const char* attrName, const char* value);

    void endElement();

    inline void addTextNode(const QString& str) {
        addTextNode(str.toUtf8());
    }

    void addTextNode(const QByteArray& cstr);

    void addTextNode(const char* cstr);

    void addCompleteElement(QIODevice* dev);

    void addManifestEntry(const QString& fullPath, const QString& mediaType);

private:
    struct Tag {
        Tag(const char *t = 0, bool ind = true)
            : hasChildren(false)
            , lastChildIsText(false)
            , openingTagClosed(false)
            , indentInside(ind)
        {
            tagName = new char[qstrlen(t) + 1];
            qstrcpy(tagName, t);
        }

        ~Tag() {
            delete[] tagName;
        }

        Tag(const Tag &original)
        {
            tagName = new char[qstrlen(original.tagName) + 1];
            qstrcpy(tagName, original.tagName);

            hasChildren = original.hasChildren;
            lastChildIsText = original.lastChildIsText;
            openingTagClosed = original.openingTagClosed;
            indentInside = original.indentInside;
        }

        char *tagName {0};
        bool hasChildren : 1;
        bool lastChildIsText : 1;
        bool openingTagClosed : 1;
        bool indentInside : 1;
    };

    void writeIndent();

    void writeCString(const char* cstr);

    void writeChar(char c);

    inline void closeStartElement(Tag& tag) {
        if (!tag.openingTagClosed) {
            tag.openingTagClosed = true;
            writeChar('>');
        }
    }
    char *escapeForXML(const char* source, int length) const;
    bool prepareForChild(bool indentInside = true);
    void prepareForTextNode();

    class Private;
    Private * const d;

    KoXmlWriter(const KoXmlWriter &);
    KoXmlWriter& operator=(const KoXmlWriter &);
};

#endif

