VlakWindow vlak_window;
TTFFont vlak_ttf_font;

bool running = true;

/* Helper Function's */
u32 color_alpha_blend(u32 color0, u32 color1, u32 alpha)
{
    u32 alpha1 = 0xff - alpha;
    return (((alpha *((color0 >> 16) & 0xff) + alpha1 * ((color1 >> 16) & 0xff)) / 255) << 16) |
        (((alpha * ((color0 >> 8) & 0xff) + alpha1 * ((color1 >> 8) & 0xff)) / 255) << 8) |
        ((alpha * (color0 & 0xff) + alpha1 * (color1 & 0xff)) / 255);
}

u32 utf8_decode(const u8 *text, u32 *i)
{
    u32 result = 0;
    u8 c = text[*i];

    if (c < 0x80)
    {
        result = c;
    }
    else if ((c & 0xe0) == 0xc0) // 2-byte sequence
    {
        result = c & 0x1F;
        (*i)++;
        result = (result << 6) | (text[*i] & 0x3F);
    }
    else if ((c & 0xf0) == 0xe0) // 3-byte sequence
    {
        result = c & 0x0F;
        (*i)++;
        result = (result << 6) | (text[*i] & 0x3F);
        (*i)++;
        result = (result << 6) | (text[*i] & 0x3F);
    }
    else if ((c & 0xf8) == 0xf0) // 4-byte sequence
    {
        result = c & 0x07;
        (*i)++;
        result = (result << 6) | (text[*i] & 0x3F);
        (*i)++;
        result = (result << 6) | (text[*i] & 0x3F);
        (*i)++;
        result = (result << 6) | (text[*i] & 0x3F);
    }

    return result;
}

/* Vlak Key */
/*
bool vlak_key_modifier_pressed(u16 modifier)
{
    return (GetAsyncKeyState(modifier) & 0x8000);
}
*/

/* Render */
void vlak_render_line_vertical(VlakRect *vlak_rect,
                               s16 x, s16 y,
                               s16 line_width,
                               u32 color)
{
    u32 *pixels_start = NULL;
    u32 *pixels_end = NULL;

    if (vlak_rect->x == -1)
        return;

    if (y < 0)
    {
        line_width += y;
        y = 0;
        
        if (line_width < 0)
            return;
    }

    if (y >= vlak_rect->height)
        return;

    if (y + line_width > vlak_rect->height)
        line_width -= y + line_width - vlak_rect->height;

    if (x < 0)
        return;

    if (x >= vlak_rect->width)
        return;

    /* convert to winodw x, y */
    x += vlak_rect->x;
    y += vlak_rect->y;

    pixels_start = &(vlak_window.pixels)[y * (vlak_window.rect.width) + x];
    pixels_end = pixels_start + line_width * (vlak_window.rect.width);

    while (pixels_start != pixels_end)
    {
        *pixels_start = color_alpha_blend(color, *pixels_start, color >> 24);
        pixels_start += (vlak_window.rect.width);
    }
}

void vlak_render_line_horizontal(VlakRect *vlak_rect, s16 x, s16 y, s16 line_width, u32 color)
{
    u32 *pixels_start = NULL;
    u32 *pixels_end = NULL;

    if (vlak_rect->x == -1)
        return;

    if (x < 0)
    {
        line_width += x;
        x = 0;
        
        if (line_width < 0)
            return;
    }

    if (x >= vlak_rect->width)
        return;

    if (x + line_width > vlak_rect->width)
        line_width -= x + line_width - vlak_rect->width;

    if (y < 0)
        return;

    if (y >= vlak_rect->height)
        return;

    /* convert to winodw x, y */
    x += vlak_rect->x;
    y += vlak_rect->y;

    pixels_start = &(vlak_window.pixels)[y * (vlak_window.rect.width) + x];
    pixels_end = pixels_start + line_width;

    while (pixels_start != pixels_end)
    {
        *pixels_start = color_alpha_blend(color, *pixels_start, color >> 24);
        pixels_start++;
    }
}

void vlak_render_rectangle(VlakRect *vlak_rect,
                          s16 x, s16 y,
                          s16 width, s16 height,
                          u32 color)
{
    vlak_render_line_horizontal(vlak_rect, x, y, width, color);
    vlak_render_line_horizontal(vlak_rect, x, y + height - 1, width, color);

    vlak_render_line_vertical(vlak_rect, x, y, height, color);
    vlak_render_line_vertical(vlak_rect, x + width - 1, y, height, color);
}

