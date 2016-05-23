/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stdlib.h>
#include <time.h>

#include "libyuv/compare.h"
#include "libyuv/convert.h"
#include "libyuv/convert_argb.h"
#include "libyuv/convert_from.h"
#include "libyuv/convert_from_argb.h"
#include "libyuv/cpu_id.h"
#include "libyuv/format_conversion.h"
#include "libyuv/planar_functions.h"
#include "libyuv/rotate.h"
#include "../unit_test/unit_test.h"

#if defined(_MSC_VER)
#define SIMD_ALIGNED(var) __declspec(align(16)) var
#else  // __GNUC__
#define SIMD_ALIGNED(var) var __attribute__((aligned(16)))
#endif

namespace libyuv {

#define TESTPLANARTOPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,           \
                       FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, W1280, N, NEG, OFF)   \
TEST_F(libyuvTest, SRC_FMT_PLANAR##To##FMT_PLANAR##N) {                        \
  const int kWidth = W1280;                                                    \
  const int kHeight = 720;                                                     \
  align_buffer_16(src_y, kWidth * kHeight + OFF);                              \
  align_buffer_16(src_u,                                                       \
                  kWidth / SRC_SUBSAMP_X * kHeight / SRC_SUBSAMP_Y + OFF);     \
  align_buffer_16(src_v,                                                       \
                  kWidth / SRC_SUBSAMP_X * kHeight / SRC_SUBSAMP_Y + OFF);     \
  align_buffer_16(dst_y_c, kWidth * kHeight);                                  \
  align_buffer_16(dst_u_c, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);          \
  align_buffer_16(dst_v_c, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);          \
  align_buffer_16(dst_y_opt, kWidth * kHeight);                                \
  align_buffer_16(dst_u_opt, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);        \
  align_buffer_16(dst_v_opt, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);        \
  srandom(time(NULL));                                                         \
  for (int i = 0; i < kHeight; ++i)                                            \
    for (int j = 0; j < kWidth; ++j)                                           \
      src_y[(i * kWidth) + j + OFF] = (random() & 0xff);                       \
  for (int i = 0; i < kHeight / SRC_SUBSAMP_Y; ++i) {                          \
    for (int j = 0; j < kWidth / SRC_SUBSAMP_X; ++j) {                         \
      src_u[(i * kWidth / SRC_SUBSAMP_X) + j + OFF] = (random() & 0xff);       \
      src_v[(i * kWidth / SRC_SUBSAMP_X) + j + OFF] = (random() & 0xff);       \
    }                                                                          \
  }                                                                            \
  MaskCpuFlags(0);                                                             \
  SRC_FMT_PLANAR##To##FMT_PLANAR(src_y + OFF, kWidth,                          \
                                 src_u + OFF, kWidth / SRC_SUBSAMP_X,          \
                                 src_v + OFF, kWidth / SRC_SUBSAMP_X,          \
                                 dst_y_c, kWidth,                              \
                                 dst_u_c, kWidth / SUBSAMP_X,                  \
                                 dst_v_c, kWidth / SUBSAMP_X,                  \
                                 kWidth, NEG kHeight);                         \
  MaskCpuFlags(-1);                                                            \
  for (int i = 0; i < benchmark_iterations_; ++i) {                            \
    SRC_FMT_PLANAR##To##FMT_PLANAR(src_y + OFF, kWidth,                        \
                                   src_u + OFF, kWidth / SRC_SUBSAMP_X,        \
                                   src_v + OFF, kWidth / SRC_SUBSAMP_X,        \
                                   dst_y_opt, kWidth,                          \
                                   dst_u_opt, kWidth / SUBSAMP_X,              \
                                   dst_v_opt, kWidth / SUBSAMP_X,              \
                                   kWidth, NEG kHeight);                       \
  }                                                                            \
  int max_diff = 0;                                                            \
  for (int i = 0; i < kHeight; ++i) {                                          \
    for (int j = 0; j < kWidth; ++j) {                                         \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_y_c[i * kWidth + j]) -                      \
              static_cast<int>(dst_y_opt[i * kWidth + j]));                    \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 1);                                                      \
  for (int i = 0; i < kHeight / SUBSAMP_Y; ++i) {                              \
    for (int j = 0; j < kWidth / SUBSAMP_X; ++j) {                             \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_u_c[i * kWidth / SUBSAMP_X + j]) -          \
              static_cast<int>(dst_u_opt[i * kWidth / SUBSAMP_X + j]));        \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 1);                                                      \
  for (int i = 0; i < kHeight / SUBSAMP_Y; ++i) {                              \
    for (int j = 0; j < kWidth / SUBSAMP_X; ++j) {                             \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_v_c[i * kWidth / SUBSAMP_X + j]) -          \
              static_cast<int>(dst_v_opt[i * kWidth / SUBSAMP_X + j]));        \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 1);                                                      \
  free_aligned_buffer_16(dst_y_c)                                              \
  free_aligned_buffer_16(dst_u_c)                                              \
  free_aligned_buffer_16(dst_v_c)                                              \
  free_aligned_buffer_16(dst_y_opt)                                            \
  free_aligned_buffer_16(dst_u_opt)                                            \
  free_aligned_buffer_16(dst_v_opt)                                            \
  free_aligned_buffer_16(src_y)                                                \
  free_aligned_buffer_16(src_u)                                                \
  free_aligned_buffer_16(src_v)                                                \
}

