#include "gnomemenu.h"
#include "menucallbacks.h"

void message_dialog (GSimpleAction *action, GVariant *parameter, gpointer data, gchar *showText)
{
	const gchar *name;
	GtkWidget *dialog;
	widgets *a = (widgets *) data;

	name = g_action_get_name (G_ACTION (action));
	dialog = gtk_message_dialog_new (GTK_WINDOW (a->window),	 GTK_DIALOG_DESTROY_WITH_PARENT,
									 GTK_MESSAGE_INFO,
									 GTK_BUTTONS_OK,
									 "%s %s", name, showText);
	g_signal_connect (dialog, "response", G_CALLBACK (gtk_widget_destroy), NULL);
	gtk_widget_show (dialog);
}


void message_dialog_OK (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	GtkWidget *dialog;
	gint response;

	widgets *a = (widgets *) data;

	dialog = gtk_dialog_new_with_buttons ("really want to restart?", 	  GTK_WINDOW (a->window),
										  GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,
										  ("_OK"),
										  GTK_RESPONSE_OK,
										  "_Cancel",
										  GTK_RESPONSE_CANCEL,
										  NULL);

	gtk_widget_show (dialog);
	response = gtk_dialog_run (GTK_DIALOG (dialog));

	if (response == GTK_RESPONSE_OK)
	{
		construct_overlay (a->app, NULL, (gpointer) a, a->difficulty);
		gtk_widget_destroy (dialog);
	}
	else
	{
		gtk_widget_destroy (dialog);
	}
}

void message_dialog_startgame (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	GtkWidget *dialog;
	gint response;

	widgets *a = (widgets *) data;

	dialog = gtk_dialog_new_with_buttons ("ready to start new game?", 	  GTK_WINDOW (a->window),
										  GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,
										  ("_OK"),
										  GTK_RESPONSE_OK,
										  "_Cancel",
										  GTK_RESPONSE_CANCEL,
										  NULL);

	gtk_widget_show (dialog);
	response = gtk_dialog_run (GTK_DIALOG (dialog));

	if (response == GTK_RESPONSE_OK)
	{
		if (a->firstgame == 0)
		{
			gtk_container_remove (GTK_CONTAINER (a->window), a->overlay);
		}
		
		construct_overlay (a->app, NULL, (gpointer) a, a->difficulty);
		gtk_widget_destroy (dialog);
	}
	else
	{
		gtk_widget_destroy (dialog);
	}	
}

void restart_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	widgets *a = (widgets *) data;
	
	_reset_timer((gpointer) a);

	if (a->firstgame == 1)
	{
		message_dialog (action, NULL, (gpointer) a, "?\n\nyou did not start\nup to now...");
		g_print ("restart was clicked but first game...\n");
	}
	else
	{
		message_dialog_OK (action, NULL, (gpointer) a);
		g_print ("restart was clicked ...\n");
	}
}

void easy_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	widgets *a = (widgets *) data;

	g_print ("easy was clicked ...\n");
	a->difficulty = EASY;
	message_dialog_startgame (action, NULL, (gpointer) a);
	g_print ("start was clicked ...\n");
	
}

void medium_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	widgets *a = (widgets *) data;

	g_print ("medium was clicked ...\n");
	a->difficulty = MEDIUM;
	message_dialog_startgame (action, NULL, (gpointer) a);
}

void hard_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	widgets *a = (widgets *) data;

	g_print ("hard was clicked ...\n");
	a->difficulty = HARD;
	message_dialog_startgame (action, NULL, (gpointer) a);
}


void view_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	widgets *a = (widgets *) data;

	message_dialog (action, NULL, (gpointer) a, "view klick");
	g_print ("view was clicked ...\n");
}

void quit_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	widgets *a = (widgets *) data;

	g_print ("quit was clicked ...\n");
	g_application_quit (G_APPLICATION (a->app));
}

void about_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	widgets *a = (widgets *) data;

	message_dialog (action, NULL, (gpointer) a, "section:\n\nsmall memory game\nby el16b005 BEL2 SS2017\n\ncheck help section for details");
	g_print ("about was clicked ...\n");
}

void help_callback (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	widgets *a = (widgets *) data;

	message_dialog (action, NULL, (gpointer) a, "section:\n\nsmall memory game\n\n");
	g_print ("help was clicked ...\n");
}