void vlak_render_rectangle_fill(VlakRect *vlak_rect,
                                s16 x, s16 y,
                                s16 width, s16 height,
                                u32 color)
{
    u32 *pixels_start = NULL;
    u32 *pixels_end = NULL;

    if (vlak_rect->x == -1)
        return;

    if (x < 0)
    {
        width += x;
        x = 0;

        if (width < 0)
            return;
    }

    if (x >= vlak_rect->width)
        return;

    if (x + width > vlak_rect->width)
        width -= x + width - vlak_rect->width;

    if (y < 0)
    {
        height += y;
        y = 0;

        if (height < 0)
            return;
    }

    if (y >= vlak_rect->height)
        return;

    if (y + height > vlak_rect->height)
        height -= y + height - vlak_rect->height;


    /* convert to winodw x, y */
    x += vlak_rect->x;
    y += vlak_rect->y;

    pixels_start = &(vlak_window.pixels)[y * (vlak_window.rect.width) + x];
    pixels_end = pixels_start + (height - 1) * (vlak_window.rect.width) + width;

    //*pixels_end = 0xff000000;

    u32 i;
    while (pixels_start < pixels_end)
    {
        for (i = 0; i < width; i++)
        {
            pixels_start[i] = color_alpha_blend(color, pixels_start[i], color >> 24);
        }
        pixels_start += (vlak_window.rect.width);
    }


#if 0
    u32 pixels_index;
    u32 i;

    if (x >= vlak_rect->width)
        return;

    if (y >= vlak_rect->height)
        return;

    if (x + width > vlak_rect->width)
        width = vlak_rect->width - x;

    if (y + height > vlak_rect->height)
        height = vlak_rect->height - y;

    while (height--)
    {
        for (i = 0; i < width; i++)
        {
            pixels_index = (y + height) * (vlak_window.rect.width) + x + i;
            (vlak_window.pixels)[pixels_index] = color_alpha_blend(color & 0xffffff,
                                                            (vlak_window.pixels)[pixels_index],
                                                            color >> 24);
        }
    }
#endif
}

void vlak_render_text(VlakRect *vlak_rect,
                      u16 x, u16 y,
                      const u8 *text,
                      u32 color)
{
    FT_GlyphSlot glyph_slot;
    FT_ULong glyph_index;
    TTFCacheData *ttf_cache_data = NULL;
    u32 unicode_char;
    u32 i;
    u32 pixels_index;
    u16 pen_x = x + vlak_rect->x;
    u16 pen_y = y + vlak_rect->y;
    u16 rows, cols;
    u32 bitmap_alpha;
    u16 x_max = vlak_rect->width + vlak_rect->x;
    u16 y_max = vlak_rect->height + vlak_rect->y;

    if (x_max > (vlak_window.rect.width))
        x_max = (vlak_window.rect.width);

    if (y_max > (vlak_window.rect.height))
        y_max = (vlak_window.rect.height);

    if (pen_x >= x_max ||
        pen_y - y >= y_max)
        return;

    for (i = 0; text[i] != 0; i++)
    {
        unicode_char = utf8_decode(text, &i);
        ttf_cache_data = &global_ttf_cache_data[unicode_char];

        if (ttf_cache_data->has_cache == false)
        {
            glyph_slot = vlak_ttf_font.face->glyph;
            glyph_index = FT_Get_Char_Index(vlak_ttf_font.face, unicode_char);
            if (glyph_index == 0)
                glyph_index = FT_Get_Char_Index(vlak_ttf_font.face, L'�');

            FT_Load_Glyph(vlak_ttf_font.face, glyph_index, FT_LOAD_DEFAULT);
            FT_Render_Glyph(glyph_slot, FT_RENDER_MODE_NORMAL);

            ttf_cache_data->bitmap = glyph_slot->bitmap;
            ttf_cache_data->bitmap_left = glyph_slot->bitmap_left;
            ttf_cache_data->bitmap_top = glyph_slot->bitmap_top;
            ttf_cache_data->advance = glyph_slot->advance;

            ttf_cache_data->bitmap.buffer = malloc(sizeof(*ttf_cache_data->bitmap.buffer) * ttf_cache_data->bitmap.rows * ttf_cache_data->bitmap.width);
            memcpy(ttf_cache_data->bitmap.buffer, glyph_slot->bitmap.buffer, ttf_cache_data->bitmap.rows * ttf_cache_data->bitmap.width);

            ttf_cache_data->has_cache = true;
        }

        pen_x += ttf_cache_data->bitmap_left;
		
        u16 rows_offset = 0;
        if (ttf_cache_data->bitmap_top > y)
        {
            rows_offset = ttf_cache_data->bitmap_top - y;
        }

        pen_y = vlak_rect->y + y - ttf_cache_data->bitmap_top;

        // (vlak_window.pixels)[pen_y * (vlak_window.rect.width) + pen_x] = 0xff000000;

        /* test */
        u32 bitmap_pitch = ttf_cache_data->bitmap.pitch;
        if (pen_x + bitmap_pitch > x_max)
        {
            if (pen_x > x_max)
                return;
            bitmap_pitch = x_max - pen_x;
        }

        u32 bitmap_rows = ttf_cache_data->bitmap.rows;
        if (pen_y + bitmap_rows > y_max)
        {
            bitmap_rows = y_max - pen_y;
        }
		
		if (!(pen_y > y_max))
		{
			for (rows = rows_offset; rows < bitmap_rows; rows++)
			{
				for (cols = 0; cols < bitmap_pitch; cols++)
				{
					bitmap_alpha = ttf_cache_data->bitmap.buffer[rows * ttf_cache_data->bitmap.pitch + cols];
					pixels_index = (pen_y + rows) * (vlak_window.rect.width) + (pen_x + cols);
                    (vlak_window.pixels)[pixels_index] = color_alpha_blend(color_alpha_blend(color,
                                                                                          (vlak_window.pixels)[pixels_index],
                                                                                          color >> 24),
                                                                        (vlak_window.pixels)[pixels_index],
                                                                        bitmap_alpha);
                }
			}
		}	
        pen_x -= ttf_cache_data->bitmap_left;
        pen_x += ttf_cache_data->advance.x >> 6;
    }
}