#define TESTPLANARTOP(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,            \
                      FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y)                        \
    TESTPLANARTOPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,               \
                   FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1280, _Opt, +, 0)         \
    TESTPLANARTOPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,               \
                   FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1280, _Unaligned, +, 1)   \
    TESTPLANARTOPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,               \
                   FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1280, _Invert, -, 0)      \
    TESTPLANARTOPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,               \
                   FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1276, _Any, +, 0)

TESTPLANARTOP(I420, 2, 2, I420, 2, 2)
TESTPLANARTOP(I422, 2, 1, I420, 2, 2)
TESTPLANARTOP(I444, 1, 1, I420, 2, 2)
TESTPLANARTOP(I411, 4, 1, I420, 2, 2)
TESTPLANARTOP(I420, 2, 2, I422, 2, 1)
TESTPLANARTOP(I420, 2, 2, I444, 1, 1)
TESTPLANARTOP(I420, 2, 2, I411, 4, 1)
TESTPLANARTOP(I420, 2, 2, I420Mirror, 2, 2)


#define TESTPLANARTOBPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,          \
                       FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, W1280, N, NEG, OFF)   \
TEST_F(libyuvTest, SRC_FMT_PLANAR##To##FMT_PLANAR##N) {                        \
  const int kWidth = W1280;                                                    \
  const int kHeight = 720;                                                     \
  align_buffer_16(src_y, kWidth * kHeight + OFF);                              \
  align_buffer_16(src_u,                                                       \
                  kWidth / SRC_SUBSAMP_X * kHeight / SRC_SUBSAMP_Y + OFF);     \
  align_buffer_16(src_v,                                                       \
                  kWidth / SRC_SUBSAMP_X * kHeight / SRC_SUBSAMP_Y + OFF);     \
  align_buffer_16(dst_y_c, kWidth * kHeight);                                  \
  align_buffer_16(dst_uv_c, kWidth * 2 / SUBSAMP_X * kHeight / SUBSAMP_Y);     \
  align_buffer_16(dst_y_opt, kWidth * kHeight);                                \
  align_buffer_16(dst_uv_opt, kWidth * 2 / SUBSAMP_X * kHeight / SUBSAMP_Y);   \
  srandom(time(NULL));                                                         \
  for (int i = 0; i < kHeight; ++i)                                            \
    for (int j = 0; j < kWidth; ++j)                                           \
      src_y[(i * kWidth) + j + OFF] = (random() & 0xff);                       \
  for (int i = 0; i < kHeight / SRC_SUBSAMP_Y; ++i) {                          \
    for (int j = 0; j < kWidth / SRC_SUBSAMP_X; ++j) {                         \
      src_u[(i * kWidth / SRC_SUBSAMP_X) + j + OFF] = (random() & 0xff);       \
      src_v[(i * kWidth / SRC_SUBSAMP_X) + j + OFF] = (random() & 0xff);       \
    }                                                                          \
  }                                                                            \
  MaskCpuFlags(0);                                                             \
  SRC_FMT_PLANAR##To##FMT_PLANAR(src_y + OFF, kWidth,                          \
                                 src_u + OFF, kWidth / SRC_SUBSAMP_X,          \
                                 src_v + OFF, kWidth / SRC_SUBSAMP_X,          \
                                 dst_y_c, kWidth,                              \
                                 dst_uv_c, kWidth * 2 / SUBSAMP_X,             \
                                 kWidth, NEG kHeight);                         \
  MaskCpuFlags(-1);                                                            \
  for (int i = 0; i < benchmark_iterations_; ++i) {                            \
    SRC_FMT_PLANAR##To##FMT_PLANAR(src_y + OFF, kWidth,                        \
                                   src_u + OFF, kWidth / SRC_SUBSAMP_X,        \
                                   src_v + OFF, kWidth / SRC_SUBSAMP_X,        \
                                   dst_y_opt, kWidth,                          \
                                   dst_uv_opt, kWidth * 2 / SUBSAMP_X,         \
                                   kWidth, NEG kHeight);                       \
  }                                                                            \
  int max_diff = 0;                                                            \
  for (int i = 0; i < kHeight; ++i) {                                          \
    for (int j = 0; j < kWidth; ++j) {                                         \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_y_c[i * kWidth + j]) -                      \
              static_cast<int>(dst_y_opt[i * kWidth + j]));                    \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 1);                                                      \
  for (int i = 0; i < kHeight / SUBSAMP_Y; ++i) {                              \
    for (int j = 0; j < kWidth * 2 / SUBSAMP_X; ++j) {                         \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_uv_c[i * kWidth * 2 / SUBSAMP_X + j]) -     \
              static_cast<int>(dst_uv_opt[i * kWidth * 2 / SUBSAMP_X + j]));   \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 1);                                                      \
  free_aligned_buffer_16(dst_y_c)                                              \
  free_aligned_buffer_16(dst_uv_c)                                             \
  free_aligned_buffer_16(dst_y_opt)                                            \
  free_aligned_buffer_16(dst_uv_opt)                                           \
  free_aligned_buffer_16(src_y)                                                \
  free_aligned_buffer_16(src_u)                                                \
  free_aligned_buffer_16(src_v)                                                \
}

#define TESTPLANARTOBP(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,           \
                      FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y)                        \
    TESTPLANARTOBPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,              \
                   FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1280, _Opt, +, 0)         \
    TESTPLANARTOBPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,              \
                   FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1280, _Unaligned, +, 1)   \
    TESTPLANARTOBPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,              \
                   FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1280, _Invert, -, 0)      \
    TESTPLANARTOBPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,              \
                   FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1276, _Any, +, 0)

TESTPLANARTOBP(I420, 2, 2, NV12, 2, 2)
TESTPLANARTOBP(I420, 2, 2, NV21, 2, 2)

