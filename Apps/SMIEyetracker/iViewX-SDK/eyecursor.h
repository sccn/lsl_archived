#ifndef AL_EYECURSOR_H
#define EYECURSOR_H


#include "allegro5/allegro.h"
#include "allegro5/allegro_primitives.h"
#include "allegro5/allegro_image.h"
#include "stdio.h"
#include <float.h>
const int BOUNCER_SIZE = 16;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_BITMAP *bouncer = NULL;
boost::mutex display_mutex;

int open_display(int displayLeft, int displayTop, int displayWidth, int displayHeight) {
	display_mutex.lock();
	al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_REQUIRE);
	al_set_new_display_flags(/*ALLEGRO_FULLSCREEN_WINDOW |*/ ALLEGRO_NOFRAME);
 
   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }

   if(!al_init_primitives_addon()) {
      fprintf(stderr, "failed to initialize allegro primitives addon!\n");
      return -1;
   }
 
   display = al_create_display(displayWidth, displayHeight);
   al_set_window_position(display,displayLeft,displayTop);
  
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      return -1;
   }
 
   bouncer = al_create_bitmap(BOUNCER_SIZE, BOUNCER_SIZE);
   if(!bouncer) {
      fprintf(stderr, "failed to create bouncer bitmap!\n");
      al_destroy_display(display);
      return -1;
   }
 
   al_set_target_bitmap(bouncer);
 
   al_clear_to_color(al_map_rgb(128, 128, 128));
   al_draw_filled_circle(BOUNCER_SIZE/2.0, BOUNCER_SIZE/2.0, BOUNCER_SIZE/2.0, al_map_rgb(255,0,0));
 
   al_set_target_bitmap(al_get_backbuffer(display));
  
   al_clear_to_color(al_map_rgb(128,128,128));
 
   al_flip_display();
 
   display_mutex.unlock();
   return 0;
}

void update_display(double x, double y) {
	 display_mutex.lock();
	 al_set_target_bitmap(al_get_backbuffer(display));
     al_clear_to_color(al_map_rgb(128,128,128));
	 
	 if(bouncer) al_draw_bitmap(bouncer, x-BOUNCER_SIZE/2, y-BOUNCER_SIZE/2, 0);
     al_flip_display();
	 display_mutex.unlock();
}


void destroy_display() {
    display_mutex.lock();
	al_destroy_display(display);
	al_destroy_bitmap(bouncer);
/*	al_shutdown_primitives_addon();
	al_uninstall_system();
*/	display_mutex.unlock();
  }
#endif