void vlak_render_window_background_clear(u32 color)
{
    u32 i;

    for (i = 0; i < vlak_window.rect.width * vlak_window.rect.height; i++)
        vlak_window.pixels[i] = color;
}

/* Vlak Region */
bool vlak_region_point_over(VlakRect *rect, s16 x, s16 y)
{
    if (x >= rect->x && x < rect->x + rect->width &&
        y >= rect->y && y < rect->y + rect->height)
        return true;

    return false;
}

void vlak_region_sub_create(VlakRect *rect, VlakRect *rect_sub, s16 x, s16 y, s16 width, s16 height)
{
    if (x < 0)
    {
        width += x;
        x = 0;

    }

    if (x + width > rect->width)
        width -= x + width - rect->width;

    if (y < 0)
    {
        height += y;
        y = 0;
    }

    if (y + height > rect->height)
        height -= y + height - rect->height;

    if (x < 0 ||
        y < 0 ||
        width < 0 ||
        height < 0)
    {
        x = -1;
        y = -1;
        width = -1;
        height = -1;
    }

    rect_sub->x = x + rect->x;
    rect_sub->y = y + rect->y;
    rect_sub->width = width;
    rect_sub->height = height;
}

void vlak_region_create(VlakRect *rect, s16 x, s16 y, s16 width, s16 height)
{
    if (x + width > vlak_window.rect.width)
        width -= x + width - vlak_window.rect.width;

    if (y + height > vlak_window.rect.height)
        height -= y + height - vlak_window.rect.height;

    if (x < 0 ||
        y < 0 ||
        width < 0 ||
        height < 0)
    {
        x = -1;
        y = -1;
        width = -1;
        height = -1;
    }

    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
}

/* TTF Font */
u32 vlak_ttf_font_text_width_in_pixels(const u8 *text)
{
    u32 text_length;
    for (text_length = 0; text[text_length] != 0; text_length++) { }
    return (vlak_ttf_font.face->size->metrics.max_advance >> 6) * text_length;
}