#define TESTBIPLANARTOPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,         \
                         FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, W1280, N, NEG, OFF) \
TEST_F(libyuvTest, SRC_FMT_PLANAR##To##FMT_PLANAR##N) {                        \
  const int kWidth = W1280;                                                    \
  const int kHeight = 720;                                                     \
  align_buffer_16(src_y, kWidth * kHeight + OFF);                              \
  align_buffer_16(src_uv, 2 * kWidth / SRC_SUBSAMP_X *                         \
                  kHeight / SRC_SUBSAMP_Y + OFF);                              \
  align_buffer_16(dst_y_c, kWidth * kHeight);                                  \
  align_buffer_16(dst_u_c, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);          \
  align_buffer_16(dst_v_c, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);          \
  align_buffer_16(dst_y_opt, kWidth * kHeight);                                \
  align_buffer_16(dst_u_opt, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);        \
  align_buffer_16(dst_v_opt, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);        \
  srandom(time(NULL));                                                         \
  for (int i = 0; i < kHeight; ++i)                                            \
    for (int j = 0; j < kWidth; ++j)                                           \
      src_y[(i * kWidth) + j + OFF] = (random() & 0xff);                       \
  for (int i = 0; i < kHeight / SRC_SUBSAMP_Y; ++i) {                          \
    for (int j = 0; j < 2 * kWidth / SRC_SUBSAMP_X; ++j) {                     \
      src_uv[(i * 2 * kWidth / SRC_SUBSAMP_X) + j + OFF] = (random() & 0xff);  \
    }                                                                          \
  }                                                                            \
  MaskCpuFlags(0);                                                             \
  SRC_FMT_PLANAR##To##FMT_PLANAR(src_y, kWidth,                          \
                                 src_uv + OFF, 2 * kWidth / SRC_SUBSAMP_X,     \
                                 dst_y_c, kWidth,                              \
                                 dst_u_c, kWidth / SUBSAMP_X,                  \
                                 dst_v_c, kWidth / SUBSAMP_X,                  \
                                 kWidth, NEG kHeight);                         \
  MaskCpuFlags(-1);                                                            \
  for (int i = 0; i < benchmark_iterations_; ++i) {                            \
    SRC_FMT_PLANAR##To##FMT_PLANAR(src_y, kWidth,                        \
                                   src_uv + OFF, 2 * kWidth / SRC_SUBSAMP_X,   \
                                   dst_y_opt, kWidth,                          \
                                   dst_u_opt, kWidth / SUBSAMP_X,              \
                                   dst_v_opt, kWidth / SUBSAMP_X,              \
                                   kWidth, NEG kHeight);                       \
  }                                                                            \
  int max_diff = 0;                                                            \
  for (int i = 0; i < kHeight; ++i) {                                          \
    for (int j = 0; j < kWidth; ++j) {                                         \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_y_c[i * kWidth + j]) -                      \
              static_cast<int>(dst_y_opt[i * kWidth + j]));                    \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 1);                                                      \
  for (int i = 0; i < kHeight / SUBSAMP_Y; ++i) {                              \
    for (int j = 0; j < kWidth / SUBSAMP_X; ++j) {                             \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_u_c[i * kWidth / SUBSAMP_X + j]) -          \
              static_cast<int>(dst_u_opt[i * kWidth / SUBSAMP_X + j]));        \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 1);                                                      \
  for (int i = 0; i < kHeight / SUBSAMP_Y; ++i) {                              \
    for (int j = 0; j < kWidth / SUBSAMP_X; ++j) {                             \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_v_c[i * kWidth / SUBSAMP_X + j]) -          \
              static_cast<int>(dst_v_opt[i * kWidth / SUBSAMP_X + j]));        \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 1);                                                      \
  free_aligned_buffer_16(dst_y_c)                                              \
  free_aligned_buffer_16(dst_u_c)                                              \
  free_aligned_buffer_16(dst_v_c)                                              \
  free_aligned_buffer_16(dst_y_opt)                                            \
  free_aligned_buffer_16(dst_u_opt)                                            \
  free_aligned_buffer_16(dst_v_opt)                                            \
  free_aligned_buffer_16(src_y)                                                \
  free_aligned_buffer_16(src_uv)                                               \
}

#define TESTBIPLANARTOP(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,          \
                        FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y)                      \
    TESTBIPLANARTOPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,             \
                     FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1280, _Opt, +, 0)       \
    TESTBIPLANARTOPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,             \
                     FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1280, _Unaligned, +, 1) \
    TESTBIPLANARTOPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,             \
                     FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1280, _Invert, -, 0)    \
    TESTBIPLANARTOPI(SRC_FMT_PLANAR, SRC_SUBSAMP_X, SRC_SUBSAMP_Y,             \
                     FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, 1276, _Any, +, 0)

TESTBIPLANARTOP(NV12, 2, 2, I420, 2, 2)
TESTBIPLANARTOP(NV21, 2, 2, I420, 2, 2)

#define TESTPLANARTOBI(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B, ALIGN,  \
                       W1280, DIFF, N, NEG, OFF)                               \
