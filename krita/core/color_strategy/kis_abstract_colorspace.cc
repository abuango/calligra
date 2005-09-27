/*
 *  Copyright (c) 2002 Patrick Julien  <freak@codepimps.org>
 *  Copyright (c) 2004 Cyrille Berger <cberger@cberger.net>
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
#include <qdir.h>

#include <kdebug.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <kinstance.h>

#include "kis_abstract_colorspace.h"
#include "kis_pixel.h"
#include "kis_global.h"
#include "kis_profile.h"
#include "kis_config.h"
#include "kis_id.h"
#include "kis_integer_maths.h"
#include "kis_color_conversions.h"
#include "kis_colorspace_factory_registry.h"
#include "kis_channelinfo.h"
#include "kis_xyz_colorspace.h"

struct KisColorAdjustment
{
    ~KisColorAdjustment() { cmsDeleteTransform(transform);
        cmsCloseProfile(profiles[0]);
        cmsCloseProfile(profiles[1]);
        cmsCloseProfile(profiles[2]);
    }

    cmsHPROFILE profiles[3];
    cmsHTRANSFORM transform;
};

KisAbstractColorSpace::KisAbstractColorSpace(const KisID& id, DWORD cmType, icColorSpaceSignature colorSpaceSignature, KisProfile *p)
    : m_id(id),
      m_cmType(cmType),
      m_colorSpaceSignature(colorSpaceSignature)
{
    m_alphaPos = -1;
    m_alphaSize = -1;
    m_qcolordata = 0;
    m_lastUsedDstProfile = 0;
    m_lastUsedTransform = 0;
    m_profile = p;
}

void KisAbstractColorSpace::init()
{
    // Default pixel buffer for QColor conversion
    m_qcolordata = new Q_UINT8[3];
    Q_CHECK_PTR(m_qcolordata);

    if(m_profile==0) return;

    cmsHPROFILE hProfile = m_profile->profile();

    if (m_cmType != TYPE_BGR_8) {
        // For conversions from default rgb
        cmsHPROFILE hsRGB = cmsCreate_sRGBProfile();

        m_defaultFromRGB = cmsCreateTransform(hsRGB, TYPE_BGR_8,
                                              hProfile, m_cmType,
                                              INTENT_PERCEPTUAL, 0);

        m_defaultToRGB =  cmsCreateTransform(hProfile, m_cmType,
                                             hsRGB, TYPE_BGR_8,
                                             INTENT_PERCEPTUAL, 0);
    }

    if (m_cmType != TYPE_XYZ_16) {
    	// For conversion from default 16 bit xyz for default pixel ops
    	cmsHPROFILE hsXYZ = cmsCreateXYZProfile();

	    m_defaultFromXYZ = cmsCreateTransform(hsXYZ, TYPE_XYZ_16,
		    	                		      hProfile, m_cmType,
                			    		      INTENT_PERCEPTUAL, 0);

      	m_defaultToXYZ = cmsCreateTransform(hProfile, m_cmType,
	                    				    hsXYZ, TYPE_XYZ_16,
					                        INTENT_PERCEPTUAL, 0);

    }
}

KisAbstractColorSpace::~KisAbstractColorSpace()
{
}


bool KisAbstractColorSpace::convertTo(KisPixel& src, KisPixel& dst, Q_INT32 renderingIntent)
{
    return convertPixelsTo(src.channels(),
			   dst.channels(), dst.colorSpace(),
			   renderingIntent);
}

bool KisAbstractColorSpace::convertPixelsTo(const Q_UINT8 * src,
					    Q_UINT8 * dst,
					    KisColorSpace * dstColorSpace,
					    Q_UINT32 numPixels,
					    Q_INT32 renderingIntent)
{

    cmsHTRANSFORM tf = 0;

    Q_INT32 srcPixelSize = pixelSize();
    Q_INT32 dstPixelSize = dstColorSpace -> pixelSize();

//    kdDebug() << "src space: " << id().name() << ", src profile " << srcProfile->productName()
//              << ", dst space: " << dstColorSpace->id().name() << ", dst profile " << dstProfile->productName()
//              << ", number of pixels: " << numPixels << "\n";


    if (m_lastUsedTransform != 0) {
        if (dstColorSpace->getProfile() == m_lastUsedDstProfile)
            tf = m_lastUsedTransform;
    }

    if (!tf && m_profile && dstColorSpace->getProfile()) {

        if (!m_transforms.contains(KisProfilePair(m_profile, dstColorSpace->getProfile()))) {
            tf = createTransform(dstColorSpace,
                         m_profile,
                         dstColorSpace->getProfile(),
                         renderingIntent);
            if (tf) {
//                  kdDebug() << "Going to add transform to cache "
//                            << " m_profile: " << m_profile->productName()
//                            << " dstProfile " << dstColorSpace->getProfile()->productName() << "\n";

                 m_transforms[KisProfilePair(m_profile, dstColorSpace->getProfile())] = tf;
            }
        }
        else {
            tf = m_transforms[KisProfilePair(m_profile, dstColorSpace->getProfile())];
        }

        if ( tf ) {
            m_lastUsedTransform = tf;
            m_lastUsedDstProfile = dstColorSpace->getProfile();
        }
    }

    if (tf) {

        cmsDoTransform(tf, const_cast<Q_UINT8 *>(src), dst, numPixels);

        if (dstColorSpace -> hasAlpha())
        {
            // Lcms does nothing to the destination alpha channel so we must convert that manually.
            while (numPixels > 0) {
                Q_UINT8 alpha = getAlpha(src);
                dstColorSpace -> setAlpha(dst, alpha, 1);

                src += srcPixelSize;
                dst += dstPixelSize;
                numPixels--;
            }
        }

        return true;
    }

    // Couldn't get a profile. Use QColor -- this is okay here, because even if we were to use KisColor,
    // we still wouldn't be able to get a transform. That's why we're here...
    while (numPixels > 0) {
        QColor color;
        Q_UINT8 opacity;

        toQColor(src, &color, &opacity);
        dstColorSpace -> fromQColor(color, opacity, dst);

        src += srcPixelSize;
        dst += dstPixelSize;
        numPixels--;
    }

    return true;
}


KisColorAdjustment *KisAbstractColorSpace::createBrightnessContrastAdjustment(Q_UINT16 *transferValues)
{
    LPGAMMATABLE transferFunctions[3];
    transferFunctions[0] = cmsBuildGamma(256, 1.0);
    transferFunctions[1] = cmsBuildGamma(256, 1.0);
    transferFunctions[2] = cmsBuildGamma(256, 1.0);

    for(int i =0; i < 256; i++)
        transferFunctions[0]->GammaTable[i] = transferValues[i];

    KisColorAdjustment *adj = new KisColorAdjustment;
    adj->profiles[1] = cmsCreateLinearizationDeviceLink(icSigLabData, transferFunctions);
    cmsSetDeviceClass(adj->profiles[1], icSigAbstractClass);

    adj->profiles[0] = m_profile->profile();
    adj->profiles[2] = m_profile->profile();
    adj->transform  = cmsCreateMultiprofileTransform(adj->profiles, 3, m_cmType, m_cmType, INTENT_PERCEPTUAL, 0);

    return adj;
}

void KisAbstractColorSpace::applyAdjustment(const Q_UINT8 *src, Q_UINT8 *dst, KisColorAdjustment *adj, Q_INT32 nPixels)
{
    cmsDoTransform(adj->transform, const_cast<Q_UINT8 *>(src), dst, nPixels);
}


// BC: should this default be HSV-based?
Q_INT8 KisAbstractColorSpace::difference(const Q_UINT8* src1, const Q_UINT8* src2)
{
    if ( m_defaultToXYZ != 0 && m_defaultFromXYZ != 0 ) {
        Q_UINT32 psize = xyz::MAX_CHANNEL_XYZA * sizeof(Q_UINT16);


        if ( m_conversionCache.size() < 2 * psize ) {
            m_conversionCache.resize( 2 * psize, QGArray::SpeedOptim );
        }

        cmsDoTransform( m_defaultToXYZ, const_cast<Q_UINT8*>( src1 ), m_conversionCache.data(), 1);
        cmsDoTransform( m_defaultToXYZ, const_cast<Q_UINT8*>( src2 ), m_conversionCache.data() + psize, 1);

        return KisColorSpaceFactoryRegistry::getXYZ16()->difference( m_conversionCache.data(), m_conversionCache.data() + psize );

    }
    else {
        QColor color1, color2;
        toQColor(src1, &color1);
        toQColor(src2, &color2);

        int h1, s1, v1, h2, s2, v2;
        rgb_to_hsv(color1.red(), color1.green(), color1.blue(), &h1, &s1, &v1);
        rgb_to_hsv(color2.red(), color2.green(), color2.blue(), &h2, &s2, &v2);

        return QMAX(QABS(v1 - v2), QMAX(QABS(s1 - s2), QABS(h1 - h2)));
    }
}

void KisAbstractColorSpace::mixColors(const Q_UINT8 **colors, const Q_UINT8 *weights, Q_UINT32 nColors, Q_UINT8 *dst) const
{
    Q_UINT32 totalRed = 0, totalGreen = 0, totalBlue = 0, newAlpha = 0;

    QColor c;
    Q_UINT8 opacity;

    while (nColors--)
    {
        // Ugly hack to get around the current constness mess of the colour strategy...
        const_cast<KisAbstractColorSpace *>(this) -> toQColor(*colors, &c, &opacity);

        Q_UINT32 alphaTimesWeight = UINT8_MULT(opacity, *weights);

        totalRed += c.red() * alphaTimesWeight;
        totalGreen += c.green() * alphaTimesWeight;
        totalBlue += c.blue() * alphaTimesWeight;
        newAlpha += alphaTimesWeight;

        weights++;
        colors++;
    }

    Q_ASSERT(newAlpha <= 255);

    if (newAlpha > 0) {
        totalRed = UINT8_DIVIDE(totalRed, newAlpha);
        totalGreen = UINT8_DIVIDE(totalGreen, newAlpha);
        totalBlue = UINT8_DIVIDE(totalBlue, newAlpha);
    }

    // Divide by 255.
    totalRed += 0x80;

    Q_UINT32 dstRed = ((totalRed >> 8) + totalRed) >> 8;
    Q_ASSERT(dstRed <= 255);

    totalGreen += 0x80;
    Q_UINT32 dstGreen = ((totalGreen >> 8) + totalGreen) >> 8;
    Q_ASSERT(dstGreen <= 255);

    totalBlue += 0x80;
    Q_UINT32 dstBlue = ((totalBlue >> 8) + totalBlue) >> 8;
    Q_ASSERT(dstBlue <= 255);

    const_cast<KisAbstractColorSpace *>(this) -> fromQColor(QColor(dstRed, dstGreen, dstBlue), newAlpha, dst);
}

void KisAbstractColorSpace::convolveColors(Q_UINT8** colors, Q_INT32 * kernelValues, enumChannelFlags channelFlags, Q_UINT8 *dst, Q_INT32 factor, Q_INT32 offset, Q_INT32 nColors) const
{
    Q_INT32 totalRed = 0, totalGreen = 0, totalBlue = 0, totalAlpha = 0;

    QColor dstColor;
    Q_UINT8 dstOpacity;

    const_cast<KisAbstractColorSpace *>(this)->toQColor(dst, &dstColor, &dstOpacity);

    while (nColors--)
    {
        Q_INT32 weight = *kernelValues;

        if (weight != 0) {
            QColor c;
            Q_UINT8 opacity;
            const_cast<KisAbstractColorSpace *>(this)->toQColor( *colors, &c, &opacity );
            totalRed += c.red() * weight;
            totalGreen += c.green() * weight;
            totalBlue += c.blue() * weight;
            totalAlpha += opacity * weight;
        }
        colors++;
        kernelValues++;
    }


    if (channelFlags & FLAG_COLOR) {
        const_cast<KisAbstractColorSpace *>(this)->fromQColor(QColor(CLAMP((totalRed / factor) + offset, 0, Q_UINT8_MAX),
                                        CLAMP((totalGreen / factor) + offset, 0, Q_UINT8_MAX),
                                        CLAMP((totalBlue / factor) + offset, 0, Q_UINT8_MAX)),
            dstOpacity,
            dst);
    }
    if (channelFlags & FLAG_ALPHA) {
        const_cast<KisAbstractColorSpace *>(this)->fromQColor(dstColor, CLAMP((totalAlpha/ factor) + offset, 0, Q_UINT8_MAX), dst);
    }

}

void KisAbstractColorSpace::darken(const Q_UINT8 * src, Q_UINT8 * dst, Q_INT32 shade, bool compensate, double compensation, Q_INT32 nPixels) const
{
    QColor c;
    Q_INT32 psize = pixelSize();

    for (int i = 0; i < nPixels; ++i) {

        const_cast<KisAbstractColorSpace *>(this) -> toQColor(src + (i * psize), &c);
        Q_INT32 r, g, b;

        if (compensate) {
            r = (Q_INT32) QMIN(255, ((c.red() * shade) / (compensation * 255)));
            g = (Q_INT32) QMIN(255, ((c.green() * shade) / (compensation * 255)));
            b = (Q_INT32) QMIN(255, ((c.blue() * shade) / (compensation * 255)));
        }
        else {
            r = (Q_INT32) QMIN(255, (c.red() * shade / 255));
            g = (Q_INT32) QMIN(255, (c.green() * shade / 255));
            b = (Q_INT32) QMIN(255, (c.blue() * shade / 255));
        }
        c.setRgb(r, g, b);

        const_cast<KisAbstractColorSpace *>(this)->fromQColor( c, dst  + (i * psize));
    }
}

Q_UINT8 KisAbstractColorSpace::intensity8(const Q_UINT8 * src) const
{
    QColor c;
        Q_UINT8 opacity;
        const_cast<KisAbstractColorSpace *>(this)->toQColor(src, &c, &opacity);
        return (Q_UINT8)((c.red() * 0.30 + c.green() * 0.59 + c.blue() * 0.11) + 0.5);

}


void KisAbstractColorSpace::bitBlt(Q_UINT8 *dst,
                   Q_INT32 dststride,
                   KisColorSpace * srcSpace,
                   const Q_UINT8 *src,
                   Q_INT32 srcRowStride,
                   const Q_UINT8 *srcAlphaMask,
                   Q_INT32 maskRowStride,
                   Q_UINT8 opacity,
                   Q_INT32 rows,
                   Q_INT32 cols,
                   const KisCompositeOp& op)
{
    if (rows <= 0 || cols <= 0)
        return;

    if (m_id!= srcSpace -> id()) {
        Q_UINT32 len = pixelSize() * rows * cols;

        KisProfile * srcProfile = srcSpace -> getProfile();
        KisProfile * dstProfile = getProfile();

        // If our conversion cache is too small, extend it.
        if (!m_conversionCache.resize( len, QGArray::SpeedOptim )) {
            kdDebug() << "Could not allocate enough memory for the conversion!\n";
            // XXX: We should do a slow, pixel by pixel bitblt here...
            return;
        }

        if (srcProfile && dstProfile) {
            for (Q_INT32 row = 0; row < rows; row++) {
                srcSpace -> convertPixelsTo(src + row * srcRowStride,
                                            m_conversionCache.data() + row * cols * pixelSize(), this,
                                            cols);
            }
        }
        else {
            for (Q_INT32 row = 0; row < rows; row++) {
                srcSpace -> convertPixelsTo(src + row * srcRowStride,
                                            m_conversionCache.data() + row * cols * pixelSize(), this,
                                            cols);
            }
        }


        // The old srcRowStride is no longer valid because we converted to the current cs
        srcRowStride = cols * pixelSize();

        bitBlt(dst,
               dststride,
               m_conversionCache.data(),
               srcRowStride,
               srcAlphaMask,
               maskRowStride,
               opacity,
               rows,
               cols,
               op);

    }
    else {
        bitBlt(dst,
               dststride,
               src,
               srcRowStride,
               srcAlphaMask,
               maskRowStride,
               opacity,
               rows,
               cols,
               op);
    }
}

QImage KisAbstractColorSpace::convertToQImage(const Q_UINT8 *data, Q_INT32 width, Q_INT32 height,
                                              KisProfile *  dstProfile,
                                              Q_INT32 renderingIntent, float /*exposure*/)

