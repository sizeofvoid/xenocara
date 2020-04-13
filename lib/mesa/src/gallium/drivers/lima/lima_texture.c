/*
 * Copyright (c) 2011-2013 Luc Verhaegen <libv@skynet.be>
 * Copyright (c) 2018-2019 Lima Project
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sub license,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 */

#include "util/u_memory.h"
#include "util/u_upload_mgr.h"
#include "util/u_math.h"
#include "util/u_debug.h"
#include "util/u_transfer.h"

#include "lima_bo.h"
#include "lima_context.h"
#include "lima_screen.h"
#include "lima_texture.h"
#include "lima_resource.h"
#include "lima_submit.h"
#include "lima_util.h"

#include <drm-uapi/lima_drm.h>

#define LIMA_TEXEL_FORMAT_L8           0x09
#define LIMA_TEXEL_FORMAT_A8           0x0a
#define LIMA_TEXEL_FORMAT_I8           0x0b
#define LIMA_TEXEL_FORMAT_BGR_565      0x0e
#define LIMA_TEXEL_FORMAT_L8A8         0x11
#define LIMA_TEXEL_FORMAT_L16          0x12
#define LIMA_TEXEL_FORMAT_A16          0x13
#define LIMA_TEXEL_FORMAT_I16          0x14
#define LIMA_TEXEL_FORMAT_RGB_888      0x15
#define LIMA_TEXEL_FORMAT_RGBA_8888    0x16
#define LIMA_TEXEL_FORMAT_RGBX_8888    0x17
#define LIMA_TEXEL_FORMAT_Z24S8        0x2c

#define lima_tex_list_size 64

typedef struct {
   bool present;
   uint32_t lima_format;
   bool swap_r_b;
} lima_format;

#define LIMA_FORMAT(pipe, lima, swap) \
        [PIPE_FORMAT_##pipe] = { .present = true, .lima_format = lima, \
                                 .swap_r_b = swap }

static const lima_format lima_format_table[] = {
   LIMA_FORMAT(R8G8B8A8_UNORM, LIMA_TEXEL_FORMAT_RGBA_8888, true),
   LIMA_FORMAT(B8G8R8A8_UNORM, LIMA_TEXEL_FORMAT_RGBA_8888, false),
   LIMA_FORMAT(R8G8B8A8_SRGB, LIMA_TEXEL_FORMAT_RGBA_8888, true),
   LIMA_FORMAT(B8G8R8A8_SRGB, LIMA_TEXEL_FORMAT_RGBA_8888, false),
   LIMA_FORMAT(R8G8B8X8_UNORM, LIMA_TEXEL_FORMAT_RGBX_8888, true),
   LIMA_FORMAT(B8G8R8X8_UNORM, LIMA_TEXEL_FORMAT_RGBX_8888, false),
   LIMA_FORMAT(R8G8B8_UNORM, LIMA_TEXEL_FORMAT_RGB_888, true),
   LIMA_FORMAT(B5G6R5_UNORM, LIMA_TEXEL_FORMAT_BGR_565, false),
   LIMA_FORMAT(Z24_UNORM_S8_UINT, LIMA_TEXEL_FORMAT_Z24S8, false),
   LIMA_FORMAT(Z24X8_UNORM, LIMA_TEXEL_FORMAT_Z24S8, false),
   /* Blob uses L16 for Z16 */
   LIMA_FORMAT(Z16_UNORM, LIMA_TEXEL_FORMAT_L16, false),
   LIMA_FORMAT(L16_UNORM, LIMA_TEXEL_FORMAT_L16, false),
   LIMA_FORMAT(L8_UNORM, LIMA_TEXEL_FORMAT_L8, false),
   LIMA_FORMAT(A16_UNORM, LIMA_TEXEL_FORMAT_A16, false),
   LIMA_FORMAT(A8_UNORM, LIMA_TEXEL_FORMAT_A8, false),
   LIMA_FORMAT(I16_UNORM, LIMA_TEXEL_FORMAT_I16, false),
   LIMA_FORMAT(I8_UNORM, LIMA_TEXEL_FORMAT_I8, false),
   LIMA_FORMAT(L8A8_UNORM, LIMA_TEXEL_FORMAT_L8A8, false),
};