TEST_F(libyuvTest, FMT_PLANAR##To##FMT_B##N) {                                 \
  const int kWidth = W1280;                                                    \
  const int kHeight = 720;                                                     \
  const int kStrideB = ((kWidth * 8 * BPP_B + 7) / 8 + ALIGN - 1) /            \
      ALIGN * ALIGN;                                                           \
  align_buffer_16(src_y, kWidth * kHeight + OFF);                              \
  align_buffer_16(src_u, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y + OFF);      \
  align_buffer_16(src_v, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y + OFF);      \
  align_buffer_16(dst_argb_c, kStrideB * kHeight);                             \
  align_buffer_16(dst_argb_opt, kStrideB * kHeight);                           \
  memset(dst_argb_c, 0, kStrideB * kHeight);                                   \
  memset(dst_argb_opt, 0, kStrideB * kHeight);                                 \
  srandom(time(NULL));                                                         \
  for (int i = 0; i < kHeight; ++i) {                                          \
    for (int j = 0; j < kWidth; ++j) {                                         \
      src_y[(i * kWidth) + j + OFF] = (random() & 0xff);                       \
    }                                                                          \
  }                                                                            \
  for (int i = 0; i < kHeight / SUBSAMP_Y; ++i) {                              \
    for (int j = 0; j < kWidth / SUBSAMP_X; ++j) {                             \
      src_u[(i * kWidth / SUBSAMP_X) + j + OFF] = (random() & 0xff);           \
      src_v[(i * kWidth / SUBSAMP_X) + j + OFF] = (random() & 0xff);           \
    }                                                                          \
  }                                                                            \
  MaskCpuFlags(0);                                                             \
  FMT_PLANAR##To##FMT_B(src_y + OFF, kWidth,                                   \
                        src_u + OFF, kWidth / SUBSAMP_X,                       \
                        src_v + OFF, kWidth / SUBSAMP_X,                       \
                        dst_argb_c, kStrideB,                                  \
                        kWidth, NEG kHeight);                                  \
  MaskCpuFlags(-1);                                                            \
  for (int i = 0; i < benchmark_iterations_; ++i) {                            \
    FMT_PLANAR##To##FMT_B(src_y + OFF, kWidth,                                 \
                          src_u + OFF, kWidth / SUBSAMP_X,                     \
                          src_v + OFF, kWidth / SUBSAMP_X,                     \
                          dst_argb_opt, kStrideB,                              \
                          kWidth, NEG kHeight);                                \
  }                                                                            \
  int max_diff = 0;                                                            \
  /* Convert to ARGB so 565 is expanded to bytes that can be compared. */      \
  align_buffer_16(dst_argb32_c, kWidth * 4 * kHeight);                         \
  align_buffer_16(dst_argb32_opt, kWidth * 4 * kHeight);                       \
  memset(dst_argb32_c, 0, kWidth * 4 * kHeight);                               \
  memset(dst_argb32_opt, 0, kWidth * 4 * kHeight);                             \
  FMT_B##ToARGB(dst_argb_c, kStrideB,                                          \
                dst_argb32_c, kWidth * 4,                                      \
                kWidth, kHeight);                                              \
  FMT_B##ToARGB(dst_argb_opt, kStrideB,                                        \
                dst_argb32_opt, kWidth * 4,                                    \
                kWidth, kHeight);                                              \
  for (int i = 0; i < kHeight; ++i) {                                          \
    for (int j = 0; j < kWidth * 4; ++j) {                                     \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_argb32_c[i * kWidth * 4 + j]) -             \
              static_cast<int>(dst_argb32_opt[i * kWidth * 4 + j]));           \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
                                                                               \
  EXPECT_LE(max_diff, DIFF);                                                   \
  free_aligned_buffer_16(src_y)                                                \
  free_aligned_buffer_16(src_u)                                                \
  free_aligned_buffer_16(src_v)                                                \
  free_aligned_buffer_16(dst_argb_c)                                           \
  free_aligned_buffer_16(dst_argb_opt)                                         \
  free_aligned_buffer_16(dst_argb32_c)                                         \
  free_aligned_buffer_16(dst_argb32_opt)                                       \
}

#define TESTPLANARTOB(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B, ALIGN,   \
                      DIFF)                                                    \
    TESTPLANARTOBI(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B, ALIGN,      \
                   1280, DIFF, _Opt, +, 0)                                     \
    TESTPLANARTOBI(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B, ALIGN,      \
                   1280, DIFF, _Unaligned, +, 1)                               \
    TESTPLANARTOBI(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B, ALIGN,      \
                   1280, DIFF, _Invert, -, 0)                                  \
    TESTPLANARTOBI(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B, ALIGN,      \
                   1276, DIFF, _Any, +, 0)

TESTPLANARTOB(I420, 2, 2, ARGB, 4, 4, 2)
TESTPLANARTOB(I420, 2, 2, BGRA, 4, 4, 2)
TESTPLANARTOB(I420, 2, 2, ABGR, 4, 4, 2)
TESTPLANARTOB(I420, 2, 2, RGBA, 4, 4, 2)
TESTPLANARTOB(I420, 2, 2, RAW, 3, 3, 2)
TESTPLANARTOB(I420, 2, 2, RGB24, 3, 3, 2)
TESTPLANARTOB(I420, 2, 2, RGB565, 2, 2, 9)
TESTPLANARTOB(I420, 2, 2, ARGB1555, 2, 2, 9)
TESTPLANARTOB(I420, 2, 2, ARGB4444, 2, 2, 17)
TESTPLANARTOB(I422, 2, 1, ARGB, 4, 4, 2)
TESTPLANARTOB(I422, 2, 1, BGRA, 4, 4, 2)
TESTPLANARTOB(I422, 2, 1, ABGR, 4, 4, 2)
TESTPLANARTOB(I422, 2, 1, RGBA, 4, 4, 2)
TESTPLANARTOB(I411, 4, 1, ARGB, 4, 4, 2)
TESTPLANARTOB(I444, 1, 1, ARGB, 4, 4, 2)
// TODO(fbarchard): Fix TESTPLANARTOB(I420, 2, 2, V210, 16 / 6, 128)
TESTPLANARTOB(I420, 2, 2, YUY2, 2, 4, 1)
TESTPLANARTOB(I420, 2, 2, UYVY, 2, 4, 1)
TESTPLANARTOB(I422, 2, 1, YUY2, 2, 4, 0)
TESTPLANARTOB(I422, 2, 1, UYVY, 2, 4, 0)
TESTPLANARTOB(I420, 2, 2, I400, 1, 1, 0)
TESTPLANARTOB(I420, 2, 2, BayerBGGR, 1, 1, 2)
TESTPLANARTOB(I420, 2, 2, BayerRGGB, 1, 1, 2)
TESTPLANARTOB(I420, 2, 2, BayerGBRG, 1, 1, 2)
TESTPLANARTOB(I420, 2, 2, BayerGRBG, 1, 1, 2)

