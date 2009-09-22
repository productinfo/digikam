/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2009-08-11
 * Description : the main parser object for manual rename
 *
 * Copyright (C) 2009 by Andi Clemens <andi dot clemens at gmx dot net>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "parser.h"

// Qt includes

#include <QFileInfo>

namespace Digikam
{

class ParserPriv
{
public:

    ParserPriv()
    {
    }
    SubParserList subparsers;
};

// --------------------------------------------------------

Parser::Parser()
      : d(new ParserPriv)
{
}

Parser::~Parser()
{
    foreach (SubParser* subparser, d->subparsers)
    {
        delete subparser;
    }
    d->subparsers.clear();
}

bool Parser::stringIsValid(const QString& str)
{
    QRegExp invalidString("^\\s*$");
    if (str.isEmpty() || invalidString.exactMatch(str))
        return false;
    return true;
}

SubParserList Parser::subParsers() const
{
    return d->subparsers;
}

ModifierList Parser::modifiers() const
{
    ModifierList modifierList;
    if (!d->subparsers.isEmpty())
    {
        modifierList = d->subparsers.first()->modifiers();
    }
    return modifierList;
}

void Parser::registerSubParser(SubParser* parser)
{
    if (!parser)
        return;

    if (!parser->isValid())
        return;

    d->subparsers.append(parser);
}

ParseResults Parser::parseResults(const QString& parseString)
{
    ParseResults results;
    ParseInformation info;

    parseOperation(parseString, info, results, false);
    return results;
}

ParseResults Parser::modifierResults(const QString& parseString)
{
    ParseResults results;
    ParseInformation info;

    parseOperation(parseString, info, results, true);
    return results;
}

QString Parser::parse(const QString& parseString, ParseInformation& info)
{
    ParseResults results;
    return parseOperation(parseString, info, results);
}

QString Parser::parseOperation(const QString& parseString, ParseInformation& info, ParseResults& results,
                               bool replace)
{
    if (!stringIsValid(parseString))
    {
        QFileInfo fi(info.filePath);
        QString baseName = fi.baseName();
        return baseName;
    }

    if (!d->subparsers.isEmpty())
    {
        foreach (SubParser* parser, d->subparsers)
        {
            parser->parse(parseString, info);

            ParseResults r;
            if (replace)
                r = parser->modifiedResults();
            else
                r = parser->parseResults();

            results.append(r);
        }
    }

    QString parsed;
    if (replace)
        parsed = results.replaceTokens(parseString);

    return parsed;
}

bool Parser::tokenAtPosition(const QString& parseString, int pos)
{
    int start;
    int length;
    return tokenAtPosition(parseString, pos, start, length);
}

bool Parser::tokenAtPosition(const QString& parseString, int pos, int& start, int& length)
{
    bool found = false;

    ParseResults results         = parseResults(parseString);
    ParseResults::ResultsKey key = results.keyAtApproximatePosition(pos);
    start  = key.first;
    length = key.second;

    if ((pos >= start) && (pos <= start + length))
    {
        found = true;
    }
    return found;
}

bool Parser::tokenModifierAtPosition(const QString& parseString, int pos)
{
    int start;
    int length;
    return tokenAtPosition(parseString, pos, start, length);
}

bool Parser::tokenModifierAtPosition(const QString& parseString, int pos, int& start, int& length)
{
    bool found = false;

    ParseResults results         = modifierResults(parseString);
    ParseResults::ResultsKey key = results.keyAtApproximatePosition(pos);
    start  = key.first;
    length = key.second;

    if ((pos >= start) && (pos <= start + length))
    {
        found = true;
    }
    return found;
}

}  // namespace Digikam
