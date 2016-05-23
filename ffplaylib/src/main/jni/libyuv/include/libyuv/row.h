/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef INCLUDE_LIBYUV_ROW_H_  // NOLINT
#define INCLUDE_LIBYUV_ROW_H_

#include "libyuv/basic_types.h"

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// TODO(fbarchard): Remove kMaxStride
#define kMaxStride (2880 * 4)
#define IS_ALIGNED(p, a) (!((uintptr_t)(p) & ((a) - 1)))

#if defined(__CLR_VER) || defined(COVERAGE_ENABLED) || \
    defined(TARGET_IPHONE_SIMULATOR)
#define YUV_DISABLE_ASM
#endif
// True if compiling for SSSE3 as a requirement.
#if defined(__SSSE3__) || (defined(_M_IX86_FP) && (_M_IX86_FP >= 3))
#define LIBYUV_SSSE3_ONLY
#endif

// The following are available on all x86 platforms:
#if !defined(YUV_DISABLE_ASM) && \
    (defined(_M_IX86) || defined(__x86_64__) || defined(__i386__))
// Conversions.
#define HAS_ABGRTOARGBROW_SSSE3
#define HAS_ABGRTOARGBROW_SSSE3
#define HAS_ABGRTOUVROW_SSSE3
#define HAS_ABGRTOYROW_SSSE3
#define HAS_ARGB1555TOARGBROW_SSE2
#define HAS_ARGB4444TOARGBROW_SSE2
#define HAS_ARGBTOARGB1555ROW_SSE2
#define HAS_ARGBTOARGB4444ROW_SSE2
#define HAS_ARGBTOBAYERROW_SSSE3
#define HAS_ARGBTORAWROW_SSSE3
#define HAS_ARGBTORGB24ROW_SSSE3
#define HAS_ARGBTORGB565ROW_SSE2
#define HAS_ARGBTORGBAROW_SSSE3
#define HAS_ARGBTOUVROW_SSSE3
#define HAS_ARGBTOYROW_SSSE3
#define HAS_BGRATOARGBROW_SSSE3
#define HAS_BGRATOUVROW_SSSE3
#define HAS_BGRATOYROW_SSSE3
#define HAS_COPYROW_SSE2
#define HAS_COPYROW_X86
#define HAS_HALFROW_SSE2
#define HAS_I400TOARGBROW_SSE2
#define HAS_I411TOARGBROW_SSSE3
#define HAS_I422TOABGRROW_SSSE3
#define HAS_I422TOARGBROW_SSSE3
#define HAS_I422TOBGRAROW_SSSE3
#define HAS_I422TORAWROW_SSSE3
#define HAS_I422TORGB24ROW_SSSE3
#define HAS_I422TORGBAROW_SSSE3
#define HAS_I444TOARGBROW_SSSE3
#define HAS_MIRRORROW_SSSE3
#define HAS_MIRRORROWUV_SSSE3
#define HAS_NV12TOARGBROW_SSSE3
#define HAS_NV21TOARGBROW_SSSE3
#define HAS_RAWTOARGBROW_SSSE3
#define HAS_RGB24TOARGBROW_SSSE3
#define HAS_RGB565TOARGBROW_SSE2
#define HAS_RGBATOARGBROW_SSSE3
#define HAS_RGBATOUVROW_SSSE3
#define HAS_RGBATOYROW_SSSE3
#define HAS_SETROW_X86
#define HAS_SPLITUV_SSE2
#define HAS_UYVYTOUV422ROW_SSE2
#define HAS_UYVYTOUVROW_SSE2
#define HAS_UYVYTOYROW_SSE2
#define HAS_YTOARGBROW_SSE2
#define HAS_YUY2TOUV422ROW_SSE2
#define HAS_YUY2TOUVROW_SSE2
#define HAS_YUY2TOYROW_SSE2
#define HAS_I422TOYUY2ROW_SSE2
#define HAS_I422TOUYVYROW_SSE2

// Effects
#define HAS_ARGBAFFINEROW_SSE2
#define HAS_ARGBATTENUATEROW_SSSE3
#define HAS_ARGBBLENDROW_SSSE3
#define HAS_ARGBCOLORMATRIXROW_SSSE3
#define HAS_ARGBGRAYROW_SSSE3
#define HAS_ARGBINTERPOLATEROW_SSSE3
#define HAS_ARGBMIRRORROW_SSSE3
#define HAS_ARGBQUANTIZEROW_SSE2
#define HAS_ARGBSEPIAROW_SSSE3
#define HAS_ARGBSHADE_SSE2
#define HAS_ARGBUNATTENUATEROW_SSE2
#define HAS_COMPUTECUMULATIVESUMROW_SSE2
#define HAS_CUMULATIVESUMTOAVERAGE_SSE2
#endif

// The following are Windows only.
// TODO(fbarchard): Port to gcc.
#if !defined(YUV_DISABLE_ASM) && defined(_M_IX86)
#define HAS_ARGBCOLORTABLEROW_X86
#endif

