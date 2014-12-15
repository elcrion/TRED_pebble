 #include <pebble.h>

#define KEY_COMPL 5
#define KEY_DAYS  6
	
ActionBarLayer *action_bar;
static Window *s_main_window;
static TextLayer *s_output_layer;
static TextLayer *s_detail_layer;
static BitmapLayer *icon_layer;
static BitmapLayer *conn_icon_layer;
static TextLayer *battery_text_layer;
static TextLayer *conn_text_layer;

GBitmap *future_bitmap;
Layer *window_layer;
GFont custom_font ;
GFont custom_font2 ;
GFont custom_font_bat;
GFont custom_font_conn;
TextLayer *text_date_layer;
TextLayer *text_time_layer;
Layer *line_layer;
static GBitmap *icon_bitmap = NULL;
static GBitmap *wicon_bitmap = NULL;

enum BatteryKey {
  BATTERY_LOW_KEY = 0x0,         // TUPLE_INT
  BATTERY_FULL_KEY = 0x1,  // TUPLE_CSTRING
  BATTERY_MED_KEY = 0x2,         // TUPLE_CSTRING
	BATTERY_CHARGING_KEY = 0x3,
		BATTERY_CHARGED_KEY = 0x4
};

static const uint32_t Battery_ICONS[] = {
	 RESOURCE_ID_BATTERY_LOW, 
  RESOURCE_ID_BATTERY_FULL, 
 
  RESOURCE_ID_BATTERY_MED, 
  RESOURCE_ID_BATTERY_CHARGING, 
	RESOURCE_ID_BATTERY_CHARGED
};



static void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";
  if (icon_bitmap) {
        gbitmap_destroy(icon_bitmap);
      }
	
  if (charge_state.is_charging) {
	  
	  if(charge_state.charge_percent ==100){
		  icon_bitmap = gbitmap_create_with_resource(Battery_ICONS[BATTERY_CHARGED_KEY]);
		  
	  }else{
 	  icon_bitmap = gbitmap_create_with_resource(Battery_ICONS[BATTERY_CHARGING_KEY]);
     
	  }
	   bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
	   snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);  
	  
  } else {
	    if(charge_state.charge_percent <=100 && charge_state.charge_percent >75 ){
	  
	   icon_bitmap = gbitmap_create_with_resource(Battery_ICONS[BATTERY_FULL_KEY]);
		}else if(charge_state.charge_percent <=75 && charge_state.charge_percent >35 )	{
			
		 icon_bitmap = gbitmap_create_with_resource(Battery_ICONS[BATTERY_MED_KEY]);	
		}else if( charge_state.charge_percent <=35 ){
				
			 icon_bitmap = gbitmap_create_with_resource(Battery_ICONS[BATTERY_LOW_KEY]);
		}
		
      bitmap_layer_set_bitmap(icon_layer, icon_bitmap);
    snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
  }
  text_layer_set_text(battery_text_layer, battery_text);
}



void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char time_text[] = "00:00 am";
  static char date_text[] = "Xxxxxxxxx 00";

  char *time_format;

  if (!tick_time) {
    time_t now = time(NULL);
    tick_time = localtime(&now);
  }

  // TODO: Only update the date when it's changed.
  strftime(date_text, sizeof(date_text), "%B %e", tick_time);
  text_layer_set_text(text_date_layer, date_text);


    time_format = "%I:%M%p";
  
	

  strftime(time_text, sizeof(time_text), time_format, tick_time);

  handle_battery(battery_state_service_peek());

  text_layer_set_text(text_time_layer, time_text);
}


void line_layer_update_callback(Layer *layer, GContext* ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
	
	
}

static void handle_bluetooth(bool connected) {
	 //static char conn_text[] = "No connection";
	
	 if (wicon_bitmap) {
        gbitmap_destroy(wicon_bitmap);
      } 
	
	if(connected){
		
		 wicon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_SIGNAL_IMG );
     
	 	  bitmap_layer_set_bitmap(conn_icon_layer, wicon_bitmap);
		 //snprintf(conn_text, sizeof(conn_text), "Connected");
	
	}else{
		   vibes_long_pulse();
		 wicon_bitmap = gbitmap_create_with_resource(RESOURCE_ID_NO_CONNECTION_IMG  );
     
	 	  bitmap_layer_set_bitmap(conn_icon_layer, wicon_bitmap);
	//	snprintf(conn_text, sizeof(conn_text), "No connection");
		
	}
		//text_layer_set_text(conn_text_layer, conn_text);
}



void showmain(){
	
	
		window_set_background_color(s_main_window, GColorBlack);
 layer_add_child(window_layer, bitmap_layer_get_layer(icon_layer));
 layer_add_child(window_layer, bitmap_layer_get_layer(conn_icon_layer));
	
	
	
	
	
	  text_layer_set_text_color(battery_text_layer, GColorWhite);
  text_layer_set_background_color(battery_text_layer, GColorClear);
  text_layer_set_font(battery_text_layer, custom_font_bat);
  text_layer_set_text_alignment(battery_text_layer, GTextAlignmentCenter);
 layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));		
	
	  text_layer_set_text_color(conn_text_layer, GColorWhite);
  text_layer_set_background_color(conn_text_layer, GColorClear);
  text_layer_set_font(conn_text_layer, custom_font_conn);
  text_layer_set_text_alignment(conn_text_layer, GTextAlignmentCenter);
 layer_add_child(window_layer, text_layer_get_layer(conn_text_layer));
	
	
	
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  text_time_layer = text_layer_create(GRect(7, 92, 144-7, 168-92));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, custom_font2);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  GRect line_frame = GRect(8, 97, 139, 2);
  line_layer = layer_create(line_frame);
  layer_set_update_proc(line_layer, line_layer_update_callback);
  layer_add_child(window_layer, line_layer);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  handle_minute_tick(NULL, MINUTE_UNIT);
	 battery_state_service_subscribe(&handle_battery);
		handle_bluetooth(bluetooth_connection_service_peek());
	 bluetooth_connection_service_subscribe(&handle_bluetooth);

		 layer_set_hidden(bitmap_layer_get_layer(icon_layer), false);
  	   layer_set_hidden(bitmap_layer_get_layer(conn_icon_layer), false);
      
	layer_set_hidden(text_layer_get_layer(text_date_layer),false);
		layer_set_hidden(text_layer_get_layer(battery_text_layer),false);
		layer_set_hidden(text_layer_get_layer(text_time_layer),false);
	
	
}



