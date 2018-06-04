#include <pebble.h>
#include <pebble-fctx/fctx.h>
#include <pebble-fctx/ffont.h>

#define SETTINGS_KEY 1

typedef struct PulsarSettings {
    GColor BackgroundColor;
    GColor ForegroundColor;
} PulsarSettings;
static PulsarSettings settings;

Window *g_window;
Layer *g_layer;
FFont *g_font;
struct tm g_local_time;

void on_layer_update(Layer* layer, GContext* ctx) {
    GRect bounds = layer_get_bounds(layer);
    FPoint center = FPointI(bounds.size.w / 2, bounds.size.h / 2);
    
    char time_string[8]; // = "23:59"; // For Testing
    strftime(time_string, sizeof time_string, clock_is_24h_style() ? "%H:%M" : "%I:%M", &g_local_time);
    
    FContext fctx;
    fctx_init_context(&fctx, ctx);
    fctx_set_color_bias(&fctx, 0);
    fctx_set_fill_color(&fctx, settings.BackgroundColor);
    
    FPoint time_pos;
    time_pos.x = center.x + INT_TO_FIXED(0);
    time_pos.y = center.y + INT_TO_FIXED(0);
    
    fctx_begin_fill(&fctx);
    fctx_set_text_em_height(&fctx, g_font, 14);
    fctx_set_fill_color(&fctx, settings.ForegroundColor);
    fctx_set_pivot(&fctx, FPointZero);
    fctx_set_offset(&fctx, time_pos);
    fctx_set_rotation(&fctx, 0);
    fctx_draw_string(&fctx, time_string, g_font, GTextAlignmentCenter, FTextAnchorMiddle);
    fctx_end_fill(&fctx);
    
    fctx_deinit_context(&fctx);
}

void on_tick_timer(struct tm *tick_time, TimeUnits units_changed) {
    g_local_time = *tick_time;
    layer_mark_dirty(g_layer);
}

void save_settings() {
    persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
}

void on_inbox_received(DictionaryIterator *iter, void *context) {
    // Color Preferences
    Tuple *bg_color = dict_find(iter, MESSAGE_KEY_BackgroundColor);
    if (bg_color) {
        settings.BackgroundColor = GColorFromHEX(bg_color->value->int32);
    }
    
    Tuple *fg_color = dict_find(iter, MESSAGE_KEY_ForegroundColor);
    if (fg_color) {
        settings.ForegroundColor = GColorFromHEX(fg_color->value->int32);
    }
    
    window_set_background_color(g_window, settings.BackgroundColor);
    
    save_settings();
}

static void init() {
    // Settings
    if (persist_exists(SETTINGS_KEY)) {
        persist_read_data(SETTINGS_KEY, &settings, sizeof(PulsarSettings));
    } else {
        settings.BackgroundColor = GColorBlack;
        settings.ForegroundColor = GColorRed;
    }
    app_message_register_inbox_received(&on_inbox_received);
    app_message_open(128, 128);
    
    g_font = ffont_create_from_resource(RESOURCE_ID_FFONT_PULSAR);
    
    g_window = window_create();
    window_set_background_color(g_window, settings.BackgroundColor);
    window_stack_push(g_window, true);
    
    Layer* window_layer = window_get_root_layer(g_window);
    GRect window_frame = layer_get_frame(window_layer);
    
    g_layer = layer_create(window_frame);
    layer_set_update_proc(g_layer, &on_layer_update);
    layer_add_child(window_layer, g_layer);
    
    time_t now = time(NULL);
    g_local_time = *localtime(&now);
    tick_timer_service_subscribe(MINUTE_UNIT, &on_tick_timer);
}

static void deinit() {
    tick_timer_service_unsubscribe();
    window_destroy(g_window);
    layer_destroy(g_layer);
    ffont_destroy(g_font);
}

int main() {
    init();
    app_event_loop();
    deinit();
}
