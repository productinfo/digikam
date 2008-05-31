/* ============================================================
 *
 * This file is a part of digiKam project
 * http://www.digikam.org
 *
 * Date        : 2003-01-17
 * Description : Haar Database interface
 *
 * Copyright (C) 2003 by Ricardo Niederberger Cabral <nieder at mail dot ru>
 * Copyright (C) 2008 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2008 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
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

// C++ Includes

#include <fstream>
#include <cmath>
#include <cstring>

// QT Includes

#include <QByteArray>
#include <QDataStream>
#include <QImage>
#include <QImageReader>
#include <QMap>

// KDE includes

#include <kurl.h>

// Local includes.

#include "ddebug.h"
#include "jpegutils.h"
#include "dimg.h"
#include "imageinfo.h"
#include "databaseaccess.h"
#include "albumdb.h"
#include "databasebackend.h"
#include "haar.h"
#include "haariface.h"

using namespace std;

namespace Digikam
{

/** This class encapsulates the Haar signature in a QByteArray
 *  that can be stored as a BLOB in the database.
 *
 *  Reading and writing is done in a platform-independent manner, which
 *  induces a certain overhead, but which is necessary IMO. 
 */
class DatabaseBlob
{

public:

    enum { Version = 1 };

public:

    DatabaseBlob(){}

    /** Read the QByteArray into the Haar::SignatureData. 
     */
    void read(const QByteArray &array, Haar::SignatureData *data)
    {
        QDataStream stream(array);

        // check version
        qint32 version;
        stream >> version;
        if (version != Version)
        {
            DError() << "Unsupported binary version of Haar Blob in database";
            return;
        }

        // read averages
        for (int i=0; i<3; i++)
            stream >> data->avg[i];

        // read coefficients
        for (int i=0; i<3; i++)
            stream.readRawData((char*)data->sig[i], sizeof(qint32[Haar::NumberOfCoefficients]));
    }

    QByteArray write(Haar::SignatureData *data)
    {
        QByteArray array;
        array.reserve(sizeof(qint32) + 3*sizeof(double) + 3*sizeof(qint32)*Haar::NumberOfCoefficients);
        QDataStream stream(&array, QIODevice::WriteOnly);

        // write version
        stream << (qint32)Version;

        // write averages
        for (int i=0; i<3; i++)
            stream << data->avg[i];

        // write coefficients
        for (int i=0; i<3; i++)
            stream.writeRawData((char*)data->sig[i], sizeof(qint32[Haar::NumberOfCoefficients]));

        return array;
    }
};

class HaarIfacePriv
{

public:

    HaarIfacePriv()
    {
        data = 0;
        bin  = 0;
    }

    ~HaarIfacePriv()
    {
        delete data;
        delete bin;
    }

    void createLoadingBuffer()
    {
        if (!data)
            data = new Haar::ImageData;
    }

    void createWeightBin()
    {
        if (!bin)
            bin = new Haar::WeightBin;
    }

    Haar::ImageData *data;
    Haar::WeightBin *bin;
};

HaarIface::HaarIface()
{
    d = new HaarIfacePriv();
}

HaarIface::~HaarIface()
{
    delete d;
}

int HaarIface::preferredSize()
{
    return Haar::NumberOfPixels;
}

bool HaarIface::indexImage(const QString& filename)
{
    QImage image = loadQImage(filename);
    if (image.isNull())
        return false;
    return indexImage(filename, image);
}

bool HaarIface::indexImage(const QString& filename, const QImage &image)
{
    ImageInfo info(KUrl::fromPath(filename));
    if (info.isNull())
        return false;
    return indexImage(info.id(), image);
}

bool HaarIface::indexImage(const QString& filename, const DImg &image)
{
    ImageInfo info(KUrl::fromPath(filename));
    if (info.isNull())
        return false;
    return indexImage(info.id(), image);
}

bool HaarIface::indexImage(qlonglong imageid, const QImage& image)
{
    if (image.isNull())
        return false;

    d->createLoadingBuffer();
    d->data->fillPixelData(image);

    return indexImage(imageid);
}

