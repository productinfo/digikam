/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-01-23
 * Description : A widget to display a list of camera folders.
 *
 * Copyright (C) 2003-2005 by Renchi Raju <renchi@pooh.tam.uiuc.edu>
 * Copyright (C) 2006-2011 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef CAMERAFOLDERVIEW_H
#define CAMERAFOLDERVIEW_H

// Qt includes

#include <QString>
#include <QPixmap>
#include <QTreeWidget>

// KDE includes

#include <kiconloader.h>

namespace Digikam
{

class CameraFolderItem;

class CameraFolderView : public QTreeWidget
{
    Q_OBJECT

public:

    CameraFolderView(QWidget* parent);
    ~CameraFolderView();

    void addVirtualFolder(const QString& name, const QPixmap& pixmap = SmallIcon("camera-photo"));
    void addRootFolder(const QString& folder, int nbItems = -1, const QPixmap& pixmap = SmallIcon("folder"));

    CameraFolderItem* addFolder(const QString& folder, const QString& subFolder, int nbItems,
                                const QPixmap& pixmap = SmallIcon("folder"));

    CameraFolderItem* findFolder(const QString& folderPath);

    CameraFolderItem* virtualFolder() const;
    CameraFolderItem* rootFolder() const;

    virtual void clear();

Q_SIGNALS:

    void signalFolderChanged(CameraFolderItem*);
    void signalCleared();

private Q_SLOTS:

    void slotCurrentChanged(QTreeWidgetItem*, int);

private:

    class CameraFolderViewPriv;
    CameraFolderViewPriv* const d;
};

} // namespace Digikam

#endif /* CAMERAFOLDERVIEW_H */