{
    QImage img = QImage(width, height, 32, 0, QImage::LittleEndian);
    img.setAlphaBuffer( true );

    KisColorSpace * dstCS;

    if (dstProfile)
        dstCS = KisColorSpaceFactoryRegistry::instance() -> getColorSpace(KisID("RGBA",""),dstProfile->productName());
    else
        dstCS = KisColorSpaceFactoryRegistry::instance() -> getColorSpace(KisID("RGBA",""),"");

printf("profile = %d dstProfile = %d\n",getProfile(),dstProfile);
if(getProfile())
    printf("profile = %s\n",getProfile()->productName().ascii());

if(dstProfile)
    printf("dstProfile = %s\n",dstProfile->productName().ascii());

if(dstCS->getProfile())
    printf("dstCS->Profile = %s\n",dstCS->getProfile()->productName().ascii());

    convertPixelsTo(const_cast<Q_UINT8 *>(data), img.bits(), dstCS, width * height, renderingIntent);

    return img;
}


cmsHTRANSFORM KisAbstractColorSpace::createTransform(KisColorSpace * dstColorSpace,
                             KisProfile *  srcProfile,
                             KisProfile *  dstProfile,
                             Q_INT32 renderingIntent)
{
    KisConfig cfg;
    int flags = 0;

    if (cfg.useBlackPointCompensation()) {
        flags = cmsFLAGS_BLACKPOINTCOMPENSATION;
    }

    if (dstProfile && srcProfile ) {
        cmsHTRANSFORM tf = cmsCreateTransform(srcProfile -> profile(),
                              colorSpaceType(),
                              dstProfile -> profile(),
                              dstColorSpace -> colorSpaceType(),
                              renderingIntent,
                              flags);

        return tf;
    }
    return 0;
}