#define TESTBIPLANARTOBI(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B,       \
                         W1280, DIFF, N, NEG, OFF)                             \
TEST_F(libyuvTest, FMT_PLANAR##To##FMT_B##N) {                                 \
  const int kWidth = W1280;                                                    \
  const int kHeight = 720;                                                     \
  const int kStrideB = kWidth * BPP_B;                                         \
  align_buffer_16(src_y, kWidth * kHeight + OFF);                              \
  align_buffer_16(src_uv, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y * 2 + OFF); \
  align_buffer_16(dst_argb_c, kStrideB * kHeight);                             \
  align_buffer_16(dst_argb_opt, kStrideB * kHeight);                           \
  srandom(time(NULL));                                                         \
  for (int i = 0; i < kHeight; ++i)                                            \
    for (int j = 0; j < kWidth; ++j)                                           \
      src_y[(i * kWidth) + j + OFF] = (random() & 0xff);                       \
  for (int i = 0; i < kHeight / SUBSAMP_Y; ++i)                                \
    for (int j = 0; j < kWidth / SUBSAMP_X * 2; ++j) {                         \
      src_uv[(i * kWidth / SUBSAMP_X) * 2 + j + OFF] = (random() & 0xff);      \
    }                                                                          \
  MaskCpuFlags(0);                                                             \
  FMT_PLANAR##To##FMT_B(src_y + OFF, kWidth,                                   \
                        src_uv + OFF, kWidth / SUBSAMP_X * 2,                  \
                        dst_argb_c, kWidth * BPP_B,                            \
                        kWidth, NEG kHeight);                                  \
  MaskCpuFlags(-1);                                                            \
  for (int i = 0; i < benchmark_iterations_; ++i) {                            \
    FMT_PLANAR##To##FMT_B(src_y + OFF, kWidth,                                 \
                          src_uv + OFF, kWidth / SUBSAMP_X * 2,                \
                          dst_argb_opt, kWidth * BPP_B,                        \
                          kWidth, NEG kHeight);                                \
  }                                                                            \
  /* Convert to ARGB so 565 is expanded to bytes that can be compared. */      \
  align_buffer_16(dst_argb32_c, kWidth * 4 * kHeight);                         \
  align_buffer_16(dst_argb32_opt, kWidth * 4 * kHeight);                       \
  memset(dst_argb32_c, 0, kWidth * 4 * kHeight);                               \
  memset(dst_argb32_opt, 0, kWidth * 4 * kHeight);                             \
  FMT_B##ToARGB(dst_argb_c, kStrideB,                                          \
                dst_argb32_c, kWidth * 4,                                      \
                kWidth, kHeight);                                              \
  FMT_B##ToARGB(dst_argb_opt, kStrideB,                                        \
                dst_argb32_opt, kWidth * 4,                                    \
                kWidth, kHeight);                                              \
  int max_diff = 0;                                                            \
  for (int i = 0; i < kHeight; ++i) {                                          \
    for (int j = 0; j < kWidth * 4; ++j) {                                     \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_argb32_c[i * kWidth * 4 + j]) -             \
              static_cast<int>(dst_argb32_opt[i * kWidth * 4 + j]));           \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, DIFF);                                                   \
  free_aligned_buffer_16(src_y)                                                \
  free_aligned_buffer_16(src_uv)                                               \
  free_aligned_buffer_16(dst_argb_c)                                           \
  free_aligned_buffer_16(dst_argb_opt)                                         \
  free_aligned_buffer_16(dst_argb32_c)                                         \
  free_aligned_buffer_16(dst_argb32_opt)                                       \
}

#define TESTBIPLANARTOB(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B, DIFF)  \
    TESTBIPLANARTOBI(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B,           \
                     1280, DIFF, _Opt, +, 0)                                   \
    TESTBIPLANARTOBI(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B,           \
                     1280, DIFF, _Unaligned, +, 1)                             \
    TESTBIPLANARTOBI(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B,           \
                     1280, DIFF, _Invert, -, 0)                                \
    TESTBIPLANARTOBI(FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y, FMT_B, BPP_B,           \
                     1276, DIFF, _Any, +, 0)

TESTBIPLANARTOB(NV12, 2, 2, ARGB, 4, 2)
TESTBIPLANARTOB(NV21, 2, 2, ARGB, 4, 2)
TESTBIPLANARTOB(NV12, 2, 2, RGB565, 2, 9)
TESTBIPLANARTOB(NV21, 2, 2, RGB565, 2, 9)

#define TESTATOPLANARI(FMT_A, BPP_A, FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y,         \
                       W1280, N, NEG, OFF)                                     \
