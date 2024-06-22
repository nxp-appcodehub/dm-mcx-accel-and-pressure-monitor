/*
 * Copyright 2024 NXP
 * NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
 * accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
 * activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
 * comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
 * terms, then you may not retain, install, activate or otherwise use the software.
 */
/*******************************************************************************
 * Size: 16 px
 * Bpp: 4
 * Opts: --user-data-dir=C:\Users\nxf94210\AppData\Roaming\gui-guider --app-path=C:\nxp\GUI-Guider-1.7.1-GA\resources\app.asar --no-sandbox --no-zygote --lang=en-US --device-scale-factor=1.25 --num-raster-threads=4 --enable-main-frame-before-activation --renderer-client-id=5 --time-ticks-at-unix-epoch=-1712593924485672 --launch-time-ticks=269027445595 --mojo-platform-channel-handle=2912 --field-trial-handle=1752,i,8796389826535415046,9056103041613645347,131072 --disable-features=SpareRendererForSitePerProcess,WinRetrieveSuggestionsOnlyOnDemand /prefetch:1
 ******************************************************************************/

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
#include "lvgl.h"
#else
#include "lvgl.h"
#endif

#ifndef LV_CUSTOMER_FONT_MONTSERRATMEDIUM_16
#define LV_CUSTOMER_FONT_MONTSERRATMEDIUM_16 1
#endif

#if LV_CUSTOMER_FONT_MONTSERRATMEDIUM_16

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0065 "e" */
    0x0, 0x3b, 0xfe, 0xa2, 0x0, 0x4, 0xfc, 0x67,
    0xee, 0x20, 0xe, 0xc0, 0x0, 0x1e, 0xa0, 0x3f,
    0x50, 0x0, 0x7, 0xf0, 0x4f, 0xff, 0xff, 0xff,
    0xf1, 0x3f, 0x72, 0x22, 0x22, 0x20, 0xe, 0xc0,
    0x0, 0x2, 0x0, 0x4, 0xfd, 0x87, 0xaf, 0x50,
    0x0, 0x3a, 0xef, 0xd6, 0x0,

    /* U+0073 "s" */
    0x2, 0xae, 0xfd, 0x91, 0x1e, 0xd7, 0x69, 0xd0,
    0x4f, 0x30, 0x0, 0x0, 0x2f, 0xb4, 0x10, 0x0,
    0x6, 0xef, 0xfd, 0x60, 0x0, 0x1, 0x5b, 0xf5,
    0x1, 0x0, 0x0, 0xf7, 0x5f, 0xa7, 0x6b, 0xf3,
    0x19, 0xdf, 0xec, 0x40,

    /* U+0074 "t" */
    0x5, 0xf3, 0x0, 0x0, 0x5f, 0x30, 0x0, 0xcf,
    0xff, 0xfa, 0x4, 0x8f, 0x75, 0x30, 0x5, 0xf3,
    0x0, 0x0, 0x5f, 0x30, 0x0, 0x5, 0xf3, 0x0,
    0x0, 0x5f, 0x30, 0x0, 0x4, 0xf4, 0x0, 0x0,
    0x1f, 0xc6, 0x80, 0x0, 0x5d, 0xfc, 0x10
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 157, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 45, .adv_w = 128, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 81, .adv_w = 106, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_0[] = {
    0x0, 0xe, 0xf
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 101, .range_length = 16, .glyph_id_start = 1,
        .unicode_list = unicode_list_0, .glyph_id_ofs_list = NULL, .list_length = 3, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 1
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 1, 0, 2
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    -5, -3
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 1,
    .right_class_cnt     = 2,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 1,
    .bpp = 4,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t lv_customer_font_montserratMedium_16 = {
#else
lv_font_t lv_customer_font_montserratMedium_16 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 16,          /*The maximum line height required by the font  default: (f.src.ascent - f.src.descent)*/
    .base_line = 2,                          /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if LV_CUSTOMER_FONT_MONTSERRATMEDIUM_16*/