static_assert(offsetof(lima_tex_desc, va) == 24, "lima_tex_desc->va offset isn't 24");

bool
lima_texel_format_supported(enum pipe_format pformat)
{
   if (pformat >= ARRAY_SIZE(lima_format_table))
      return false;

   return lima_format_table[pformat].present;
}

static void
lima_texture_desc_set_va(lima_tex_desc *desc,
                         int idx,
                         uint32_t va)
{
   unsigned va_bit_idx = VA_BIT_OFFSET + (VA_BIT_SIZE * idx);
   unsigned va_idx = va_bit_idx / 32;
   va_bit_idx %= 32;

   va >>= 6;

   desc->va[va_idx] |= va << va_bit_idx;
   if (va_bit_idx <= 6)
      return;
   desc->va[va_idx + 1] |= va >> (32 - va_bit_idx);
}

void
lima_texture_desc_set_res(struct lima_context *ctx, lima_tex_desc *desc,
                          struct pipe_resource *prsc,
                          unsigned first_level, unsigned last_level)
{
   unsigned width, height, layout, i;
   struct lima_resource *lima_res = lima_resource(prsc);

   width = prsc->width0;
   height = prsc->height0;
   if (first_level != 0) {
      width = u_minify(width, first_level);
      height = u_minify(height, first_level);
   }

   assert(prsc->format < ARRAY_SIZE(lima_format_table));
   assert(lima_format_table[prsc->format].present);

   desc->format = lima_format_table[prsc->format].lima_format;
   desc->swap_r_b = lima_format_table[prsc->format].swap_r_b;
   desc->width  = width;
   desc->height = height;
   desc->unknown_3_1 = 1;

   if (lima_res->tiled)
      layout = 3;
   else {
      /* for padded linear texture */
      if (lima_res->levels[first_level].width != width) {
         desc->stride = lima_res->levels[first_level].width;
         desc->has_stride = 1;
      }
      layout = 0;
   }

   lima_submit_add_bo(ctx->pp_submit, lima_res->bo, LIMA_SUBMIT_BO_READ);

   uint32_t base_va = lima_res->bo->va;

   /* attach first level */
   uint32_t first_va = base_va + lima_res->levels[first_level].offset;
   desc->va_s.va_0 = first_va >> 6;
   desc->va_s.layout = layout;

   /* Attach remaining levels.
    * Each subsequent mipmap address is specified using the 26 msbs.
    * These addresses are then packed continuously in memory */
   for (i = 1; i <= (last_level - first_level); i++) {
      uint32_t address = base_va + lima_res->levels[first_level + i].offset;
      lima_texture_desc_set_va(desc, i, address);
   }
}