TEST_F(libyuvTest, FMT_A##To##FMT_PLANAR##N) {                                 \
  const int kWidth = W1280;                                                    \
  const int kHeight = 720;                                                     \
  const int kStride = (kWidth * 8 * BPP_A + 7) / 8;                            \
  align_buffer_16(src_argb, kStride * kHeight + OFF);                          \
  align_buffer_16(dst_y_c, kWidth * kHeight);                                  \
  align_buffer_16(dst_u_c, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);          \
  align_buffer_16(dst_v_c, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);          \
  align_buffer_16(dst_y_opt, kWidth * kHeight);                                \
  align_buffer_16(dst_u_opt, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);        \
  align_buffer_16(dst_v_opt, kWidth / SUBSAMP_X * kHeight / SUBSAMP_Y);        \
  srandom(time(NULL));                                                         \
  for (int i = 0; i < kHeight; ++i)                                            \
    for (int j = 0; j < kStride; ++j)                                          \
      src_argb[(i * kStride) + j + OFF] = (random() & 0xff);                   \
  MaskCpuFlags(0);                                                             \
  FMT_A##To##FMT_PLANAR(src_argb + OFF, kStride,                               \
                        dst_y_c, kWidth,                                       \
                        dst_u_c, kWidth / SUBSAMP_X,                           \
                        dst_v_c, kWidth / SUBSAMP_X,                           \
                        kWidth, NEG kHeight);                                  \
  MaskCpuFlags(-1);                                                            \
  for (int i = 0; i < benchmark_iterations_; ++i) {                            \
    FMT_A##To##FMT_PLANAR(src_argb + OFF, kStride,                             \
                          dst_y_opt, kWidth,                                   \
                          dst_u_opt, kWidth / SUBSAMP_X,                       \
                          dst_v_opt, kWidth / SUBSAMP_X,                       \
                          kWidth, NEG kHeight);                                \
  }                                                                            \
  int max_diff = 0;                                                            \
  for (int i = 0; i < kHeight; ++i) {                                          \
    for (int j = 0; j < kWidth; ++j) {                                         \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_y_c[i * kWidth + j]) -                      \
              static_cast<int>(dst_y_opt[i * kWidth + j]));                    \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 2);                                                      \
  for (int i = 0; i < kHeight / SUBSAMP_Y; ++i) {                              \
    for (int j = 0; j < kWidth / SUBSAMP_X; ++j) {                             \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_u_c[i * kWidth / SUBSAMP_X + j]) -          \
              static_cast<int>(dst_u_opt[i * kWidth / SUBSAMP_X + j]));        \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 2);                                                      \
  for (int i = 0; i < kHeight / SUBSAMP_Y; ++i) {                              \
    for (int j = 0; j < kWidth / SUBSAMP_X; ++j) {                             \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_v_c[i * kWidth / SUBSAMP_X + j]) -          \
              static_cast<int>(dst_v_opt[i * kWidth / SUBSAMP_X + j]));        \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, 2);                                                      \
  free_aligned_buffer_16(dst_y_c)                                              \
  free_aligned_buffer_16(dst_u_c)                                              \
  free_aligned_buffer_16(dst_v_c)                                              \
  free_aligned_buffer_16(dst_y_opt)                                            \
  free_aligned_buffer_16(dst_u_opt)                                            \
  free_aligned_buffer_16(dst_v_opt)                                            \
  free_aligned_buffer_16(src_argb)                                             \
}

#define TESTATOPLANAR(FMT_A, BPP_A, FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y)          \
    TESTATOPLANARI(FMT_A, BPP_A, FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y,             \
                   1280, _Opt, +, 0)                                           \
    TESTATOPLANARI(FMT_A, BPP_A, FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y,             \
                   1280, _Unaligned, +, 1)                                     \
    TESTATOPLANARI(FMT_A, BPP_A, FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y,             \
                   1280, _Invert, -, 0)                                        \
    TESTATOPLANARI(FMT_A, BPP_A, FMT_PLANAR, SUBSAMP_X, SUBSAMP_Y,             \
                   1276, _Any, +, 0)

TESTATOPLANAR(ARGB, 4, I420, 2, 2)
TESTATOPLANAR(BGRA, 4, I420, 2, 2)
TESTATOPLANAR(ABGR, 4, I420, 2, 2)
TESTATOPLANAR(RGBA, 4, I420, 2, 2)
TESTATOPLANAR(RAW, 3, I420, 2, 2)
TESTATOPLANAR(RGB24, 3, I420, 2, 2)
TESTATOPLANAR(RGB565, 2, I420, 2, 2)
TESTATOPLANAR(ARGB1555, 2, I420, 2, 2)
TESTATOPLANAR(ARGB4444, 2, I420, 2, 2)
// TESTATOPLANAR(ARGB, 4, I411, 4, 1)
TESTATOPLANAR(ARGB, 4, I422, 2, 1)
// TESTATOPLANAR(ARGB, 4, I444, 1, 1)
// TODO(fbarchard): Implement and test 411 and 444
TESTATOPLANAR(V210, 16 / 6, I420, 2, 2)
TESTATOPLANAR(YUY2, 2, I420, 2, 2)
TESTATOPLANAR(UYVY, 2, I420, 2, 2)
TESTATOPLANAR(YUY2, 2, I422, 2, 1)
TESTATOPLANAR(UYVY, 2, I422, 2, 1)
TESTATOPLANAR(I400, 1, I420, 2, 2)
TESTATOPLANAR(BayerBGGR, 1, I420, 2, 2)
TESTATOPLANAR(BayerRGGB, 1, I420, 2, 2)
TESTATOPLANAR(BayerGBRG, 1, I420, 2, 2)
TESTATOPLANAR(BayerGRBG, 1, I420, 2, 2)