bool HaarIface::indexImage(qlonglong imageid, const DImg& image)
{
    if (image.isNull())
        return false;

    d->createLoadingBuffer();
    d->data->fillPixelData(image);

    return indexImage(imageid);
}

// private method: d->data has been filled
bool HaarIface::indexImage(qlonglong imageid)
{
    Haar::Calculator haar;
    haar.transform(d->data);

    Haar::SignatureData sig;
    haar.calcHaar(d->data, &sig);

    DatabaseAccess access;

    // Store main entry
    {
        // prepare blob
        DatabaseBlob blob;
        QByteArray array = blob.write(&sig);

        access.backend()->execSql(QString("REPLACE INTO ImageHaarMatrix "
                                          " (imageid, modificationDate, uniqueHash, matrix) "
                                          " SELECT id, modificationDate, uniqueHash, ? "
                                          "  FROM Images WHERE id=?; "),
                                  array, imageid);
    }

    return true;
}

QString HaarIface::signatureAsText(const QImage &image)
{
    d->createLoadingBuffer();
    d->data->fillPixelData(image);

    Haar::Calculator haar;
    haar.transform(d->data);
    Haar::SignatureData sig;
    haar.calcHaar(d->data, &sig);

    DatabaseBlob blob;
    QByteArray array = blob.write(&sig);

    return array.toBase64();
}

QList<qlonglong> HaarIface::bestMatchesForImage(const QImage& image, int numberOfResults, SketchType type)
{
    d->createLoadingBuffer();
    d->data->fillPixelData(image);

    Haar::Calculator haar;
    haar.transform(d->data);
    Haar::SignatureData sig;
    haar.calcHaar(d->data, &sig);

    return bestMatches(&sig, numberOfResults, type);
}

QList<qlonglong> HaarIface::bestMatchesForImage(qlonglong imageid, int numberOfResults, SketchType type)
{
    QList<QVariant> values;
    DatabaseAccess().backend()->execSql(QString("SELECT matrix FROM ImageHaarMatrix WHERE imageid=?"),
                                        imageid, &values);

    if (values.isEmpty())
        return QList<qlonglong>();

    DatabaseBlob blob;
    Haar::SignatureData sig;

    blob.read(values.first().toByteArray(), &sig);

    return bestMatches(&sig, numberOfResults, type);
}

QList<qlonglong> HaarIface::bestMatchesForFile(const QString& filename, int numberOfResults, SketchType type)
{
    QImage image = loadQImage(filename);
    if (image.isNull())
        return QList<qlonglong>();

    return bestMatchesForImage(image, numberOfResults, type);
}

QList<qlonglong> HaarIface::bestMatchesForSignature(const QString& signature, int numberOfResults, SketchType type)
{
    QByteArray bytes = QByteArray::fromBase64(signature.toAscii());

    DatabaseBlob blobReader;
    Haar::SignatureData sig;
    blobReader.read(bytes, &sig);

    return bestMatches(&sig, numberOfResults, type);
}

