/*
 * Copyright 2012 The Android Open Source Project
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */


#ifndef SkMorphologyImageFilter_DEFINED
#define SkMorphologyImageFilter_DEFINED

#include "SkImageFilter.h"

class SK_API SkMorphologyImageFilter : public SkImageFilter {
public:
    explicit SkMorphologyImageFilter(SkFlattenableReadBuffer& buffer);
    SkMorphologyImageFilter(int radiusX, int radiusY);

protected:
    virtual void flatten(SkFlattenableWriteBuffer& buffer) SK_OVERRIDE;
    SkISize    radius() const { return fRadius; }

private:
    SkISize    fRadius;
    typedef SkImageFilter INHERITED;
};

class SK_API SkDilateImageFilter : public SkMorphologyImageFilter {
public:
    SkDilateImageFilter(int radiusX, int radiusY) : INHERITED(radiusX, radiusY) {}
    explicit SkDilateImageFilter(SkFlattenableReadBuffer& buffer) : INHERITED(buffer) {}

    virtual bool asADilate(SkISize* radius) const SK_OVERRIDE;
    virtual bool onFilterImage(Proxy*, const SkBitmap& src, const SkMatrix&,
                               SkBitmap* result, SkIPoint* offset) SK_OVERRIDE;
    static SkFlattenable* CreateProc(SkFlattenableReadBuffer& buffer) {
        return SkNEW_ARGS(SkDilateImageFilter, (buffer));
    }
    virtual Factory getFactory() SK_OVERRIDE { return CreateProc; }
    SK_DECLARE_FLATTENABLE_REGISTRAR()

    typedef SkMorphologyImageFilter INHERITED;
};

class SK_API SkErodeImageFilter : public SkMorphologyImageFilter {
public:
    SkErodeImageFilter(int radiusX, int radiusY) : INHERITED(radiusX, radiusY) {}
    explicit SkErodeImageFilter(SkFlattenableReadBuffer& buffer) : INHERITED(buffer) {}

    virtual bool asAnErode(SkISize* radius) const SK_OVERRIDE;
    virtual bool onFilterImage(Proxy*, const SkBitmap& src, const SkMatrix&,
                               SkBitmap* result, SkIPoint* offset) SK_OVERRIDE;

    static SkFlattenable* CreateProc(SkFlattenableReadBuffer& buffer) {
        return SkNEW_ARGS(SkErodeImageFilter, (buffer));
    }
    virtual Factory getFactory() SK_OVERRIDE { return CreateProc; }
    SK_DECLARE_FLATTENABLE_REGISTRAR()

private:
    typedef SkMorphologyImageFilter INHERITED;
};

#endif