#define TESTATOBI(FMT_A, BPP_A, STRIDE_A, FMT_B, BPP_B, W1280, DIFF,           \
                  N, NEG, OFF)                                                 \
TEST_F(libyuvTest, FMT_A##To##FMT_B##N) {                                      \
  const int kWidth = W1280;                                                    \
  const int kHeight = 720;                                                     \
  align_buffer_16(src_argb, (kWidth * BPP_A) * kHeight + OFF);                 \
  align_buffer_16(dst_argb_c, (kWidth * BPP_B) * kHeight);                     \
  align_buffer_16(dst_argb_opt, (kWidth * BPP_B) * kHeight);                   \
  srandom(time(NULL));                                                         \
  for (int i = 0; i < kHeight * kWidth * BPP_A; ++i) {                         \
    src_argb[i + OFF] = (random() & 0xff);                                     \
  }                                                                            \
  MaskCpuFlags(0);                                                             \
  FMT_A##To##FMT_B(src_argb + OFF, kWidth * STRIDE_A,                          \
                   dst_argb_c, kWidth * BPP_B,                                 \
                   kWidth, NEG kHeight);                                       \
  MaskCpuFlags(-1);                                                            \
  for (int i = 0; i < benchmark_iterations_; ++i) {                            \
    FMT_A##To##FMT_B(src_argb + OFF, kWidth * STRIDE_A,                        \
                     dst_argb_opt, kWidth * BPP_B,                             \
                     kWidth, NEG kHeight);                                     \
  }                                                                            \
  int max_diff = 0;                                                            \
  for (int i = 0; i < kHeight * kWidth * BPP_B; ++i) {                         \
    int abs_diff =                                                             \
        abs(static_cast<int>(dst_argb_c[i]) -                                  \
            static_cast<int>(dst_argb_opt[i]));                                \
    if (abs_diff > max_diff) {                                                 \
      max_diff = abs_diff;                                                     \
    }                                                                          \
  }                                                                            \
  EXPECT_LE(max_diff, DIFF);                                                   \
  free_aligned_buffer_16(src_argb)                                             \
  free_aligned_buffer_16(dst_argb_c)                                           \
  free_aligned_buffer_16(dst_argb_opt)                                         \
}
#define TESTATOB(FMT_A, BPP_A, STRIDE_A, FMT_B, BPP_B, DIFF)                   \
    TESTATOBI(FMT_A, BPP_A, STRIDE_A, FMT_B, BPP_B, 1280, DIFF, _Opt, +, 0)    \
    TESTATOBI(FMT_A, BPP_A, STRIDE_A, FMT_B, BPP_B, 1280, DIFF,                \
              _Unaligned, +, 1)                                                \
    TESTATOBI(FMT_A, BPP_A, STRIDE_A, FMT_B, BPP_B, 1280, DIFF, _Invert, -, 0) \
    TESTATOBI(FMT_A, BPP_A, STRIDE_A, FMT_B, BPP_B, 1280, DIFF, _Any, +, 0)

TESTATOB(ARGB, 4, 4, ARGB, 4, 0)
TESTATOB(ARGB, 4, 4, BGRA, 4, 0)
TESTATOB(ARGB, 4, 4, ABGR, 4, 0)
TESTATOB(ARGB, 4, 4, RGBA, 4, 0)
TESTATOB(ARGB, 4, 4, RAW, 3, 0)
TESTATOB(ARGB, 4, 4, RGB24, 3, 0)
TESTATOB(ARGB, 4, 4, RGB565, 2, 0)
TESTATOB(ARGB, 4, 4, ARGB1555, 2, 0)
TESTATOB(ARGB, 4, 4, ARGB4444, 2, 0)
TESTATOB(ARGB, 4, 4, BayerBGGR, 1, 0)
TESTATOB(ARGB, 4, 4, BayerRGGB, 1, 0)
TESTATOB(ARGB, 4, 4, BayerGBRG, 1, 0)
TESTATOB(ARGB, 4, 4, BayerGRBG, 1, 0)
TESTATOB(ARGB, 4, 4, I400, 1, 2)
TESTATOB(BGRA, 4, 4, ARGB, 4, 0)
TESTATOB(ABGR, 4, 4, ARGB, 4, 0)
TESTATOB(RGBA, 4, 4, ARGB, 4, 0)
TESTATOB(RAW, 3, 3, ARGB, 4, 0)
TESTATOB(RGB24, 3, 3, ARGB, 4, 0)
TESTATOB(RGB565, 2, 2, ARGB, 4, 0)
TESTATOB(ARGB1555, 2, 2, ARGB, 4, 0)
TESTATOB(ARGB4444, 2, 2, ARGB, 4, 0)
TESTATOB(YUY2, 2, 2, ARGB, 4, 0)
TESTATOB(UYVY, 2, 2, ARGB, 4, 0)
TESTATOB(M420, 3 / 2, 1, ARGB, 4, 0)
TESTATOB(BayerBGGR, 1, 1, ARGB, 4, 0)
TESTATOB(BayerRGGB, 1, 1, ARGB, 4, 0)
TESTATOB(BayerGBRG, 1, 1, ARGB, 4, 0)
TESTATOB(BayerGRBG, 1, 1, ARGB, 4, 0)
TESTATOB(I400, 1, 1, ARGB, 4, 0)
TESTATOB(I400, 1, 1, I400, 1, 0)
TESTATOB(ARGB, 4, 4, ARGBMirror, 4, 0)

