/* ============================================================
 * Authors: Renchi Raju <renchi@pooh.tam.uiuc.edu>
 *          Gilles Caulier <caulier dot gilles at kdemail dot net> 
 * Date   : 2004-09-17
 * Description : digital camera controller
 * 
 * Copyright 2004-2005 by Renchi Raju
 * Copyright 2006 by Gilles Caulier
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

#ifndef CAMERACONTROLLER_H
#define CAMERACONTROLLER_H

// Qt includes.

#include <qobject.h>
#include <qstring.h>

// Local includes.

#include "downloadsettingscontainer.h"
#include "gpiteminfo.h"

namespace Digikam
{

class CameraControllerPriv;

class CameraController : public QObject
{
    Q_OBJECT

public:

    CameraController(QWidget* parent, const QString& mode,
                     const QString& port, const QString& path);
    ~CameraController();

    void listFolders();
    void listFiles(const QString& folder);
    void getThumbnail(const QString& folder, const QString& file);
    void getExif(const QString& folder, const QString& file);
    void getCameraInformations();

    void downloadPrep();
    void download(DownloadSettingsContainer downloadSettings);

    void deleteFile(const QString& folder, const QString& file);

    void openFile(const QString& folder, const QString& file);
    
signals:

    void signalBusy(bool val);
    void signalInfoMsg(const QString& msg);
    void signalErrorMsg(const QString& msg);
    void signalCameraInformations(const QString& summary, const QString& manual, const QString& about);

    void signalConnected(bool val);
    void signalFolderList(const QStringList& folderList);
    void signalFileList(const GPItemInfoList& infoList);
    void signalDownloaded(const QString& folder, const QString& file, int status);
    void signalSkipped(const QString& folder, const QString& file);
    void signalDeleted(const QString& folder, const QString& file);
    void signalThumbnail(const QString& folder, const QString& file, const QImage& thumb);
    void signalExifFromFile(const QString& folder, const QString& file);
    void signalExifData(const QByteArray& exifData);
        
protected:

    void customEvent(QCustomEvent* e);
    
public slots:

    void slotCancel();
    void slotConnect();

private slots:

    void slotProcessNext();

private:

    CameraControllerPriv *d;
    
    friend class CameraThread;
};
    
}  // namespace Digikam

#endif /* CAMERACONTROLLER_H */