static void
lima_update_tex_desc(struct lima_context *ctx, struct lima_sampler_state *sampler,
                     struct lima_sampler_view *texture, void *pdesc,
                     unsigned desc_size)
{
   lima_tex_desc *desc = pdesc;
   unsigned first_level;
   unsigned last_level;
   bool mipmapping;

   memset(desc, 0, desc_size);

   /* 2D texture */
   desc->texture_2d = 1;

   first_level = texture->base.u.tex.first_level;
   last_level = texture->base.u.tex.last_level;
   if (last_level - first_level >= LIMA_MAX_MIP_LEVELS)
      last_level = first_level + LIMA_MAX_MIP_LEVELS - 1;

   switch (sampler->base.min_mip_filter) {
      case PIPE_TEX_MIPFILTER_LINEAR:
         desc->min_mipfilter = 3;
      case PIPE_TEX_MIPFILTER_NEAREST:
         mipmapping = true;
         desc->miplevels = (last_level - first_level);
         break;
      case PIPE_TEX_MIPFILTER_NONE:
      default:
         mipmapping = false;
         break;
   }

   switch (sampler->base.mag_img_filter) {
   case PIPE_TEX_FILTER_LINEAR:
      desc->mag_img_filter_nearest = 0;
      /* no mipmap, filter_mag = linear */
      if (!mipmapping)
         desc->disable_mipmap = 1;
      break;
   case PIPE_TEX_FILTER_NEAREST:
   default:
      desc->mag_img_filter_nearest = 1;
      break;
   }

   switch (sampler->base.min_img_filter) {
      break;
   case PIPE_TEX_FILTER_LINEAR:
      desc->min_img_filter_nearest = 0;
      break;
   case PIPE_TEX_FILTER_NEAREST:
   default:
      desc->min_img_filter_nearest = 1;
      break;
   }

   /* Only clamp, clamp to edge, repeat and mirror repeat are supported */
   switch (sampler->base.wrap_s) {
   case PIPE_TEX_WRAP_CLAMP:
      desc->wrap_s_clamp = 1;
      break;
   case PIPE_TEX_WRAP_CLAMP_TO_EDGE:
   case PIPE_TEX_WRAP_CLAMP_TO_BORDER:
      desc->wrap_s_clamp_to_edge = 1;
      break;
   case PIPE_TEX_WRAP_MIRROR_REPEAT:
      desc->wrap_s_mirror_repeat = 1;
      break;
   case PIPE_TEX_WRAP_REPEAT:
   default:
      break;
   }

   /* Only clamp, clamp to edge, repeat and mirror repeat are supported */
   switch (sampler->base.wrap_t) {
   case PIPE_TEX_WRAP_CLAMP:
      desc->wrap_t_clamp = 1;
      break;
   case PIPE_TEX_WRAP_CLAMP_TO_EDGE:
   case PIPE_TEX_WRAP_CLAMP_TO_BORDER:
      desc->wrap_t_clamp_to_edge = 1;
      break;
   case PIPE_TEX_WRAP_MIRROR_REPEAT:
      desc->wrap_t_mirror_repeat = 1;
      break;
   case PIPE_TEX_WRAP_REPEAT:
   default:
      break;
   }

   lima_texture_desc_set_res(ctx, desc, texture->base.texture,
                             first_level, last_level);
}

static unsigned
lima_calc_tex_desc_size(struct lima_sampler_view *texture)
{
   unsigned size = offsetof(lima_tex_desc, va);
   unsigned va_bit_size;
   unsigned first_level = texture->base.u.tex.first_level;
   unsigned last_level = texture->base.u.tex.last_level;

   if (last_level - first_level >= LIMA_MAX_MIP_LEVELS)
      last_level = first_level + LIMA_MAX_MIP_LEVELS - 1;

   va_bit_size = VA_BIT_OFFSET + VA_BIT_SIZE * (last_level - first_level + 1);
   size += (va_bit_size + 7) >> 3;
   size = align(size, lima_min_tex_desc_size);

   return size;
}

void
lima_update_textures(struct lima_context *ctx)
{
   struct lima_texture_stateobj *lima_tex = &ctx->tex_stateobj;

   assert (lima_tex->num_samplers <= 16);

   /* Nothing to do - we have no samplers or textures */
   if (!lima_tex->num_samplers || !lima_tex->num_textures)
      return;

   unsigned size = lima_tex_list_size;
   for (int i = 0; i < lima_tex->num_samplers; i++) {
      struct lima_sampler_view *texture = lima_sampler_view(lima_tex->textures[i]);
      size += lima_calc_tex_desc_size(texture);
   }

   uint32_t *descs =
      lima_ctx_buff_alloc(ctx, lima_ctx_buff_pp_tex_desc, size, true);

   off_t offset = lima_tex_list_size;
   for (int i = 0; i < lima_tex->num_samplers; i++) {
      struct lima_sampler_state *sampler = lima_sampler_state(lima_tex->samplers[i]);
      struct lima_sampler_view *texture = lima_sampler_view(lima_tex->textures[i]);
      unsigned desc_size = lima_calc_tex_desc_size(texture);

      descs[i] = lima_ctx_buff_va(ctx, lima_ctx_buff_pp_tex_desc,
                                  LIMA_CTX_BUFF_SUBMIT_PP) + offset;
      lima_update_tex_desc(ctx, sampler, texture, (void *)descs + offset, desc_size);
      offset += desc_size;
   }

   lima_dump_command_stream_print(
      descs, size, false, "add textures_desc at va %x\n",
      lima_ctx_buff_va(ctx, lima_ctx_buff_pp_tex_desc, 0));
}