// The following are Yasm x86 only.
// TODO(fbarchard): Port AVX2 to inline.
#if !defined(YUV_DISABLE_ASM) && defined(HAVE_YASM)
    (defined(_M_IX86) || defined(_M_X64) || \
    defined(__x86_64__) || defined(__i386__))
#define HAS_SPLITUV_AVX2
#define HAS_SPLITUV_MMX
#define HAS_YUY2TOYROW_AVX2
#define HAS_UYVYTOYROW_AVX2
#define HAS_YUY2TOYROW_MMX
#define HAS_UYVYTOYROW_MMX
#endif

// The following are disabled when SSSE3 is available:
#if !defined(YUV_DISABLE_ASM) && \
    (defined(_M_IX86) || defined(__x86_64__) || defined(__i386__)) && \
    !defined(LIBYUV_SSSE3_ONLY)
#define HAS_ARGBATTENUATE_SSE2
#define HAS_ARGBBLENDROW_SSE2
#define HAS_MIRRORROW_SSE2
#endif

// The following are available on Neon platforms
#if !defined(YUV_DISABLE_ASM) && (defined(__ARM_NEON__) || defined(LIBYUV_NEON))
#define HAS_ABGRTOARGBROW_NEON
#define HAS_ARGBTOBAYERROW_NEON
#define HAS_ARGBTORAWROW_NEON
#define HAS_ARGBTORGB24ROW_NEON
#define HAS_ARGBTORGBAROW_NEON
#define HAS_BGRATOARGBROW_NEON
#define HAS_COPYROW_NEON
#define HAS_HALFROW_NEON
#define HAS_I422TOABGRROW_NEON
#define HAS_I422TOARGBROW_NEON
#define HAS_I422TOBGRAROW_NEON
#define HAS_I422TORAWROW_NEON
#define HAS_I422TORGB24ROW_NEON
#define HAS_I422TORGBAROW_NEON
#define HAS_MIRRORROW_NEON
#define HAS_MIRRORROWUV_NEON
#define HAS_NV12TOARGBROW_NEON
#define HAS_NV21TOARGBROW_NEON
#define HAS_RAWTOARGBROW_NEON
#define HAS_RGB24TOARGBROW_NEON
#define HAS_RGBATOARGBROW_NEON
#define HAS_SETROW_NEON
#define HAS_SPLITUV_NEON
#define HAS_UYVYTOUV422ROW_NEON
#define HAS_UYVYTOUVROW_NEON
#define HAS_UYVYTOYROW_NEON
#define HAS_YUY2TOUV422ROW_NEON
#define HAS_YUY2TOUVROW_NEON
#define HAS_YUY2TOYROW_NEON
#define HAS_I422TOYUY2ROW_NEON
#define HAS_I422TOUYVYROW_NEON
#define HAS_ARGBTORGB565ROW_NEON
#define HAS_ARGBTOARGB1555ROW_NEON
#define HAS_ARGBTOARGB4444ROW_NEON
#define HAS_ARGBTOYROW_NEON
#define HAS_MERGEUV_NEON
#endif

// The following are available on Mips platforms
#if !defined(YUV_DISABLE_ASM) && defined(__mips__)
#if defined(__mips_dsp) && (__mips_dsp_rev >= 2)
#define HAS_SPLITUV_MIPS_DSPR2
#endif
#endif

#if defined(_MSC_VER) && !defined(__CLR_VER)
#define SIMD_ALIGNED(var) __declspec(align(16)) var
typedef __declspec(align(16)) int8 vec8[16];
typedef __declspec(align(16)) uint8 uvec8[16];
typedef __declspec(align(16)) int16 vec16[8];
typedef __declspec(align(16)) uint16 uvec16[8];
typedef __declspec(align(16)) int32 vec32[4];
typedef __declspec(align(16)) uint32 uvec32[4];
#elif defined(__GNUC__)
#define SIMD_ALIGNED(var) var __attribute__((aligned(16)))
typedef int8 __attribute__((vector_size(16))) vec8;
typedef uint8 __attribute__((vector_size(16))) uvec8;
typedef int16 __attribute__((vector_size(16))) vec16;
typedef uint16 __attribute__((vector_size(16))) uvec16;
typedef int32 __attribute__((vector_size(16))) vec32;
typedef uint32 __attribute__((vector_size(16))) uvec32;
#else
#define SIMD_ALIGNED(var) var
typedef int8 vec8[16];
typedef uint8 uvec8[16];
typedef int16 vec16[8];
typedef uint16 uvec16[8];
typedef int32 vec32[4];
typedef uint32 uvec32[4];
#endif

#if defined(__APPLE__) || defined(__x86_64__) || defined(__llvm__)
#define OMITFP
#else
#define OMITFP __attribute__((optimize("omit-frame-pointer")))
#endif

void I422ToARGBRow_NEON(const uint8* y_buf,
                        const uint8* u_buf,
                        const uint8* v_buf,
                        uint8* rgb_buf,
                        int width);
void I422ToBGRARow_NEON(const uint8* y_buf,
                        const uint8* u_buf,
                        const uint8* v_buf,
                        uint8* rgb_buf,
                        int width);