void vlak_ttf_font_size_set(u32 size)
{
    FT_Set_Char_Size(vlak_ttf_font.face, 0, size * 64, vlak_ttf_font.dpi_x, vlak_ttf_font.dpi_y);
vlak_ttf_font.size = size;
    vlak_ttf_font.descender = (vlak_ttf_font.face->size->metrics.descender >> 6);
    vlak_ttf_font.ascender = (vlak_ttf_font.face->size->metrics.ascender >> 6);
    vlak_ttf_font.height = vlak_ttf_font.ascender - vlak_ttf_font.descender;

    u32 i;
    for (i = 0; i < global_ttf_cache_sizes_count; i++)
    {
        if (size == global_ttf_cache_sizes[i].font_size)
        {
            global_ttf_cache_data = global_ttf_cache_sizes[i].ttf_cache_data;
            return;
        }

        if (size < global_ttf_cache_sizes[i].font_size)
            break;
    }

    global_ttf_cache_sizes[i].font_size = size;
    global_ttf_cache_data = global_ttf_cache_sizes[i].ttf_cache_data;

    memset(global_ttf_cache_data, 0, 256 * sizeof(*global_ttf_cache_data));
}

void vlak_ttf_font_load(char *path, u32 size)
{
    FT_Error error;
    FT_Init_FreeType(&vlak_ttf_font.library);

#if defined(__ANDROID__)
    AAssetManager *asset_manager = vlak_window.app->activity->assetManager;
    AAsset *asset = AAssetManager_open(asset_manager, "fonts/LiberationMono-Regular.ttf", AASSET_MODE_BUFFER);

    if (!asset) {
        __android_log_print(ANDROID_LOG_ERROR, "EATSYNC", "Font-Asset nicht gefunden!");
        return;
    }


    size_t asset_size = AAsset_getLength(asset);
    const void *buffer = AAsset_getBuffer(asset);


    error = FT_New_Memory_Face(vlak_ttf_font.library, buffer, asset_size, 0, &vlak_ttf_font.face);
#else
    error = FT_New_Face(vlak_ttf_font.library, path, 0, &vlak_ttf_font.face);
#endif

    if (error)
    {
        fprintf(stderr, "Can't load Font: %s\n", path);
        exit(1);
    }

    vlak_ttf_font_size_set(size);
}

/* Vlak */
void vlak_event_func_set(VlakEvent *vlak_event)
{
    if (vlak_event == NULL)
        return;

    if (vlak_event->render_func != NULL)
        vlak_window.vlak_event.render_func = vlak_event->render_func;
    if (vlak_event->input_func != NULL)
        vlak_window.vlak_event.input_func = vlak_event->input_func;
    if (vlak_event->mouse_click_func != NULL)
        vlak_window.vlak_event.mouse_click_func = vlak_event->mouse_click_func;
    if (vlak_event->mouse_move_func != NULL)
        vlak_window.vlak_event.mouse_move_func = vlak_event->mouse_move_func;
    if (vlak_event->mouse_wheel_func != NULL)
        vlak_window.vlak_event.mouse_wheel_func = vlak_event->mouse_wheel_func;
}

VlakRect region0;
VlakRect region1;
VlakRect region2;
VlakRect region3;

u16 clicked_x = 0, clicked_y = 0;

void vlak_render_default_func(VlakWindow *window)
{
    vlak_render_window_background_clear(0xffffffff);

    vlak_region_create(&region0, 20, 20, window->rect.width - 2 * 20, window->rect.height - 2 * 20);
    vlak_region_sub_create(&region0, &region1, 0, 0, 800, 800); 
    vlak_region_sub_create(&region1, &region2, 0, 0, 500, 500);
    vlak_region_sub_create(&region2, &region3, ALIGN_RIGHT(region2.width, 400), 30, 150, 150);

    vlak_render_rectangle(&region0,
                          0, 0,
                          region0.width,
                          region0.height,
                          0xff000000);

    vlak_render_rectangle(&region1,
                          0, 0,
                          region1.width,
                          region1.height,
                          0xff000000);

    vlak_render_rectangle(&region2,
                          0, 0,
                          region2.width, region2.height,
                          0xff000000);

    vlak_render_rectangle(&region3,
                          0, 0,
                          region3.width, region3.height,
                          0xff0000f0);

    vlak_render_rectangle(&window->rect,
                          400, 300,
                          100, 100,
                          0xffff0000);

    /* Clicked */
    vlak_render_line_horizontal(&window->rect,
                                clicked_x, clicked_y,
                                1,
                                0xffff0000);

    vlak_render_text(&window->rect, 50, 100, "Das ist ein Test", 0xff000000);
}

void vlak_input_default_func(VlakWindow *window, u8 key)
{
}

void vlak_mouse_click_default_func(VlakWindow *window, u16 x, u16 y, u8 type)
{
    clicked_x = x;
    clicked_y = y;
    if (type == VLAK_MOUSE_CLICK_LBUTTON_DOWN)
    {
        if (vlak_region_point_over(&region3, x, y))
            printf("Clicked on Region 3\n");
    }

    vlak_redraw();
}

