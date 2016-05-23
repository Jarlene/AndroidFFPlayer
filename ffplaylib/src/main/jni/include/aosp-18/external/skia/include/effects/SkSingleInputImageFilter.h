/*
 * Copyright 2012 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#ifndef SkSingleInputImageFilter_DEFINED
#define SkSingleInputImageFilter_DEFINED

#include "SkImageFilter.h"

class SkMatrix;
struct SkIPoint;
class GrTexture;

class SK_API SkSingleInputImageFilter : public SkImageFilter {
public:
    SK_DECLARE_PUBLIC_FLATTENABLE_DESERIALIZATION_PROCS(SkSingleInputImageFilter)

protected:
    explicit SkSingleInputImageFilter(SkImageFilter* input);
    ~SkSingleInputImageFilter();
    explicit SkSingleInputImageFilter(SkFlattenableReadBuffer& rb);
    virtual void flatten(SkFlattenableWriteBuffer& wb) const SK_OVERRIDE;

    // Recurses on input (if non-NULL), and returns the processed result,
    // otherwise returns src.
    SkBitmap getInputResult(Proxy*, const SkBitmap& src, const SkMatrix&,
                            SkIPoint* offset);

    SkImageFilter* input() const { return getInput(0); }
private:
    typedef SkImageFilter INHERITED;
};

#endif
