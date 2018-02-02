/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 04.10.2009
 * Description : A tool for importing images via KIO
 *
 * Copyright (C) 2009      by Johannes Wienke <languitar at semipol dot de>
 * Copyright (C) 2011-2018 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "kioimportwindow.h"

// Qt includes

#include <QAction>
#include <QMenu>
#include <QMessageBox>

// KDE includes

#include <kio/job.h>
#include <kio/copyjob.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "kioimportwidget.h"
#include "dimageslist.h"

namespace Digikam
{

KioImportWindow::KioImportWindow(DInfoInterface* const iface, QWidget* const /*parent*/)
    : WSToolDialog(0)
{
    m_iface        = iface;
    m_importWidget = new KioImportWidget(this, m_iface);
    setMainWidget(m_importWidget);

    // window setup
    setWindowTitle(i18n("Import from Remote Storage"));
    setModal(false);
    startButton()->setEnabled(false);

    startButton()->setText(i18n("Start import"));
    startButton()->setToolTip(i18n("Start importing the specified images "
                                   "into the currently selected album."));

    // connections

    connect(startButton(), SIGNAL(clicked()),
            this, SLOT(slotImport()));

    connect(m_importWidget->imagesList(), SIGNAL(signalImageListChanged()),
            this, SLOT(slotSourceAndTargetUpdated()));

    connect(m_importWidget->uploadWidget(), SIGNAL(selectionChanged()),
            this, SLOT(slotSourceAndTargetUpdated()));

    slotSourceAndTargetUpdated();
}

KioImportWindow::~KioImportWindow()
{
}

void KioImportWindow::slotImport()
{
    int a = m_iface->albumSelectorItem();

    if (a)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "starting to import urls: " << m_importWidget->sourceUrls();

        // start copying and react on signals
        setEnabled(false);
    
        DAlbumInfo info(m_iface->albumInfo(a));

        KIO::CopyJob* const copyJob = KIO::copy(m_importWidget->imagesList()->imageUrls(),
                                                QUrl::fromLocalFile(info.path()));

        connect(copyJob, SIGNAL(copyingDone(KIO::Job*, QUrl, QUrl, QDateTime, bool, bool)),
                this, SLOT(slotCopyingDone(KIO::Job*, QUrl, QUrl, QDateTime, bool, bool)));

        connect(copyJob, SIGNAL(result(KJob*)),
                this, SLOT(slotCopyingFinished(KJob*)));
    }
}

void KioImportWindow::slotCopyingDone(KIO::Job* job, const QUrl& from, const QUrl& to,
                                      const QDateTime& mtime, bool directory, bool renamed)
{
    Q_UNUSED(job);
    Q_UNUSED(to);
    Q_UNUSED(mtime);
    Q_UNUSED(directory);
    Q_UNUSED(renamed);

    qCDebug(DIGIKAM_GENERAL_LOG) << "copied " << to.toDisplayString();

    m_importWidget->imagesList()->removeItemByUrl(from);
}

void KioImportWindow::slotCopyingFinished(KJob* job)
{
    Q_UNUSED(job);

    setEnabled(true);

    if (!m_importWidget->imagesList()->imageUrls().empty())
    {
        QMessageBox::information(this, i18n("Import not completed"),
                                 i18n("Some of the images have not been transferred "
                                      "and are still in the list. "
                                      "You can retry to import these images now."));
    }
}

void KioImportWindow::slotSourceAndTargetUpdated()
{
    bool hasUrlToImport = !m_importWidget->sourceUrls().empty();
    bool hasTarget      = false;
    int a               = m_iface->albumSelectorItem();

    if (a)
    {
        DAlbumInfo info(m_iface->albumInfo(a));
        hasTarget      = !info.path().isEmpty();
    }

    qCDebug(DIGIKAM_GENERAL_LOG) << "switching import button activity with: hasUrlToImport = "
                                 << hasUrlToImport << ", hasTarget = " << hasTarget;

    startButton()->setEnabled(hasUrlToImport && hasTarget);
}

} // namespace Digikam