#define TESTATOBRANDOM(FMT_A, BPP_A, STRIDE_A, FMT_B, BPP_B, STRIDE_B, DIFF)   \
TEST_F(libyuvTest, FMT_A##To##FMT_B##_Random) {                                \
  srandom(time(NULL));                                                         \
  for (int times = 0; times < benchmark_iterations_; ++times) {                \
    const int kWidth = (random() & 63) + 1;                                    \
    const int kHeight = (random() & 31) + 1;                                   \
    const int kStrideA = (kWidth * BPP_A + STRIDE_A - 1) / STRIDE_A * STRIDE_A;\
    const int kStrideB = (kWidth * BPP_B + STRIDE_B - 1) / STRIDE_B * STRIDE_B;\
    align_buffer_page_end(src_argb, kStrideA * kHeight);                       \
    align_buffer_page_end(dst_argb_c, kStrideB * kHeight);                     \
    align_buffer_page_end(dst_argb_opt, kStrideB * kHeight);                   \
    for (int i = 0; i < kStrideA * kHeight; ++i) {                             \
      src_argb[i] = (random() & 0xff);                                         \
    }                                                                          \
    MaskCpuFlags(0);                                                           \
    FMT_A##To##FMT_B(src_argb, kStrideA,                                       \
                     dst_argb_c, kStrideB,                                     \
                     kWidth, kHeight);                                         \
    MaskCpuFlags(-1);                                                          \
    FMT_A##To##FMT_B(src_argb, kStrideA,                                       \
                     dst_argb_opt, kStrideB,                                   \
                     kWidth, kHeight);                                         \
    int max_diff = 0;                                                          \
    for (int i = 0; i < kStrideB * kHeight; ++i) {                             \
      int abs_diff =                                                           \
          abs(static_cast<int>(dst_argb_c[i]) -                                \
              static_cast<int>(dst_argb_opt[i]));                              \
      if (abs_diff > max_diff) {                                               \
        max_diff = abs_diff;                                                   \
      }                                                                        \
    }                                                                          \
    EXPECT_LE(max_diff, DIFF);                                                 \
    free_aligned_buffer_page_end(src_argb)                                     \
    free_aligned_buffer_page_end(dst_argb_c)                                   \
    free_aligned_buffer_page_end(dst_argb_opt)                                 \
  }                                                                            \
}

TESTATOBRANDOM(ARGB, 4, 4, ARGB, 4, 4, 0)
TESTATOBRANDOM(ARGB, 4, 4, BGRA, 4, 4, 0)
TESTATOBRANDOM(ARGB, 4, 4, ABGR, 4, 4, 0)
TESTATOBRANDOM(ARGB, 4, 4, RGBA, 4, 4, 0)
TESTATOBRANDOM(ARGB, 4, 4, RAW, 3, 3, 0)
TESTATOBRANDOM(ARGB, 4, 4, RGB24, 3, 3, 0)
TESTATOBRANDOM(ARGB, 4, 4, RGB565, 2, 2, 0)
TESTATOBRANDOM(ARGB, 4, 4, ARGB1555, 2, 2, 0)
TESTATOBRANDOM(ARGB, 4, 4, ARGB4444, 2, 2, 0)
TESTATOBRANDOM(ARGB, 4, 4, I400, 1, 1, 2)
// TODO(fbarchard, 0): Implement YUY2
// TESTATOBRANDOM(ARGB, 4, 4, YUY2, 4, 2, 0)
// TESTATOBRANDOM(ARGB, 4, 4, UYVY, 4, 2, 0)
TESTATOBRANDOM(BGRA, 4, 4, ARGB, 4, 4, 0)
TESTATOBRANDOM(ABGR, 4, 4, ARGB, 4, 4, 0)
TESTATOBRANDOM(RGBA, 4, 4, ARGB, 4, 4, 0)
TESTATOBRANDOM(RAW, 3, 3, ARGB, 4, 4, 0)
TESTATOBRANDOM(RGB24, 3, 3, ARGB, 4, 4, 0)
TESTATOBRANDOM(RGB565, 2, 2, ARGB, 4, 4, 0)
TESTATOBRANDOM(ARGB1555, 2, 2, ARGB, 4, 4, 0)
TESTATOBRANDOM(ARGB4444, 2, 2, ARGB, 4, 4, 0)
TESTATOBRANDOM(I400, 1, 1, ARGB, 4, 4, 0)
TESTATOBRANDOM(YUY2, 4, 2, ARGB, 4, 4, 0)
TESTATOBRANDOM(UYVY, 4, 2, ARGB, 4, 4, 0)
TESTATOBRANDOM(I400, 1, 1, I400, 1, 1, 0)
TESTATOBRANDOM(ARGB, 4, 4, ARGBMirror, 4, 4, 0)

TEST_F(libyuvTest, Test565) {
  SIMD_ALIGNED(uint8 orig_pixels[256][4]);
  SIMD_ALIGNED(uint8 pixels565[256][2]);

  for (int i = 0; i < 256; ++i) {
    for (int j = 0; j < 4; ++j) {
      orig_pixels[i][j] = i;
    }
  }
  ARGBToRGB565(&orig_pixels[0][0], 0, &pixels565[0][0], 0, 256, 1);
  uint32 checksum = HashDjb2(&pixels565[0][0], sizeof(pixels565), 5381);
  EXPECT_EQ(610919429u, checksum);
}

}  // namespace libyuv