void vlak_mouse_move_default_func(VlakWindow *window, u16 x, u16 y)
{
}

void vlak_wheel_default_func(VlakWindow *window, u16 x, u16 y, s8 delta)
{
}

/* Vlak Platform */
#if defined(__ANDROID__)
void vlak_init(u16 width, u16 height)
{
    vlak_window.rect.x = 0;
    vlak_window.rect.y = 0;
    vlak_window.rect.width = width;
    vlak_window.rect.height = height;

    vlak_window.vlak_event.render_func = vlak_render_default_func;
    vlak_window.vlak_event.input_func = vlak_input_default_func;
    vlak_window.vlak_event.mouse_click_func = vlak_mouse_click_default_func;
    vlak_window.vlak_event.mouse_move_func = vlak_mouse_move_default_func;

    //vlak_ttf_font_load("LiberationMono-Regular.ttf", 20);
}

void vlak_redraw(void)
{
    ANativeWindow *win;
    win = vlak_window.app->window;

    // Wunschformat setzen (RGBA_8888 ist am unkompliziertesten)
    ANativeWindow_setBuffersGeometry(win, 0, 0, WINDOW_FORMAT_RGBX_8888);

    ANativeWindow_Buffer buf;
    if (ANativeWindow_lock(win, &buf, NULL) != 0) {
        LOGE("ANativeWindow_lock failed");
        return;
    }
    const int w = buf.width;
    const int h = buf.height;
    const int stride = buf.stride; // Achtung: kann > width sein

    vlak_window.rect.x = 0;
    vlak_window.rect.y = 0;
    vlak_window.rect.width = w;
    vlak_window.rect.height = h;

    vlak_window.pixels_count = buf.width * buf.height;
    vlak_window.pixels = malloc(vlak_window.pixels_count * sizeof(*vlak_window.pixels));

    vlak_window.vlak_event.render_func(&vlak_window);

#if 1
                                   //
    // Hintergrund schwarz
    /*
    uint32_t* pixels = (uint32_t*)buf.bits;
    for (int y = 0; y < h; ++y) {
        uint32_t* row = pixels + y * stride;
        for (int x = 0; x < w; ++x) row[x] = 0xff000000; // AARRGGBB -> schwarz
    }
    */

    u32 *dest = (u32 *)buf.bits;

    int y;
    for (y = 0; y < h; ++y)
    {
        memcpy(dest + y * buf.stride, vlak_window.pixels + y * w, w * sizeof(*vlak_window.pixels));
    }

    // Rotes Rechteck (voll deckend)
    /*
    for (int y = y0; y < y0 + rh; ++y) {
        uint32_t* row = pixels + y * stride;
        for (int x = x0; x < x0 + rw; ++x) row[x] = 0xFFFFffff; // AARRGGBB -> rot
    }
    */
#endif

    ANativeWindow_unlockAndPost(win);
}

static void handle_cmd(struct android_app* app, int32_t cmd) {
    if (cmd == APP_CMD_INIT_WINDOW && app->window) {
        LOGI("Window ready.");
        vlak_redraw();
    }
}

static int32_t handle_input(struct android_app* app, AInputEvent* event) {
        if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int32_t action = AMotionEvent_getAction(event);
        int32_t masked = action & AMOTION_EVENT_ACTION_MASK;

        if (masked == AMOTION_EVENT_ACTION_UP) {
            float x = AMotionEvent_getX(event, 0);
            float y = AMotionEvent_getY(event, 0);

            // Das ist dein "Click" an Position (x, y)
            vlak_window.vlak_event.mouse_click_func(&vlak_window, x, y, VLAK_MOUSE_CLICK_LBUTTON_DOWN);

        }

        return 1;
    }

    return 0;
}

void vlak_main_loop(void)
{
    vlak_window.app->onAppCmd = handle_cmd;
    vlak_window.app->onInputEvent = handle_input;
    // vlak_window.app->onInputEvent = handle_input;
    LOGI("Hello from C!");
    int ident, events;
    struct android_poll_source* source;

    for (;;) {
        // -1 = blockierend warten, bis irgendwas passiert
        while ((ident = ALooper_pollOnce(-1, NULL, &events, (void**)&source)) >= 0) {
            if (source) source->process(vlak_window.app, source);
            if (vlak_window.app->destroyRequested) return;
            source = NULL; // sauber für den nächsten Durchlauf
        }
        // ident < 0 bedeutet WAKE/CALLBACK/ERROR; einfach nächste Runde
    }
}

