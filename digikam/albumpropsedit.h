/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-03-09
 * Description : Album properties dialog.
 *
 * Copyright (C) 2003-2004 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2005 by Tom Albers <tomalbers@kde.nl>
 * Copyright (C) 2006-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef ALBUMPROPSEDIT_H
#define ALBUMPROPSEDIT_H

// Qt includes.

#include <QDateTime>
#include <QString>
#include <QStringList>

// KDE includes.

#include <kdialog.h>

namespace Digikam
{

class PAlbum;
class AlbumPropsEditPriv;

class AlbumPropsEdit : public KDialog
{
    Q_OBJECT

public:

    AlbumPropsEdit(PAlbum* album, bool create=false);
    ~AlbumPropsEdit();

    QString     title() const;
    QString     comments() const;
    QDate       date() const;
    QString     category() const;
    QStringList albumCategories() const;

    static bool editProps(PAlbum*      album,
                          QString&     title,
                          QString&     comments,
                          QDate&       date,
                          QString&     category,
                          QStringList& albumCategories);

    static bool createNew(PAlbum*      parent,
                          QString&     title,
                          QString&     comments,
                          QDate&       date,
                          QString&     category,
                          QStringList& albumCategories);

private Q_SLOTS:

    void slotTitleChanged(const QString& newtitle);
    void slotDateLowButtonClicked();
    void slotDateAverageButtonClicked();
    void slotDateHighButtonClicked();

private:

    AlbumPropsEditPriv* const d;
};

}  // namespace Digikam

#endif /* ALBUMPROPSEDIT_H */