void I422ToABGRRow_NEON(const uint8* y_buf,
                        const uint8* u_buf,
                        const uint8* v_buf,
                        uint8* rgb_buf,
                        int width);
void I422ToRGBARow_NEON(const uint8* y_buf,
                        const uint8* u_buf,
                        const uint8* v_buf,
                        uint8* rgb_buf,
                        int width);
void I422ToRGB24Row_NEON(const uint8* y_buf,
                         const uint8* u_buf,
                         const uint8* v_buf,
                         uint8* rgb_buf,
                         int width);
void I422ToRAWRow_NEON(const uint8* y_buf,
                       const uint8* u_buf,
                       const uint8* v_buf,
                       uint8* rgb_buf,
                       int width);
void NV12ToARGBRow_NEON(const uint8* y_buf,
                        const uint8* uv_buf,
                        uint8* rgb_buf,
                        int width);
void NV21ToARGBRow_NEON(const uint8* y_buf,
                        const uint8* uv_buf,
                        uint8* rgb_buf,
                        int width);

void ARGBToYRow_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void BGRAToYRow_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void ABGRToYRow_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void RGBAToYRow_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void ARGBToYRow_Unaligned_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void BGRAToYRow_Unaligned_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void ABGRToYRow_Unaligned_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void RGBAToYRow_Unaligned_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);

void ARGBToUVRow_SSSE3(const uint8* src_argb0, int src_stride_argb,
                       uint8* dst_u, uint8* dst_v, int width);
void BGRAToUVRow_SSSE3(const uint8* src_argb0, int src_stride_argb,
                       uint8* dst_u, uint8* dst_v, int width);
void ABGRToUVRow_SSSE3(const uint8* src_argb0, int src_stride_argb,
                       uint8* dst_u, uint8* dst_v, int width);
void RGBAToUVRow_SSSE3(const uint8* src_argb0, int src_stride_argb,
                       uint8* dst_u, uint8* dst_v, int width);
void ARGBToUVRow_Unaligned_SSSE3(const uint8* src_argb0, int src_stride_argb,
                                 uint8* dst_u, uint8* dst_v, int width);
void BGRAToUVRow_Unaligned_SSSE3(const uint8* src_argb0, int src_stride_argb,
                                 uint8* dst_u, uint8* dst_v, int width);
void ABGRToUVRow_Unaligned_SSSE3(const uint8* src_argb0, int src_stride_argb,
                                 uint8* dst_u, uint8* dst_v, int width);
void RGBAToUVRow_Unaligned_SSSE3(const uint8* src_argb0, int src_stride_argb,
                                 uint8* dst_u, uint8* dst_v, int width);

void ARGBToYRow_NEON(const uint8* src_argb, uint8* dst_y, int pix);

void MirrorRow_SSSE3(const uint8* src, uint8* dst, int width);
void MirrorRow_SSE2(const uint8* src, uint8* dst, int width);
void MirrorRow_NEON(const uint8* src, uint8* dst, int width);
void MirrorRow_C(const uint8* src, uint8* dst, int width);

void MirrorRowUV_SSSE3(const uint8* src, uint8* dst_u, uint8* dst_v, int width);
void MirrorRowUV_NEON(const uint8* src, uint8* dst_u, uint8* dst_v, int width);
void MirrorRowUV_C(const uint8* src, uint8* dst_u, uint8* dst_v, int width);

void ARGBMirrorRow_SSSE3(const uint8* src, uint8* dst, int width);
void ARGBMirrorRow_C(const uint8* src, uint8* dst, int width);

void SplitUV_C(const uint8* src_uv, uint8* dst_u, uint8* dst_v, int pix);
void SplitUV_SSE2(const uint8* src_uv, uint8* dst_u, uint8* dst_v, int pix);
void SplitUV_AVX2(const uint8* src_uv, uint8* dst_u, uint8* dst_v, int pix);
void SplitUV_NEON(const uint8* src_uv, uint8* dst_u, uint8* dst_v, int pix);
void SplitUV_MIPS_DSPR2(const uint8* src_uv, uint8* dst_u, uint8* dst_v,
                        int pix);
void SplitUV_Unaligned_SSE2(const uint8* src_uv, uint8* dst_u, uint8* dst_v,
                            int pix);
void SplitUV_Unaligned_AVX2(const uint8* src_uv, uint8* dst_u, uint8* dst_v,
                            int pix);
void SplitUV_Unaligned_NEON(const uint8* src_uv, uint8* dst_u, uint8* dst_v,
                            int pix);
void SplitUV_Unaligned_MIPS_DSPR2(const uint8* src_uv, uint8* dst_u,
                                  uint8* dst_v, int pix);
void SplitUV_Any_SSE2(const uint8* src_uv, uint8* dst_u, uint8* dst_v, int pix);
void SplitUV_Any_AVX2(const uint8* src_uv, uint8* dst_u, uint8* dst_v, int pix);
void SplitUV_Any_NEON(const uint8* src_uv, uint8* dst_u, uint8* dst_v, int pix);
void SplitUV_Any_MIPS_DSPR2(const uint8* src_uv, uint8* dst_u, uint8* dst_v,
                            int pix);

