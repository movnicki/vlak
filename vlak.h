#ifndef VLAK_H
#define VLAK_H

#define VLAK_VERSION "0.0.0"

//#define FT_STATIC_LIBRARY
#include<ft2build.h>
#include FT_FREETYPE_H

#include<string.h>
#include<stdlib.h>

#include<stdbool.h>
#include<stdint.h>
#include<math.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#if defined(__ANDROID__)
#include <android/log.h>
#include <android/native_activity.h>
#include<android/asset_manager.h>
#include<android/asset_manager_jni.h>
#include "android_native_app_glue.h"

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "native-min", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"native-min", __VA_ARGS__)

#elif defined(__unix__)
#include<X11/Xlib.h>
#elif defined(_WIN32)
#include<windows.h>
#endif

/* TTF Font */
typedef struct TTFFont {
    FT_Library library;
    FT_Face face;
    u16 size;
    u16 dpi_x;
    u16 dpi_y;
    u16 height;
    s16 descender;
    s16 ascender;
} TTFFont;

typedef struct TTFCacheData {
    FT_Bitmap bitmap;
    FT_Int bitmap_left;
    FT_Int bitmap_top;
    FT_Vector advance;
    bool has_cache;
} TTFCacheData;

TTFCacheData *global_ttf_cache_data;

typedef struct TTFCacheSize {
    u32 font_size;
    TTFCacheData ttf_cache_data[65535];
} TTFCacheSize;

TTFCacheSize global_ttf_cache_sizes[100];
u32 global_ttf_cache_sizes_count;

typedef struct VlakRect {
    s16 x;
    s16 y;
    s16 width;
    s16 height;
} VlakRect;

enum {
    VLAK_MOUSE_CLICK_LBUTTON_DOWN,
    VLAK_MOUSE_CLICK_LBUTTON_UP,
    VLAK_MOUSE_CLICK_RBUTTON_DOWN,
};

typedef struct VlakEvent VlakEvent;

typedef struct VlakWindow {
    VlakRect rect;
    u32 rect_id;
    VlakRect *regions;
    u32 regions_count;
    u32 *pixels;
    u32 pixels_count;

    struct VlakEvent {
        void (*render_func)(struct VlakWindow *window);
        void (*input_func)(struct VlakWindow *window, u8 key);
        void (*mouse_click_func)(struct VlakWindow *window, u16 x, u16 y, u8 type);
        void (*mouse_move_func)(struct VlakWindow *window, u16 x, u16 y);
        void (*mouse_wheel_func)(struct VlakWindow *window, u16 x, u16 y, s8 delta);
    } vlak_event;

#if defined(__ANDROID__)
    struct android_app *app;
#endif
#if defined(__unix__)
    XImage *image;
    Window window;
    Display *display;
    XIC input_context;
#elif defined(_WIN32)
    HDC device_context;
#endif
} VlakWindow;

/* Vlak Platform */
void vlak_init(u16 window_width, u16 window_height, char *font_path);
void vlak_main_loop(void);
void vlak_redraw(void);

/* Vlak */
void vlak_event_func_set(VlakEvent *event);

/* Vlak TTF Font */
void vlak_ttf_font_load(char *path, u32 size);
void vlak_ttf_font_size_set(u32 size);
u32 vlak_ttf_font_text_width_in_pixels(const u8 *text);

/* Vlak Region */
#define ALIGN_RIGHT(width, x) (width - x)
#define ALIGN_BOTTOM(height, y) (height - y)
void vlak_region_create(VlakRect *rect, s16 x, s16 y, s16 width, s16 height);
void vlak_region_sub_create(VlakRect *rect, VlakRect *rect_sub, s16 x, s16 y, s16 width, s16 height);
bool vlak_region_point_over(VlakRect *rect, s16 x, s16 y);

/* Vlak Render */
void vlak_render_window_background_clear(u32 color);
void vlak_render_text(VlakRect *rect, u16 x, u16 y, const u8 *text, u32 color);
void vlak_render_rectangle_fill(VlakRect *rect, s16 x, s16 y, s16 width, s16 height, u32 color);
void vlak_render_rectangle(VlakRect *rect, s16 x, s16 y, s16 width, s16 height, u32 color);
void vlak_render_line_horizontal(VlakRect *rect, s16 x, s16 y, s16 width, u32 color);
void vlak_render_line_vertical(VlakRect *rect, s16 x, s16 y, s16 height, u32 color);

/* Vlak Key */
bool vlak_key_modifier_pressed(u16 modifier);

#include"vlak.c"

#endif
