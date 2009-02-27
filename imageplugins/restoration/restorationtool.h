/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2005-03-26
 * Description : a digiKam image editor plugin to restore
 *               a photograph
 *
 * Copyright (C) 2005-2009 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef RESTORATIONTOOL_H
#define RESTORATIONTOOL_H

// Qt includes.

#include <QString>

// Local includes.

#include "editortool.h"

class KComboBox;
class KTabWidget;

namespace Digikam
{
class GreycstorationWidget;
class EditorToolSettings;
class ImagePanelWidget;
}

namespace DigikamRestorationImagesPlugin
{

class RestorationTool : public Digikam::EditorToolThreaded
{
    Q_OBJECT

public:

    RestorationTool(QObject* parent);
    ~RestorationTool();

private Q_SLOTS:

    void slotSaveAsSettings();
    void slotLoadSettings();
    void slotResetSettings();
    void processCImgUrl(const QString&);
    void slotResetValues(int);

private:

    void readSettings();
    void writeSettings();
    void prepareEffect();
    void prepareFinal();
    void putPreviewData();
    void putFinalData();
    void renderingFinished();

private:

    enum RestorationFilteringPreset
    {
        NoPreset=0,
        ReduceUniformNoise,
        ReduceJPEGArtefacts,
        ReduceTexturing
    };

    KTabWidget                    *m_mainTab;

    KComboBox                     *m_restorationTypeCB;

    Digikam::GreycstorationWidget *m_settingsWidget;

    Digikam::ImagePanelWidget     *m_previewWidget;

    Digikam::EditorToolSettings   *m_gboxSettings;
};

}  // namespace DigikamRestorationImagesPlugin

#endif /* RESTORATIONTOOL_H */