QList<qlonglong> HaarIface::bestMatches(Haar::SignatureData *querySig, int numberOfResults, SketchType type)
{
    d->createWeightBin();
    // The table of constant weight factors applied to each channel and bin
    Haar::Weights weights((Haar::Weights::SketchType)type);

    // layout the query signature for fast lookup
    Haar::SignatureMap queryMapY, queryMapI, queryMapQ;
    queryMapY.fill(querySig->sig[0]);
    queryMapI.fill(querySig->sig[1]);
    queryMapQ.fill(querySig->sig[2]);
    Haar::SignatureMap *queryMaps[3] = { &queryMapY, &queryMapI, &queryMapQ };

    // Map imageid -> score. Lowest score is best.
    // any newly inserted value will be initialized with a score of 0, as required
    QMap<qlonglong, float> scores;

    // Variables for data read from DB
    DatabaseBlob blob;
    qlonglong imageid;
    Haar::SignatureData targetSig;

    DatabaseAccess access;
    QSqlQuery query;
    query = access.backend()->prepareQuery(QString("SELECT imageid, matrix FROM ImageHaarMatrix"));
    if (!access.backend()->exec(query))
        return QList<qlonglong>();

    // We don't use DatabaseBackend's convenience calls, as the result set is large
    // and we try to avoid copying in a temporary QList<QVariant>
    while (query.next())
    {
        imageid = query.value(0).toLongLong();
        blob.read(query.value(1).toByteArray(), &targetSig);

        // this is a reference
        float &score = scores[imageid];

        // Step 1: Initialize scores with average intensity values of all three channels
        for (int channel=0; channel<3; channel++)
        {
            score += weights.weightForAverage(channel) * fabs( querySig->avg[channel] - targetSig.avg[channel] );
        }

        // Step 2: Decrease the score if query and target have significant coefficients in common
        for (int channel=0; channel<3; channel++)
        {
            Haar::Idx *sig = targetSig.sig[channel];
            Haar::SignatureMap *queryMap = queryMaps[channel];
            int x;
            for (int coef = 0; coef < Haar::NumberOfCoefficients; coef++)
            {
                // x is a pixel index, either positive or negative, 0..16384
                x = sig[coef];
                // If x is a significant coefficient with the same sign in the query signature as well,
                // descrease the score (lower is better)
                // Note: both method calls called with x accept positive or negative values
                if ((*queryMap)[x])
                    score -= weights.weight(d->bin->binAbs(x), channel);
            }
        }
    }

    // Find out the best matches, those with the lowest score
    // We make use of the feature that QMap keys are sorted in ascending order
    // Of course, images can have the same score, so we need a multi map
    QMultiMap<float, qlonglong> bestMatches;
    bool initialFill = false;
    float score, worstScore, bestScore;
    qlonglong id;
    for (QMap<qlonglong, float>::iterator it = scores.begin(); it != scores.end(); ++it)
    {
        score = it.value();
        id    = it.key();

        if (!initialFill)
        {
            // as long as the maximum number of results is not reached, just fill up the map
            bestMatches.insert(score, id);
            initialFill = (bestMatches.size() >= numberOfResults);
        }
        else
        {
            // find the last entry, the one with the highest (=worst) score
            QMap<float, qlonglong>::iterator last = bestMatches.end();
            --last;
            worstScore = last.key();
            // if the new entry has a higher score, put it in the list and remove that last one
            if (score < worstScore)
            {
                bestMatches.erase(last);
                bestMatches.insert(score, id);
            }
            else if (score == worstScore)
            {
                bestScore = bestMatches.begin().key();
                // if the score is identical for all entries, increase the maximum result number
                if (score == bestScore)
                    bestMatches.insert(score, id);
            }
        }
    }
    for (QMap<float, qlonglong>::iterator it = bestMatches.begin(); it != bestMatches.end(); ++it)
        DDebug() << it.key() << it.value();

    return bestMatches.values();
}

QImage HaarIface::loadQImage(const QString &filename)
{
    // TODO: Can be optimized using DImg.

    QImage image;
    if (isJpegImage(filename))
    {
        // use fast jpeg loading
        if (!loadJPEGScaled(image, filename, Haar::NumberOfPixels))
        {
            // try QT now.
            if (!image.load(filename))
                return QImage();
        }
    }
    else
    {
        // use default QT image loading
        if (!image.load(filename))
            return QImage();
    }

    return image;
}

QMap< qlonglong, QList<qlonglong> > HaarIface::findDuplicates(const QList<qlonglong>& images2Scan)
{
    QMap< qlonglong, QList<qlonglong> > resultsMap;
    QList<qlonglong>::const_iterator    it = images2Scan.begin();
    QList<qlonglong>::const_iterator    it2;
    QList<qlonglong>                    list;
    bool                                find = false;

    while (it != images2Scan.end())
    {
        list = bestMatchesForImage(*it, 20, ScannedSketch);
        if (!list.isEmpty())
        {
            // we will check if the duplicates already exist in the map.
            find = false;
            for (it2 = list.begin(); it2 != list.end(); ++it2)
            {
                if (resultsMap.find(*it2) != resultsMap.end())
                {
                    find = true;
                    break;
                }
            }

            if (!find)
                resultsMap.insert(*it, list);

            ++it;
        }
    };

    return resultsMap;
}

}  // namespace Digikam
