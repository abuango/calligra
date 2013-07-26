/*
 *  Copyright (c) 2007 Boudewijn Rempt boud@valdyas.org
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_convolution_painter_test.h"

#include <qtest_kde.h>

#include <QBitArray>

#include <KoColor.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoColorSpaceTraits.h>

#include "kis_types.h"
#include "kis_paint_device.h"
#include "kis_convolution_painter.h"
#include "kis_convolution_kernel.h"
#include "kis_paint_device.h"
#include <kis_mask_generator.h>
#include "testutil.h"

KisPaintDeviceSP initAsymTestDevice(QRect &imageRect, int &pixelSize, QByteArray &initialData)
{
    KisPaintDeviceSP dev = new KisPaintDevice(KoColorSpaceRegistry::instance()->rgb8());
    pixelSize = dev->pixelSize();

    imageRect = QRect(0,0,5,5);
    initialData.resize(25 * pixelSize);

    quint8 *ptr = (quint8*) initialData.data();
    for(int i = 0; i < 25; i++) {
        KoColor pixel(QColor(i,i,i,255), dev->colorSpace());
        memcpy(ptr, pixel.data(), pixelSize);

        ptr += pixelSize;
    }

    dev->writeBytes((const quint8*)initialData.constData(), imageRect);

    return dev;
}

Matrix<qreal, 3, 3> initSymmFilter(qreal &offset, qreal &factor)
{
    Matrix<qreal, 3, 3> filter;
    filter(0,0) = 1.0 / 21;
    filter(0,1) = 3.0 / 21;
    filter(0,2) = 1.0 / 21;

    filter(1,0) = 3.0 / 21;
    filter(1,1) = 5.0 / 21;
    filter(1,2) = 3.0 / 21;

    filter(2,0) = 1.0 / 21;
    filter(2,1) = 3.0 / 21;
    filter(2,2) = 1.0 / 21;

    offset = 0.0;
    factor = 1.0;

    return filter;
}

Matrix<qreal, 3, 3> initAsymmFilter(qreal &offset, qreal &factor)
{
    Matrix<qreal, 3, 3> filter;
    filter(0,0) = -1.0;
    filter(1,0) = -2.0;
    filter(2,0) = -1.0;

    filter(0,1) = 0;
    filter(1,1) = 0;
    filter(2,1) = 0;

    filter(0,2) = 1.0;
    filter(1,2) = 2.0;
    filter(2,2) = 1.0;

    offset = 0.5;
    factor = 1.0;

    return filter;
}

void printPixel(QString prefix, int pixelSize, quint8 *data) {
    QString str = prefix;

    for(int i = 0; i < pixelSize; i++) {
        str += " ";
        str += QString::number(data[i]);
    }

    qDebug() << str;
}

void KisConvolutionPainterTest::testIdentityConvolution()
{
    QImage qimage(QString(FILES_DATA_DIR) + QDir::separator() + "hakonepa.png");

    KisPaintDeviceSP dev = new KisPaintDevice(KoColorSpaceRegistry::instance()->rgb8());
    dev->convertFromQImage(qimage, 0, 0, 0);

    KisConvolutionKernelSP kernel = new KisConvolutionKernel(3, 3, 0, 0);
    kernel->data()[0] = 0;
    kernel->data()[1] = 0;
    kernel->data()[2] = 0;
    kernel->data()[3] = 0;
    kernel->data()[4] = 1;
    kernel->data()[5] = 0;
    kernel->data()[6] = 0;
    kernel->data()[7] = 0;
    kernel->data()[8] = 0;
    KisConvolutionPainter gc(dev);
    gc.beginTransaction(0);
    gc.applyMatrix(kernel, dev, QPoint(0, 0), QPoint(0, 0), QSize(qimage.width(), qimage.height()));
    gc.deleteTransaction();

    QImage resultImage = dev->convertToQImage(0, 0, 0, qimage.width(), qimage.height());

    QPoint errpoint;
    if (TestUtil::compareQImages(errpoint, qimage, resultImage)) {
        resultImage.save("identity_convolution.png");
        QFAIL(QString("Identity kernel did change image, first different pixel: %1,%2 ").arg(errpoint.x()).arg(errpoint.y()).toLatin1());
    }
}

void KisConvolutionPainterTest::testSymmConvolution()
{
    qreal offset = 0.0;
    qreal factor = 1.0;
    Matrix<qreal, 3, 3> filter = initSymmFilter(offset, factor);

    QRect imageRect;
    int pixelSize = 0;
    QByteArray initialData;
    KisPaintDeviceSP dev = initAsymTestDevice(imageRect, pixelSize, initialData);


    KisConvolutionKernelSP kernel =
        KisConvolutionKernel::fromMatrix(filter, offset, factor);
    KisConvolutionPainter gc(dev);
    gc.beginTransaction(0);
    gc.applyMatrix(kernel, dev, imageRect.topLeft(), imageRect.topLeft(),
                   imageRect.size());
    gc.deleteTransaction();

    QByteArray resultData(initialData.size(), 0);
    dev->readBytes((quint8*)resultData.data(), imageRect);

    QCOMPARE(resultData, initialData);
}

void KisConvolutionPainterTest::testAsymmConvolutionImp(QBitArray channelFlags)
{
    qreal offset = 0.0;
    qreal factor = 1.0;
    Matrix<qreal, 3, 3> filter = initAsymmFilter(offset, factor);

    QRect imageRect;
    int pixelSize = -1;
    QByteArray initialData;
    KisPaintDeviceSP dev = initAsymTestDevice(imageRect, pixelSize, initialData);

    KisConvolutionKernelSP kernel =
        KisConvolutionKernel::fromMatrix(filter, offset, factor);
    KisConvolutionPainter gc(dev);
    gc.beginTransaction(0);
    gc.setChannelFlags(channelFlags);
    gc.applyMatrix(kernel, dev, imageRect.topLeft(), imageRect.topLeft(),
                   imageRect.size());
    gc.deleteTransaction();


    QByteArray resultData(initialData.size(), 0);
    dev->readBytes((quint8*)resultData.data(), imageRect);

    QRect filteredRect = imageRect.adjusted(1, 1, -1, -1);
    KoColor filteredPixel(QColor(120,120,120,128), dev->colorSpace());

    quint8 *srcPtr = (quint8*) initialData.data();
    quint8 *resPtr = (quint8*) resultData.data();

    for(int row = 0; row < imageRect.height(); row++) {
        for(int col = 0; col < imageRect.width(); col++) {

            bool isFiltered = filteredRect.contains(col, row);

            KoColor referencePixel(dev->colorSpace());
            for(int j = 0; j < pixelSize; j++) {
                referencePixel.data()[j] = isFiltered && channelFlags[j] ?
                    filteredPixel.data()[j] : srcPtr[j];
            }

            if(memcmp(resPtr, referencePixel.data(), pixelSize)) {
                printPixel("Actual:  ", pixelSize, resPtr);
                printPixel("Expected:", pixelSize, referencePixel.data());
                QFAIL("Failed to filter area");
            }

            srcPtr += pixelSize;
            resPtr += pixelSize;
        }
    }
}

void KisConvolutionPainterTest::testAsymmAllChannels()
{
    QBitArray channelFlags =
        KoColorSpaceRegistry::instance()->rgb8()->channelFlags(true, true);
    testAsymmConvolutionImp(channelFlags);
}

void KisConvolutionPainterTest::testAsymmSkipRed()
{
    QBitArray channelFlags =
        KoColorSpaceRegistry::instance()->rgb8()->channelFlags(true, true);
    channelFlags[2] = false;

    testAsymmConvolutionImp(channelFlags);
}

void KisConvolutionPainterTest::testAsymmSkipGreen()
{
    QBitArray channelFlags =
        KoColorSpaceRegistry::instance()->rgb8()->channelFlags(true, true);
    channelFlags[1] = false;

    testAsymmConvolutionImp(channelFlags);
}

void KisConvolutionPainterTest::testAsymmSkipBlue()
{
    QBitArray channelFlags =
        KoColorSpaceRegistry::instance()->rgb8()->channelFlags(true, true);
    channelFlags[0] = false;

    testAsymmConvolutionImp(channelFlags);
}

void KisConvolutionPainterTest::testAsymmSkipAlpha()
{
    QBitArray channelFlags =
        KoColorSpaceRegistry::instance()->rgb8()->channelFlags(true, true);
    channelFlags[3] = false;

    testAsymmConvolutionImp(channelFlags);
}


// #include <valgrind/callgrind.h>
void KisConvolutionPainterTest::benchmarkConvolution()
{
    QImage referenceImage(QString(FILES_DATA_DIR) + QDir::separator() + "hakonepa.png");
    QRect imageRect(QPoint(), referenceImage.size());

    KisPaintDeviceSP dev = new KisPaintDevice(KoColorSpaceRegistry::instance()->rgb8());
    dev->convertFromQImage(referenceImage, 0, 0, 0);

    qreal offset = 0.0;
    qreal factor = 1.0;
    Matrix<qreal, 3, 3> filter = initAsymmFilter(offset, factor);

    int diameter = 1;

    for (int i = 0; i < 10; i++) {

        KisCircleMaskGenerator* kas = new KisCircleMaskGenerator(diameter, 1.0, 5, 5, 2);
        KisConvolutionKernelSP kernel = KisConvolutionKernel::fromMaskGenerator(kas);

        KisConvolutionPainter gc(dev);

        QTime timer; timer.start();

        // CALLGRIND_START_INSTRUMENTATION;

        gc.beginTransaction(0);
        gc.applyMatrix(kernel, dev, imageRect.topLeft(), imageRect.topLeft(),
                       imageRect.size());
        gc.deleteTransaction();

        // CALLGRIND_STOP_INSTRUMENTATION;

        qDebug() << "Diameter:" << diameter << "time:" << timer.elapsed();

        if(diameter < 10) {
            diameter += 2;
        } else {
            diameter += 8;
        }
    }
}



void KisConvolutionPainterTest::testGaussian()
{

   QImage referenceImage(QString(FILES_DATA_DIR) + QDir::separator() + "lena.png");

   KisPaintDeviceSP dev = new KisPaintDevice(KoColorSpaceRegistry::instance()->rgb8());
   dev->convertFromQImage(referenceImage, 0, 0, 0);

   QBitArray channelFlags =
       KoColorSpaceRegistry::instance()->rgb8()->channelFlags(true, true);

   KisConvolutionPainter gc(dev);


   uint horizontalRadius = 1, verticalRadius = 1;
   
   for(int i = 0; i < 10 ; i++, horizontalRadius++, verticalRadius++)
   {
       gc.beginTransaction(0);
       uint horizKernelSize = horizontalRadius * 2 + 1;
       Matrix<qreal, Dynamic, Dynamic> horizGaussian(1, horizKernelSize);
    
       qreal horizSigma = horizontalRadius;
       const qreal horizMultiplicand = 1 / (2 * M_PI * horizSigma * horizSigma);
       const qreal horizExponentMultiplicand = 1 / (2 * horizSigma * horizSigma);
    
       for (uint x = 0; x < horizKernelSize; x++)
       {
           uint xDistance = qAbs((int)horizontalRadius - (int)x);
           horizGaussian(0, x) = horizMultiplicand * exp( -(qreal)((xDistance * xDistance) + (horizontalRadius * horizontalRadius)) * horizExponentMultiplicand );
       }
       uint verticalKernelSize = verticalRadius * 2 + 1;
       Matrix<qreal, Dynamic, Dynamic> verticalGaussian(verticalKernelSize, 1);
    
       qreal verticalSigma = verticalRadius;
       const qreal verticalMultiplicand = 1 / (2 * M_PI * verticalSigma * verticalSigma);
       const qreal verticalExponentMultiplicand = 1 / (2 * verticalSigma * verticalSigma);
    
       for (uint y = 0; y < verticalKernelSize; y++)
       {
           uint yDistance = ((int)verticalRadius - (int)y);
           verticalGaussian(y, 0) = verticalMultiplicand * exp( -(qreal)((yDistance * yDistance) + (verticalRadius * verticalRadius)) * verticalExponentMultiplicand );
       }
    
       if (( horizontalRadius > 0 ) && ( verticalRadius > 0 )) {
           KisPaintDeviceSP interm = new KisPaintDevice(dev->colorSpace());
    
           KisConvolutionKernelSP kernelHoriz = KisConvolutionKernel::fromMatrix(horizGaussian, 0, horizGaussian.sum());
           KisConvolutionKernelSP kernelVertical = KisConvolutionKernel::fromMatrix(verticalGaussian, 0, verticalGaussian.sum());
    
           KisConvolutionPainter horizPainter(interm);
           horizPainter.setChannelFlags(channelFlags);
           horizPainter.applyMatrix(kernelHoriz, dev,
                                    -QPoint(0, verticalRadius),
                                    -QPoint(0, verticalRadius),
                                    QSize(0, 2 * verticalRadius), BORDER_REPEAT);
    
    

           gc.setChannelFlags(channelFlags);
           gc.applyMatrix(kernelVertical, interm, QPoint(0,0), QPoint(0,0), QSize(0,0), BORDER_REPEAT);

           QImage result = dev->convertToQImage(0, 0, 0, referenceImage.width(), referenceImage.height());
           QPoint errpoint;

           if (!TestUtil::compareQImages(errpoint, referenceImage, result)) {
               referenceImage.save("lena.png");
               result.save("lena_gaussian_blur_filter.png" + QString::number(i) );
               QFAIL(QString("Failed, first different pixel: %1,%2 \n").arg(errpoint.x()).arg(errpoint.y()).toLatin1());
              }

           gc.deleteTransaction();
       }
    }
}


QTEST_KDEMAIN(KisConvolutionPainterTest, GUI)
#include "kis_convolution_painter_test.moc"