void MergeUV_C(const uint8* src_u, const uint8* src_v, uint8* dst_uv,
               int width);
void MergeUV_NEON(const uint8* src_u, const uint8* src_v, uint8* dst_uv,
                  int width);

void CopyRow_SSE2(const uint8* src, uint8* dst, int count);
void CopyRow_X86(const uint8* src, uint8* dst, int count);
void CopyRow_NEON(const uint8* src, uint8* dst, int count);
void CopyRow_C(const uint8* src, uint8* dst, int count);

void SetRow8_X86(uint8* dst, uint32 v32, int count);
void SetRows32_X86(uint8* dst, uint32 v32, int width,
                   int dst_stride, int height);
void SetRow8_NEON(uint8* dst, uint32 v32, int count);
void SetRows32_NEON(uint8* dst, uint32 v32, int width,
                    int dst_stride, int height);
void SetRow8_C(uint8* dst, uint32 v32, int count);
void SetRows32_C(uint8* dst, uint32 v32, int width, int dst_stride, int height);

void ARGBToYRow_C(const uint8* src_argb, uint8* dst_y, int pix);
void BGRAToYRow_C(const uint8* src_argb, uint8* dst_y, int pix);
void ABGRToYRow_C(const uint8* src_argb, uint8* dst_y, int pix);
void RGBAToYRow_C(const uint8* src_argb, uint8* dst_y, int pix);

void ARGBToUVRow_C(const uint8* src_argb0, int src_stride_argb,
                   uint8* dst_u, uint8* dst_v, int width);
void BGRAToUVRow_C(const uint8* src_argb0, int src_stride_argb,
                   uint8* dst_u, uint8* dst_v, int width);
void ABGRToUVRow_C(const uint8* src_argb0, int src_stride_argb,
                   uint8* dst_u, uint8* dst_v, int width);
void RGBAToUVRow_C(const uint8* src_argb0, int src_stride_argb,
                   uint8* dst_u, uint8* dst_v, int width);

void BGRAToARGBRow_SSSE3(const uint8* src_bgra, uint8* dst_argb, int pix);
void ABGRToARGBRow_SSSE3(const uint8* src_abgr, uint8* dst_argb, int pix);
void RGBAToARGBRow_SSSE3(const uint8* src_rgba, uint8* dst_argb, int pix);
void RGB24ToARGBRow_SSSE3(const uint8* src_rgb24, uint8* dst_argb, int pix);
void RAWToARGBRow_SSSE3(const uint8* src_rgb24, uint8* dst_argb, int pix);
void RGB565ToARGBRow_SSE2(const uint8* src_argb, uint8* dst_argb, int pix);
void ARGB1555ToARGBRow_SSE2(const uint8* src_argb, uint8* dst_argb, int pix);
void ARGB4444ToARGBRow_SSE2(const uint8* src_argb, uint8* dst_argb, int pix);

void BGRAToARGBRow_NEON(const uint8* src_bgra, uint8* dst_argb, int pix);
void ABGRToARGBRow_NEON(const uint8* src_abgr, uint8* dst_argb, int pix);
void RGBAToARGBRow_NEON(const uint8* src_rgba, uint8* dst_argb, int pix);
void RGB24ToARGBRow_NEON(const uint8* src_rgb24, uint8* dst_argb, int pix);
void RAWToARGBRow_NEON(const uint8* src_rgb24, uint8* dst_argb, int pix);

void BGRAToARGBRow_C(const uint8* src_bgra, uint8* dst_argb, int pix);
void ABGRToARGBRow_C(const uint8* src_abgr, uint8* dst_argb, int pix);
void RGBAToARGBRow_C(const uint8* src_rgba, uint8* dst_argb, int pix);
void RGB24ToARGBRow_C(const uint8* src_rgb24, uint8* dst_argb, int pix);
void RAWToARGBRow_C(const uint8* src_rgb24, uint8* dst_argb, int pix);
void RGB565ToARGBRow_C(const uint8* src_rgb, uint8* dst_argb, int pix);
void ARGB1555ToARGBRow_C(const uint8* src_argb, uint8* dst_argb, int pix);
void ARGB4444ToARGBRow_C(const uint8* src_argb, uint8* dst_argb, int pix);