static void my_next_click_handler(ClickRecognizerRef recognizer, void *context) {
action_bar_layer_destroy(action_bar);
text_layer_destroy(s_detail_layer);
text_layer_destroy(s_output_layer);	
	showmain();
	
}



void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, (ClickHandler) my_next_click_handler);

	
}


void showmessage_window(){
	battery_state_service_unsubscribe();
	 bluetooth_connection_service_unsubscribe();
	 window_set_background_color(s_main_window, GColorWhite);
		 layer_set_hidden(bitmap_layer_get_layer(icon_layer), true);
  	   layer_set_hidden(bitmap_layer_get_layer(conn_icon_layer), true);
      
	layer_set_hidden(text_layer_get_layer(text_date_layer),true);
		layer_set_hidden(text_layer_get_layer(battery_text_layer),true);
		layer_set_hidden(text_layer_get_layer(text_time_layer),true);

	

	
		text_layer_set_text_color(s_detail_layer,GColorWhite );
  text_layer_set_font(s_output_layer, custom_font);
	text_layer_set_text_color(s_output_layer,GColorClear );
	text_layer_set_background_color(s_output_layer,GColorBlack   );
		text_layer_set_background_color(s_detail_layer,GColorClear   );
  text_layer_set_text(s_output_layer, "");
  text_layer_set_overflow_mode(s_output_layer, GTextOverflowModeWordWrap);
		text_layer_set_text_alignment(s_detail_layer,GTextAlignmentCenter );
	text_layer_set_text_alignment(s_output_layer,GTextAlignmentCenter );
  layer_add_child(window_layer, text_layer_get_layer(s_output_layer));
	 layer_add_child(window_layer, text_layer_get_layer(s_detail_layer));
action_bar = action_bar_layer_create();
	action_bar_layer_set_background_color(action_bar,GColorWhite);
 action_bar_layer_add_to_window(action_bar, s_main_window);
  action_bar_layer_set_click_config_provider(action_bar,
                                             click_config_provider);

  action_bar_layer_set_icon(action_bar, BUTTON_ID_SELECT, future_bitmap);	
}


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  // Get the first pair
  Tuple *t = dict_read_first(iterator);
 
showmessage_window();

	
  // Process all pairs present
  while (t != NULL) {
    // Long lived buffer
    static char s_buffer[64];


	  
    // Process this pair's key
   switch (t->key) {
      case KEY_COMPL:
        // Copy value and display
       vibes_double_pulse();
        snprintf(s_buffer, sizeof(s_buffer), "%s", t->value->cstring);
        text_layer_set_text(s_output_layer, s_buffer);
	    text_layer_set_text(s_detail_layer, "compliance");
       break;
	   
	   case KEY_DAYS:
        // Copy value and display
       vibes_double_pulse();
        snprintf(s_buffer, sizeof(s_buffer), "%s", t->value->cstring);
        text_layer_set_text(s_output_layer, s_buffer);
	    text_layer_set_text(s_detail_layer, "days streak");
       break;
	   
	  
	   
   }

    // Get next pair, if any
    t = dict_read_next(iterator);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}




static void main_window_load(Window *window) {
   window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);

	
  // Create output TextLayer
 	s_detail_layer = text_layer_create(GRect(0, window_bounds.size.h-60, window_bounds.size.w-10 , 30));
  s_output_layer = text_layer_create(GRect(5, 20, window_bounds.size.w - 5, window_bounds.size.h-40));
	text_layer_set_font(s_detail_layer,fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD ));

  text_date_layer = text_layer_create(GRect(8, 68, 144-8, 168-68));
	icon_layer = bitmap_layer_create(GRect(80, -10, 60,60));	
	conn_icon_layer= bitmap_layer_create(GRect(5, -6, 60,60));	
		  battery_text_layer = text_layer_create(GRect(83, 30, 55, 120));
	  conn_text_layer = text_layer_create(GRect(2, 42, 85, 60));
	
	
	showmain();
	


}





static void main_window_unload(Window *window) {
  // Destroy output TextLayer
  text_layer_destroy(s_output_layer);
	// text_layer_destroy(s_detail_layer);
	// text_layer_destroy(s_output_layer);
}

static void init() {
  // Register callbacks
	future_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CLOSE_ICON);
  custom_font = fonts_load_custom_font
   (resource_get_handle(RESOURCE_ID_FONT_OSP_DIN_54));
  custom_font2 = fonts_load_custom_font
   (resource_get_handle(RESOURCE_ID_FONT_OSP_DIN_42));
	 custom_font_bat =  fonts_load_custom_font
   (resource_get_handle(RESOURCE_ID_FONT_OSP_DIN_22));
		 custom_font_conn =  fonts_load_custom_font
   (resource_get_handle(RESOURCE_ID_FONT_OSP_DIN_16));
	
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());



  // Create main Window
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_stack_push(s_main_window, true);
}

static void deinit() {
  // Destroy main Window
	tick_timer_service_unsubscribe();
	  battery_state_service_unsubscribe();
	bluetooth_connection_service_unsubscribe();
	
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}