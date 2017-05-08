#ifndef _gnomemenu_
#define _gnomemenu_


#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>

typedef struct {
	GtkApplication *app;
	GtkWidget *window;
	GtkWidget *overlay;
	GtkWidget *grid;
	GtkWidget *statusbar;
	GtkWidget *gearmenubutton;
	guint id;
	GtkWidget *button;
	GtkStyleContext *context1, *context2;
	gint chosenNumber[6], spentNumber[6];
	gchar bufferStatusBar[256];
	gchar msg[256];
	int difficulty;
	int firstgame;
	int j, i, gamecount;
	int sec_expired;
	int goodcount, timecount;
} widgets;

#define EASY 4
#define MEDIUM 8
#define HARD 12

void construct_overlay (GtkApplication *app, GtkWidget *box, gpointer data, int gridsize);
void construct_overlay_play (GtkApplication *app, GtkWidget *box, gpointer data, int gridsize);
void getInteger(char *input, unsigned int *numInteger);
void message_dialog_lostgame (GSimpleAction *action, GVariant *parameter, gpointer data, gchar *showText);
void _start_timer (gpointer data);
void _reset_timer (gpointer data);
void _pause_resume_timer (gpointer data);


#endif