void ARGBToRGBARow_SSSE3(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRGB24Row_SSSE3(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRAWRow_SSSE3(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRGB565Row_SSE2(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToARGB1555Row_SSE2(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToARGB4444Row_SSE2(const uint8* src_argb, uint8* dst_rgb, int pix);

void ARGBToRGBARow_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRGB24Row_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRAWRow_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRGB565Row_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToARGB1555Row_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToARGB4444Row_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);

void ARGBToRGBARow_C(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRGB24Row_C(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRAWRow_C(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRGB565Row_C(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToARGB1555Row_C(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToARGB4444Row_C(const uint8* src_argb, uint8* dst_rgb, int pix);

void I400ToARGBRow_SSE2(const uint8* src_y, uint8* dst_argb, int pix);
void I400ToARGBRow_C(const uint8* src_y, uint8* dst_argb, int pix);

void I444ToARGBRow_C(const uint8* y_buf,
                     const uint8* u_buf,
                     const uint8* v_buf,
                     uint8* argb_buf,
                     int width);

void I422ToARGBRow_C(const uint8* y_buf,
                     const uint8* u_buf,
                     const uint8* v_buf,
                     uint8* argb_buf,
                     int width);

void I411ToARGBRow_C(const uint8* y_buf,
                     const uint8* u_buf,
                     const uint8* v_buf,
                     uint8* rgb_buf,
                     int width);

void NV12ToARGBRow_C(const uint8* y_buf,
                     const uint8* uv_buf,
                     uint8* argb_buf,
                     int width);

void NV21ToARGBRow_C(const uint8* y_buf,
                     const uint8* vu_buf,
                     uint8* argb_buf,
                     int width);

void I422ToBGRARow_C(const uint8* y_buf,
                     const uint8* u_buf,
                     const uint8* v_buf,
                     uint8* bgra_buf,
                     int width);

void I422ToABGRRow_C(const uint8* y_buf,
                     const uint8* u_buf,
                     const uint8* v_buf,
                     uint8* abgr_buf,
                     int width);

void I422ToRGBARow_C(const uint8* y_buf,
                     const uint8* u_buf,
                     const uint8* v_buf,
                     uint8* rgba_buf,
                     int width);
void I422ToRGB24Row_C(const uint8* y_buf,
                      const uint8* u_buf,
                      const uint8* v_buf,
                      uint8* rgb24_buf,
                      int width);
void I422ToRAWRow_C(const uint8* y_buf,
                    const uint8* u_buf,
                    const uint8* v_buf,
                    uint8* raw_buf,
                    int width);

void YToARGBRow_C(const uint8* y_buf,
                  uint8* rgb_buf,
                  int width);

void I444ToARGBRow_SSSE3(const uint8* y_buf,
                         const uint8* u_buf,
                         const uint8* v_buf,
                         uint8* argb_buf,
                         int width);

void I422ToARGBRow_SSSE3(const uint8* y_buf,
                         const uint8* u_buf,
                         const uint8* v_buf,
                         uint8* argb_buf,
                         int width);

void I411ToARGBRow_SSSE3(const uint8* y_buf,
                         const uint8* u_buf,
                         const uint8* v_buf,
                         uint8* rgb_buf,
                         int width);

void NV12ToARGBRow_SSSE3(const uint8* y_buf,
                         const uint8* uv_buf,
                         uint8* argb_buf,
                         int width);

void NV21ToARGBRow_SSSE3(const uint8* y_buf,
                         const uint8* vu_buf,
                         uint8* argb_buf,
                         int width);

void I422ToBGRARow_SSSE3(const uint8* y_buf,
                         const uint8* u_buf,
                         const uint8* v_buf,
                         uint8* bgra_buf,
                         int width);

void I422ToABGRRow_SSSE3(const uint8* y_buf,
                         const uint8* u_buf,
                         const uint8* v_buf,
                         uint8* abgr_buf,
                         int width);

void I422ToRGBARow_SSSE3(const uint8* y_buf,
                         const uint8* u_buf,
                         const uint8* v_buf,
                         uint8* rgba_buf,
                         int width);

// RGB24/RAW are unaligned.
void I422ToRGB24Row_SSSE3(const uint8* y_buf,
                          const uint8* u_buf,
                          const uint8* v_buf,
                          uint8* rgb_buf,
                          int width);

void I422ToRAWRow_SSSE3(const uint8* y_buf,
                        const uint8* u_buf,
                        const uint8* v_buf,
                        uint8* rgb_buf,
                        int width);

void I444ToARGBRow_Unaligned_SSSE3(const uint8* y_buf,
                                   const uint8* u_buf,
                                   const uint8* v_buf,
                                   uint8* argb_buf,
                                   int width);

void I422ToARGBRow_Unaligned_SSSE3(const uint8* y_buf,
                                   const uint8* u_buf,
                                   const uint8* v_buf,
                                   uint8* argb_buf,
                                   int width);

void I411ToARGBRow_Unaligned_SSSE3(const uint8* y_buf,
                                   const uint8* u_buf,
                                   const uint8* v_buf,
                                   uint8* rgb_buf,
                                   int width);

void NV12ToARGBRow_Unaligned_SSSE3(const uint8* y_buf,
                                   const uint8* uv_buf,
                                   uint8* argb_buf,
                                   int width);

void NV21ToARGBRow_Unaligned_SSSE3(const uint8* y_buf,
                                   const uint8* vu_buf,
                                   uint8* argb_buf,
                                   int width);

void I422ToBGRARow_Unaligned_SSSE3(const uint8* y_buf,
                                   const uint8* u_buf,
                                   const uint8* v_buf,
                                   uint8* bgra_buf,
                                   int width);

void I422ToABGRRow_Unaligned_SSSE3(const uint8* y_buf,
                                   const uint8* u_buf,
                                   const uint8* v_buf,
                                   uint8* abgr_buf,
                                   int width);

void I422ToRGBARow_Unaligned_SSSE3(const uint8* y_buf,
                                   const uint8* u_buf,
                                   const uint8* v_buf,
                                   uint8* rgba_buf,
                                   int width);

void I444ToARGBRow_Any_SSSE3(const uint8* y_buf,
                             const uint8* u_buf,
                             const uint8* v_buf,
                             uint8* argb_buf,
                             int width);

void I422ToARGBRow_Any_SSSE3(const uint8* y_buf,
                             const uint8* u_buf,
                             const uint8* v_buf,
                             uint8* argb_buf,
                             int width);

void I411ToARGBRow_Any_SSSE3(const uint8* y_buf,
                             const uint8* u_buf,
                             const uint8* v_buf,
                             uint8* rgb_buf,
                             int width);

void NV12ToARGBRow_Any_SSSE3(const uint8* y_buf,
                             const uint8* uv_buf,
                             uint8* argb_buf,
                             int width);

void NV21ToARGBRow_Any_SSSE3(const uint8* y_buf,
                             const uint8* vu_buf,
                             uint8* argb_buf,
                             int width);

void I422ToBGRARow_Any_SSSE3(const uint8* y_buf,
                             const uint8* u_buf,
                             const uint8* v_buf,
                             uint8* bgra_buf,
                             int width);

void I422ToABGRRow_Any_SSSE3(const uint8* y_buf,
                             const uint8* u_buf,
                             const uint8* v_buf,
                             uint8* abgr_buf,
                             int width);

void I422ToRGBARow_Any_SSSE3(const uint8* y_buf,
                             const uint8* u_buf,
                             const uint8* v_buf,
                             uint8* rgba_buf,
                             int width);

// RGB24/RAW are unaligned.
void I422ToRGB24Row_Any_SSSE3(const uint8* y_buf,
                              const uint8* u_buf,
                              const uint8* v_buf,
                              uint8* rgb_buf,
                              int width);

void I422ToRAWRow_Any_SSSE3(const uint8* y_buf,
                            const uint8* u_buf,
                            const uint8* v_buf,
                            uint8* rgb_buf,
                            int width);

void YToARGBRow_SSE2(const uint8* y_buf,
                     uint8* argb_buf,
                     int width);

// ARGB preattenuated alpha blend.
void ARGBBlendRow_SSSE3(const uint8* src_argb0, const uint8* src_argb1,
                        uint8* dst_argb, int width);
void ARGBBlendRow_SSE2(const uint8* src_argb0, const uint8* src_argb1,
                       uint8* dst_argb, int width);
void ARGBBlendRow_C(const uint8* src_argb0, const uint8* src_argb1,
                    uint8* dst_argb, int width);

void ARGBToRGB24Row_Any_SSSE3(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRAWRow_Any_SSSE3(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRGB565Row_Any_SSE2(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToARGB1555Row_Any_SSE2(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToARGB4444Row_Any_SSE2(const uint8* src_argb, uint8* dst_rgb, int pix);

void ARGBToRGB24Row_Any_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRAWRow_Any_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToRGB565Row_Any_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToARGB1555Row_Any_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);
void ARGBToARGB4444Row_Any_NEON(const uint8* src_argb, uint8* dst_rgb, int pix);

void ARGBToYRow_Any_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void BGRAToYRow_Any_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void ABGRToYRow_Any_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void RGBAToYRow_Any_SSSE3(const uint8* src_argb, uint8* dst_y, int pix);
void ARGBToUVRow_Any_SSSE3(const uint8* src_argb0, int src_stride_argb,
                           uint8* dst_u, uint8* dst_v, int width);
void BGRAToUVRow_Any_SSSE3(const uint8* src_argb0, int src_stride_argb,
                           uint8* dst_u, uint8* dst_v, int width);
void ABGRToUVRow_Any_SSSE3(const uint8* src_argb0, int src_stride_argb,
                           uint8* dst_u, uint8* dst_v, int width);
void RGBAToUVRow_Any_SSSE3(const uint8* src_argb0, int src_stride_argb,
                           uint8* dst_u, uint8* dst_v, int width);
void ARGBToYRow_Any_NEON(const uint8* src_argb, uint8* dst_y, int pix);

void I422ToARGBRow_Any_NEON(const uint8* y_buf,
                            const uint8* u_buf,
                            const uint8* v_buf,
                            uint8* rgb_buf,
                            int width);
void I422ToBGRARow_Any_NEON(const uint8* y_buf,
                            const uint8* u_buf,
                            const uint8* v_buf,
                            uint8* rgb_buf,
                            int width);
void I422ToABGRRow_Any_NEON(const uint8* y_buf,
                            const uint8* u_buf,
                            const uint8* v_buf,
                            uint8* rgb_buf,
                            int width);
void I422ToRGBARow_Any_NEON(const uint8* y_buf,
                            const uint8* u_buf,
                            const uint8* v_buf,
                            uint8* rgb_buf,
                            int width);
void I422ToRGB24Row_Any_NEON(const uint8* y_buf,
                             const uint8* u_buf,
                             const uint8* v_buf,
                             uint8* rgb_buf,
                             int width);
void I422ToRAWRow_Any_NEON(const uint8* y_buf,
                           const uint8* u_buf,
                           const uint8* v_buf,
                           uint8* rgb_buf,
                           int width);
void NV12ToARGBRow_Any_NEON(const uint8* y_buf,
                            const uint8* uv_buf,
                            uint8* argb_buf,
                            int width);
void NV21ToARGBRow_Any_NEON(const uint8* y_buf,
                            const uint8* uv_buf,
                            uint8* argb_buf,
                            int width);

void YUY2ToYRow_SSE2(const uint8* src_yuy2, uint8* dst_y, int pix);
void YUY2ToUVRow_SSE2(const uint8* src_yuy2, int stride_yuy2,
                      uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToUV422Row_SSE2(const uint8* src_yuy2,
                         uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToYRow_Unaligned_SSE2(const uint8* src_yuy2,
                               uint8* dst_y, int pix);
void YUY2ToUVRow_Unaligned_SSE2(const uint8* src_yuy2, int stride_yuy2,
                                uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToUV422Row_Unaligned_SSE2(const uint8* src_yuy2,
                                   uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToYRow_NEON(const uint8* src_yuy2, uint8* dst_y, int pix);
void YUY2ToUVRow_NEON(const uint8* src_yuy2, int stride_yuy2,
                      uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToUV422Row_NEON(const uint8* src_yuy2,
                         uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToYRow_C(const uint8* src_yuy2, uint8* dst_y, int pix);
void YUY2ToUVRow_C(const uint8* src_yuy2, int stride_yuy2,
                   uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToUV422Row_C(const uint8* src_yuy2,
                      uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToYRow_Any_SSE2(const uint8* src_yuy2, uint8* dst_y, int pix);
void YUY2ToUVRow_Any_SSE2(const uint8* src_yuy2, int stride_yuy2,
                          uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToUV422Row_Any_SSE2(const uint8* src_yuy2,
                             uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToYRow_Any_NEON(const uint8* src_yuy2, uint8* dst_y, int pix);
void YUY2ToUVRow_Any_NEON(const uint8* src_yuy2, int stride_yuy2,
                          uint8* dst_u, uint8* dst_v, int pix);
void YUY2ToUV422Row_Any_NEON(const uint8* src_yuy2,
                             uint8* dst_u, uint8* dst_v, int pix);

void UYVYToYRow_SSE2(const uint8* src_uyvy, uint8* dst_y, int pix);
void UYVYToUVRow_SSE2(const uint8* src_uyvy, int stride_uyvy,
                      uint8* dst_u, uint8* dst_v, int pix);
void UYVYToUV422Row_SSE2(const uint8* src_uyvy,
                         uint8* dst_u, uint8* dst_v, int pix);
void UYVYToYRow_Unaligned_SSE2(const uint8* src_uyvy,
                               uint8* dst_y, int pix);
void UYVYToUVRow_Unaligned_SSE2(const uint8* src_uyvy, int stride_uyvy,
                                uint8* dst_u, uint8* dst_v, int pix);
void UYVYToUV422Row_Unaligned_SSE2(const uint8* src_uyvy,
                                   uint8* dst_u, uint8* dst_v, int pix);
void UYVYToYRow_NEON(const uint8* src_uyvy, uint8* dst_y, int pix);
void UYVYToUVRow_NEON(const uint8* src_uyvy, int stride_uyvy,
                      uint8* dst_u, uint8* dst_v, int pix);
void UYVYToUV422Row_NEON(const uint8* src_uyvy,
                         uint8* dst_u, uint8* dst_v, int pix);

void UYVYToYRow_C(const uint8* src_uyvy, uint8* dst_y, int pix);
void UYVYToUVRow_C(const uint8* src_uyvy, int stride_uyvy,
                   uint8* dst_u, uint8* dst_v, int pix);
void UYVYToUV422Row_C(const uint8* src_uyvy,
                      uint8* dst_u, uint8* dst_v, int pix);
void UYVYToYRow_Any_SSE2(const uint8* src_uyvy, uint8* dst_y, int pix);
void UYVYToUVRow_Any_SSE2(const uint8* src_uyvy, int stride_uyvy,
                          uint8* dst_u, uint8* dst_v, int pix);
void UYVYToUV422Row_Any_SSE2(const uint8* src_uyvy,
                             uint8* dst_u, uint8* dst_v, int pix);
void UYVYToYRow_Any_NEON(const uint8* src_uyvy, uint8* dst_y, int pix);
void UYVYToUVRow_Any_NEON(const uint8* src_uyvy, int stride_uyvy,
                          uint8* dst_u, uint8* dst_v, int pix);
void UYVYToUV422Row_Any_NEON(const uint8* src_uyvy,
                             uint8* dst_u, uint8* dst_v, int pix);

void ARGBAttenuateRow_C(const uint8* src_argb, uint8* dst_argb, int width);
void ARGBAttenuateRow_SSE2(const uint8* src_argb, uint8* dst_argb, int width);
void ARGBAttenuateRow_SSSE3(const uint8* src_argb, uint8* dst_argb, int width);

// Inverse table for unattenuate, shared by C and SSE2.
extern uint32 fixed_invtbl8[256];
void ARGBUnattenuateRow_C(const uint8* src_argb, uint8* dst_argb, int width);
void ARGBUnattenuateRow_SSE2(const uint8* src_argb, uint8* dst_argb, int width);

void ARGBGrayRow_C(const uint8* src_argb, uint8* dst_argb, int width);
void ARGBGrayRow_SSSE3(const uint8* src_argb, uint8* dst_argb, int width);

void ARGBSepiaRow_C(uint8* dst_argb, int width);
void ARGBSepiaRow_SSSE3(uint8* dst_argb, int width);

void ARGBColorMatrixRow_C(uint8* dst_argb, const int8* matrix_argb, int width);
void ARGBColorMatrixRow_SSSE3(uint8* dst_argb, const int8* matrix_argb,
                              int width);

void ARGBColorTableRow_C(uint8* dst_argb, const uint8* table_argb, int width);
void ARGBColorTableRow_X86(uint8* dst_argb, const uint8* table_argb, int width);

void ARGBQuantizeRow_C(uint8* dst_argb, int scale, int interval_size,
                       int interval_offset, int width);
void ARGBQuantizeRow_SSE2(uint8* dst_argb, int scale, int interval_size,
                          int interval_offset, int width);

// Used for blur.
void CumulativeSumToAverage_SSE2(const int32* topleft, const int32* botleft,
                                 int width, int area, uint8* dst, int count);
void ComputeCumulativeSumRow_SSE2(const uint8* row, int32* cumsum,
                                  const int32* previous_cumsum, int width);

void CumulativeSumToAverage_C(const int32* topleft, const int32* botleft,
                              int width, int area, uint8* dst, int count);
void ComputeCumulativeSumRow_C(const uint8* row, int32* cumsum,
                               const int32* previous_cumsum, int width);

void ARGBShadeRow_C(const uint8* src_argb, uint8* dst_argb, int width,
                    uint32 value);
void ARGBShadeRow_SSE2(const uint8* src_argb, uint8* dst_argb, int width,
                       uint32 value);

LIBYUV_API
void ARGBAffineRow_C(const uint8* src_argb, int src_argb_stride,
                     uint8* dst_argb, const float* uv_dudv, int width);
LIBYUV_API
void ARGBAffineRow_SSE2(const uint8* src_argb, int src_argb_stride,
                        uint8* dst_argb, const float* uv_dudv, int width);

void ARGBInterpolateRow_C(uint8* dst_ptr, const uint8* src_ptr,
                          ptrdiff_t src_stride,
                          int dst_width, int source_y_fraction);
void ARGBInterpolateRow_SSSE3(uint8* dst_ptr, const uint8* src_ptr,
                              ptrdiff_t src_stride, int dst_width,
                              int source_y_fraction);

void HalfRow_C(const uint8* src_uv, int src_uv_stride,
               uint8* dst_uv, int pix);
void HalfRow_SSE2(const uint8* src_uv, int src_uv_stride,
                  uint8* dst_uv, int pix);
void HalfRow_NEON(const uint8* src_uv, int src_uv_stride,
                  uint8* dst_uv, int pix);

void ARGBToBayerRow_C(const uint8* src_argb,
                      uint8* dst_bayer, uint32 selector, int pix);
void ARGBToBayerRow_SSSE3(const uint8* src_argb,
                          uint8* dst_bayer, uint32 selector, int pix);
void ARGBToBayerRow_NEON(const uint8* src_argb,
                         uint8* dst_bayer, uint32 selector, int pix);

void I422ToYUY2Row_C(const uint8* src_y,
                     const uint8* src_u,
                     const uint8* src_v,
                     uint8* dst_frame, int width);
void I422ToUYVYRow_C(const uint8* src_y,
                     const uint8* src_u,
                     const uint8* src_v,
                     uint8* dst_frame, int width);
void I422ToYUY2Row_SSE2(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_frame, int width);
void I422ToUYVYRow_SSE2(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_frame, int width);
void I422ToYUY2Row_Any_SSE2(const uint8* src_y,
                            const uint8* src_u,
                            const uint8* src_v,
                            uint8* dst_frame, int width);
void I422ToUYVYRow_Any_SSE2(const uint8* src_y,
                            const uint8* src_u,
                            const uint8* src_v,
                            uint8* dst_frame, int width);
void I422ToYUY2Row_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_frame, int width);
void I422ToUYVYRow_NEON(const uint8* src_y,
                        const uint8* src_u,
                        const uint8* src_v,
                        uint8* dst_frame, int width);
void I422ToYUY2Row_Any_NEON(const uint8* src_y,
                            const uint8* src_u,
                            const uint8* src_v,
                            uint8* dst_frame, int width);
void I422ToUYVYRow_Any_NEON(const uint8* src_y,
                            const uint8* src_u,
                            const uint8* src_v,
                            uint8* dst_frame, int width);

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif

#endif  // INCLUDE_LIBYUV_ROW_H_  NOLINT