/* int main as entry point */
int android_entry(void);
#define main(void) android_entry(void)
void android_main(struct android_app *app) { vlak_window.app = app; android_entry(); }

#elif defined(__unix__)
void vlak_init(u16 width, u16 height, char *font_path)
{
    if (font_path == NULL)
    {
        fprintf(stderr,
                "error: no font path specified, use vlak_init() or vlak_ttf_font_load()\n");
        getchar();
        exit(1);
    }
    else
    {
        vlak_ttf_font_load(font_path, 16);
    }
    vlak_window.rect.x = 0;
    vlak_window.rect.y = 0;
    vlak_window.rect.width = width;
    vlak_window.rect.height = height;

    vlak_window.vlak_event.render_func = vlak_render_default_func;
    vlak_window.vlak_event.input_func = vlak_input_default_func;
    vlak_window.vlak_event.mouse_click_func = vlak_mouse_click_default_func;
    vlak_window.vlak_event.mouse_move_func = vlak_mouse_move_default_func;


    vlak_window.display = XOpenDisplay(NULL);
    vlak_window.window = XCreateWindow(vlak_window.display, DefaultRootWindow(vlak_window.display), 0, 0, vlak_window.rect.width, vlak_window.rect.height, 0, CopyFromParent, InputOutput, CopyFromParent, 0, 0);

    Atom protocol_wm_delete_window = XInternAtom(vlak_window.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(vlak_window.display, vlak_window.window, &protocol_wm_delete_window, 1);

    XSelectInput(vlak_window.display, vlak_window.window, StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

    XMapWindow(vlak_window.display, vlak_window.window);

    vlak_window.image = XCreateImage(vlak_window.display, DefaultVisual(vlak_window.display, 0), 24, ZPixmap, 0, NULL, 0, 0, 32,  0);

    XIM xim = XOpenIM(vlak_window.display,
                      NULL,
                      NULL,
                      NULL);

    vlak_window.input_context = XCreateIC(xim,
                                       XNInputStyle,
                                       XIMPreeditNothing | XIMStatusNothing,
                                       NULL);
}

void vlak_redraw(void)
{
    vlak_window.vlak_event.render_func(&vlak_window);
    XPutImage(vlak_window.display, vlak_window.window, DefaultGC(vlak_window.display, 0), vlak_window.image, 0, 0, 0, 0, vlak_window.rect.width, vlak_window.rect.height);
}

void vlak_main_loop(void)
{
    XEvent event;
    while (running)
    {
        XNextEvent(vlak_window.display, &event);
        switch (event.type)
        {
            case Expose:
            {
                printf("Expose\n");
                vlak_redraw();
            } break;

            case ButtonPress:
            {
                switch (event.xbutton.button)
                {
                    case Button1:
                    {
                        vlak_window.vlak_event.mouse_click_func(&vlak_window,
                                                             event.xbutton.x,
                                                             event.xbutton.y,
                                                             VLAK_MOUSE_CLICK_LBUTTON_DOWN);
                    } break;
                    case Button4:
                    {
                        vlak_window.vlak_event.mouse_wheel_func(&vlak_window,
                                                             event.xbutton.x,
                                                             event.xbutton.y,
                                                             1);
                    } break;
                    case Button5:
                    {
                        vlak_window.vlak_event.mouse_wheel_func(&vlak_window,
                                                             event.xbutton.x,
                                                             event.xbutton.y,
                                                             -1);
                    } break;
                }
            } break;

            case ButtonRelease:
            {
                vlak_window.vlak_event.mouse_click_func(&vlak_window, event.xbutton.x, event.xbutton.y, VLAK_MOUSE_CLICK_LBUTTON_UP);
            } break;

            case MotionNotify:
            {
                vlak_window.vlak_event.mouse_move_func(&vlak_window, event.xbutton.x, event.xbutton.y);
            } break;

            case ConfigureNotify:
            {
                printf("ConfigureNotify\n");

                vlak_window.rect.width = event.xconfigure.width;
                vlak_window.rect.height = event.xconfigure.height;
                printf("Width: %d Height %d\n",
                       vlak_window.rect.width,
                       vlak_window.rect.height);

                if (vlak_window.pixels)
                    free(vlak_window.pixels);

                vlak_window.pixels_count = vlak_window.rect.width * vlak_window.rect.height;
                vlak_window.pixels = malloc(vlak_window.pixels_count * sizeof(*vlak_window.pixels));

                vlak_window.image->width = vlak_window.rect.width;
                vlak_window.image->height = vlak_window.rect.height;
                vlak_window.image->bytes_per_line = vlak_window.rect.width * sizeof(*vlak_window.pixels);
                vlak_window.image->data = (char *)vlak_window.pixels;
            } break;

            case KeyPress:
            {
                char buffer[255];
                wchar_t wbuffer[255];
                KeySym key;
                printf("KeyPress\n");
                Status status;
                int buffer_bytes = XmbLookupString(vlak_window.input_context,
                                                   &event.xkey,
                                                   buffer,
                                                   255,
                                                   &key,
                                                   &status);
                int wbuffer_bytes = XwcLookupString(vlak_window.input_context,
                                                    &event.xkey,
                                                    wbuffer,
                                                    255,
                                                    &key,
                                                    &status);
                printf("buffer: %s\n", buffer);
                printf("keysym: %ld\n", key);
                printf("keysym string: %s\n", XKeysymToString(key));
                printf("bytes: %d\n", buffer_bytes);
                puts("---");
                printf("wbuffer: %s\n", "öde");
                printf("keysym: %ld\n", key);
                printf("keysym string: %s\n", XKeysymToString(key));
                printf("bytes: %d\n", wbuffer_bytes);

                vlak_window.vlak_event.input_func(&vlak_window, buffer[0]);
                vlak_redraw();
            } break;

            case KeyRelease:
            {
                printf("KeyReleased\n");
            } break;

            case ClientMessage:
            {
                printf("ClientMessage WM_DELETE_WINDOW\n");

                running = false;
            } break;

            default:
            {
                printf("Event Type: %d\n",
                       event.type);
            } break;
        }
    }
}
#elif defined(_WIN32)
void vlak_redraw(void)
{

    vlak_window.vlak_event.render_func(&vlak_window);

    BITMAPINFO bitmapinfo;
    bitmapinfo.bmiHeader.biSize = sizeof(bitmapinfo.bmiHeader);
    bitmapinfo.bmiHeader.biWidth = vlak_window.rect.width;
    bitmapinfo.bmiHeader.biHeight = -vlak_window.rect.height;
    bitmapinfo.bmiHeader.biPlanes = 1;
    bitmapinfo.bmiHeader.biBitCount = 32;
    bitmapinfo.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(vlak_window.device_context,
                  0, 0,
#ifdef SCALE
                  vlak_window.rect.width * 8,
                  vlak_window.rect.height * 8,
#else
                  vlak_window.rect.width,
                  vlak_window.rect.height,
#endif
                  0, 0,
                  vlak_window.rect.width,
                  vlak_window.rect.height,
                  vlak_window.pixels,
                  &bitmapinfo,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

LRESULT CALLBACK main_wndproc(HWND window, UINT message, WPARAM w_param, LPARAM l_param)
{
    LRESULT result = 0;
    switch (message)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
            RECT window_size;
            GetClientRect(window, &window_size);
            
            vlak_window.rect.width = window_size.right - window_size.left;
            vlak_window.rect.height = window_size.bottom - window_size.top;

#if SCALE
            vlak_window.rect.width /= 8;
            vlak_window.rect.height /= 8;
#endif


            if (vlak_window.pixels)
                free(vlak_window.pixels);
            
            vlak_window.pixels_count = vlak_window.rect.width * vlak_window.rect.height;
            vlak_window.pixels = malloc(vlak_window.pixels_count * sizeof(vlak_window.pixels));
        } break;
        
        case WM_PAINT:
        {
            PAINTSTRUCT paint;
            vlak_window.device_context = BeginPaint(window, &paint);
            OutputDebugStringA("WM_PAINT\n");
            vlak_redraw();
            EndPaint(window, &paint);
        } break;

        case WM_CHAR:
        {
            vlak_window.vlak_event.input_func(&vlak_window, (u8)w_param);
        } break;

        case WM_SYSKEYDOWN:
        {
            int a = 0;
        } break;

        case WM_KEYDOWN:
        {
            int b = 0;

            if (w_param == 0x10 || w_param == 0x11)
                vlak_window.vlak_event.input_func(&vlak_window, (u8)w_param);
        } break;

        case WM_KEYUP:
        {
        } break;
        
        case WM_DESTROY:
        {
            printf("wm_destroy\n");
            OutputDebugStringA("WM_DESTROY\n");
            running = false;
        } break;
        
        case WM_CLOSE:
        {
            printf("wm_close\n");
            OutputDebugStringA("WM_CLOSE\n");
            running = false;
        } break;

#include<windowsx.h>
        case WM_MOUSEWHEEL:
        {
            int fwKeys = GET_KEYSTATE_WPARAM(w_param);
            int zDelta = GET_WHEEL_DELTA_WPARAM(w_param);
            int xPos = GET_X_LPARAM(l_param); 
            int yPos = GET_Y_LPARAM(l_param);

            POINT p = { xPos, yPos };
            ScreenToClient(window, &p);

            vlak_window.vlak_event.mouse_wheel_func(&vlak_window, p.x, p.y, zDelta);
        } break;

        case WM_LBUTTONDOWN:
        {
            int x = GET_X_LPARAM(l_param); 
            int y = GET_Y_LPARAM(l_param);

            vlak_window.vlak_event.mouse_click_func(&vlak_window, x, y, VLAK_MOUSE_CLICK_LBUTTON_DOWN);
        } break;

        case WM_RBUTTONDOWN:
        {
            int x = GET_X_LPARAM(l_param); 
            int y = GET_Y_LPARAM(l_param);

            vlak_window.vlak_event.mouse_click_func(&vlak_window, x, y, VLAK_MOUSE_CLICK_RBUTTON_DOWN);
        } break;

        case WM_LBUTTONUP:
        {
            int x = GET_X_LPARAM(l_param); 
            int y = GET_Y_LPARAM(l_param);

            vlak_window.vlak_event.mouse_click_func(&vlak_window, x, y, VLAK_MOUSE_CLICK_LBUTTON_UP);
        } break;

        case WM_MOUSEMOVE:
        {
            int x = GET_X_LPARAM(l_param); 
            int y = GET_Y_LPARAM(l_param);

            vlak_window.vlak_event.mouse_move_func(&vlak_window, x, y);
        } break;
        
        default:
        {
            result = DefWindowProc(window, message, w_param, l_param);
        } break;
    }
    return result;
}

void vlak_main_loop(void)
{
    MSG message = { 0 };
    while (running)
    {
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE) > 0)
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
}

void vlak_init(u16 window_width, u16 window_height, char *font_path)
{
    if (font_path == NULL)
    {
        fprintf(stderr,
                "error: no font path specified, use vlak_init() or vlak_ttf_font_load()\n");
        getchar();
        exit(1);
    }
    else
    {
        vlak_ttf_font_load(font_path, 16);
    }

    vlak_window.rect.x = 0;
    vlak_window.rect.y = 0;
    vlak_window.rect.width = window_width;
    vlak_window.rect.height = window_height;

    vlak_window.vlak_event.render_func = vlak_render_default_func;
    vlak_window.vlak_event.input_func = vlak_input_default_func;
    vlak_window.vlak_event.mouse_click_func = vlak_mouse_click_default_func;
    vlak_window.vlak_event.mouse_move_func = vlak_mouse_move_default_func;
    vlak_window.vlak_event.mouse_wheel_func = vlak_wheel_default_func;

    HINSTANCE instance = GetModuleHandle(NULL);
    WNDCLASS window_class = { 0 };
    window_class.lpszClassName = "VlakApplication";
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = main_wndproc;
    window_class.hInstance = instance;
    window_class.hCursor = NULL;
    window_class.hIcon = LoadImage(NULL, "data/icon0.ico", IMAGE_ICON, 32, 32, LR_LOADFROMFILE);
#if 0
    window_class.hIcon = LoadIcon(NULL, IDI_ERROR); /* load a icon todo(mb) */
#endif
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    ATOM register_class_atom = RegisterClass(&window_class);
    
    if (!register_class_atom)
    {
        OutputDebugStringA("Failed to Register Class\n");
        return;
    }
    
    HWND window = CreateWindowExA(0,
                                  window_class.lpszClassName,
                                  "sPoor ~ movnicki",
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  vlak_window.rect.width,
                                  vlak_window.rect.height,
                                  0,
                                  0,
                                  instance,
                                  0);

    //DwmSetWindowAttribute(window, 19, &(BOOL){ TRUE }, sizeof(BOOL));
}
#endif
