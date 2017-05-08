#ifndef _menucallbacks_
#define _menucallbacks_

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

void message_dialog (GSimpleAction *action, GVariant *parameter, gpointer data, gchar *showText);
void restart_callback (GSimpleAction *action, GVariant *parameter, gpointer data);
void easy_callback (GSimpleAction *action, GVariant *parameter, gpointer data);
void medium_callback (GSimpleAction *action, GVariant *parameter, gpointer data);
void hard_callback (GSimpleAction *action, GVariant *parameter, gpointer data);
void view_callback (GSimpleAction *action, GVariant *parameter, gpointer data);
void quit_callback (GSimpleAction *action, GVariant *parameter, gpointer data);
void about_callback (GSimpleAction *action, GVariant *parameter, gpointer data);
void help_callback (GSimpleAction *action, GVariant *parameter, gpointer data);
void message_dialog_startgame (GSimpleAction *action, GVariant *parameter, gpointer data);


#endif
