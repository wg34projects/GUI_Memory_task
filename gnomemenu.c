#include "gnomemenu.h"
#include "menucallbacks.h"

// map menu actions to callbacks

const GActionEntry app_actions[] =
{
	{ "restart", restart_callback },
	{ "easy", easy_callback },
	{ "medium", medium_callback },
	{ "hard", hard_callback },
	{ "view", view_callback },
	{ "quit", quit_callback },
	{ "about", about_callback },
	{ "help", help_callback }
};

static void construct_menu (GtkApplication *app, GtkWidget *box, gpointer data);
static void do_number (GtkButton *button, gpointer data);

void normal_button (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;

	gtk_widget_set_sensitive (GTK_WIDGET (a->gearmenubutton), TRUE);
	gtk_container_remove (GTK_CONTAINER (a->window), a->grid);
}

void easy_button (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;

	gtk_widget_set_sensitive (GTK_WIDGET (a->gearmenubutton), TRUE);
	gtk_container_remove (GTK_CONTAINER (a->window), a->grid);
	a->difficulty = EASY;
	message_dialog_startgame (NULL, NULL, (gpointer) a);
}

void medium_button (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;

	gtk_widget_set_sensitive (GTK_WIDGET (a->gearmenubutton), TRUE);
	gtk_container_remove (GTK_CONTAINER (a->window), a->grid);
	a->difficulty = MEDIUM;
	message_dialog_startgame (NULL, NULL, (gpointer) a);
}

void hard_button (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;

	gtk_widget_set_sensitive (GTK_WIDGET (a->gearmenubutton), TRUE);
	gtk_container_remove (GTK_CONTAINER (a->window), a->grid);
	a->difficulty = HARD;
	message_dialog_startgame (NULL, NULL, (gpointer) a);
}

void message_dialog_lostgame (GSimpleAction *action, GVariant *parameter, gpointer data, gchar *showText)
{
	GtkWidget *dialog;
	widgets *a = (widgets *) data;

	dialog = gtk_message_dialog_new (GTK_WINDOW (a->window),	 GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,
									 GTK_MESSAGE_INFO,
									 GTK_BUTTONS_OK,
									 "%s", showText);
	g_signal_connect (dialog, "response", G_CALLBACK (gtk_widget_destroy), NULL);
	gtk_widget_show (dialog);
	gtk_container_remove (GTK_CONTAINER (a->window), a->overlay);
	
}

void message_dialog_wongame (GSimpleAction *action, GVariant *parameter, gpointer data, gchar *showText)
{
	GtkWidget *dialog;
	widgets *a = (widgets *) data;

	dialog = gtk_message_dialog_new (GTK_WINDOW (a->window),	 GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,
									 GTK_MESSAGE_INFO,
									 GTK_BUTTONS_OK,
									 "%s", showText);
	g_signal_connect (dialog, "response", G_CALLBACK (gtk_widget_destroy), NULL);
	gtk_widget_show (dialog);
	gtk_container_remove (GTK_CONTAINER (a->window), a->overlay);
}

static void do_number (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;
	gchar *temp;
	unsigned int klick = 999;
	int k, goodcount = 0;
	
	a->gamecount++;
	
	printf("%d\n",a->gamecount);

	temp = (gchar*) gtk_button_get_label (button);
	g_snprintf (a->bufferStatusBar, 256, "%s - %s", a->msg, temp);
	gtk_statusbar_push (GTK_STATUSBAR (a->statusbar), a->id, a->bufferStatusBar);

	getInteger(temp, &klick);
	
	for (k = 0; k < (a->difficulty / 2); k++)
	{
		if (klick == a->chosenNumber[k])
		{			
			goodcount = 0;
			a->chosenNumber[k] = 999;
			break;			
		}
		else
		{
			goodcount = 1;
		}
		
	}

	
	gtk_statusbar_push (GTK_STATUSBAR (a->statusbar), a->id, a->bufferStatusBar);
	
		
	if (a->gamecount == a->difficulty/2)
	{
		message_dialog_wongame (NULL, NULL, (gpointer) a, "\n\nWON\n\n");
	}
		
	if (goodcount == 1)
	{
		message_dialog_lostgame (NULL, NULL, (gpointer) a, "\n\nLOST\n\n");
	}


}


static guint wait (gpointer data)
{
	widgets *a = (widgets *) data;

	gtk_container_remove (GTK_CONTAINER (a->window), a->overlay);
	construct_overlay_play (a->app, NULL, (gpointer) a, a->difficulty);

	return FALSE;
}

void getInteger(char *input, unsigned int *numInteger)
{
	unsigned long int number = 0;
	char *pointToEnd = NULL;

	number = strtoul(input, &pointToEnd, 0);
	*numInteger = (unsigned int)number;
}

void construct_overlay (GtkApplication *app, GtkWidget *box, gpointer data, int gridsize)
{
	GtkWidget *grid;
	GtkWidget *vbox;
	int k, l;
	int good = 0;
	time_t timer;
	time (&timer);
	srand ( (unsigned int) timer);

	widgets *a = (widgets *) data;

	a->firstgame = 0;

	a->overlay = gtk_overlay_new ();
	grid = gtk_grid_new ();
	gtk_container_add (GTK_CONTAINER (a->overlay), grid);

	a->statusbar = gtk_statusbar_new();
	gtk_widget_set_size_request (a->statusbar, 300, 10);

	gtk_grid_attach (GTK_GRID (grid), a->statusbar, 0, gridsize+1, gridsize, 1);

	a->id = gtk_statusbar_get_context_id (GTK_STATUSBAR (a->statusbar), "demo");

	while (1)
	{
		for (k = 0; k < 6; k++)
		{
			a->chosenNumber[k] = rand() % (gridsize*gridsize);
		}

		for(k = 6; k > 0; k--)
		{
			for(l = k - 1; l >= 0; l--)
			{
				if(a->chosenNumber[k] == a->chosenNumber[l])
				{
					good++;
				}
			}
		}

		if (good == 0)
		{
			break;
		}
		else
		{
			good = 0;
		}
	}

	for (a->j = 0; a->j < a->difficulty; a->j++)
	{
		for (a->i = 0; a->i < a->difficulty; a->i++)
		{
			a->button = gtk_button_new();

			GValue opacity = G_VALUE_INIT;
			g_value_init (&opacity, G_TYPE_FLOAT);
			g_value_set_float (&opacity, 1.0);
			g_object_set_property (G_OBJECT (a->button), "opacity", &opacity);
			g_value_unset (&opacity);

			a->context1 = gtk_widget_get_style_context (a->button);
			gtk_style_context_add_class (a->context1, "suggested-action");

			gtk_widget_set_hexpand (a->button, TRUE);
			gtk_widget_set_vexpand (a->button, TRUE);

			gtk_grid_attach (GTK_GRID (grid), a->button, a->i, a->j, 1, 1);

			for (k = 0; k < (a->difficulty / 2); k++)
			{
				if (a->difficulty*a->j+a->i == a->chosenNumber[k])
				{
					a->context1 = gtk_widget_get_style_context (a->button);
					gtk_style_context_add_class (a->context1, "destructive-action");
				}
			}
		}
	}

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_overlay_add_overlay (GTK_OVERLAY (a->overlay), vbox);
	gtk_overlay_set_overlay_pass_through (GTK_OVERLAY (a->overlay), vbox, TRUE);
	gtk_widget_set_halign (vbox, GTK_ALIGN_CENTER);
	gtk_widget_set_valign (vbox, GTK_ALIGN_CENTER);

	gtk_container_add (GTK_CONTAINER (a->window), a->overlay);

	gtk_widget_show_all (GTK_WIDGET (a->window));

	g_snprintf (a->msg, 256, "watch out the places for %d secondes...", a->difficulty/2);
	gtk_statusbar_push (GTK_STATUSBAR (a->statusbar), a->id, a->msg);

	g_timeout_add (a->difficulty*500, (GSourceFunc) wait, (gpointer) a);
}

void construct_overlay_play (GtkApplication *app, GtkWidget *box, gpointer data, int gridsize)
{
	GtkWidget *grid;
	GtkWidget *vbox;
	char *text;
	time_t timer;
	time (&timer);
	srand ( (unsigned int) timer);

	widgets *a = (widgets *) data;

	a->overlay = gtk_overlay_new ();
	grid = gtk_grid_new ();
	gtk_container_add (GTK_CONTAINER (a->overlay), grid);

	a->statusbar = gtk_statusbar_new();
	gtk_widget_set_size_request (a->statusbar, 300, 10);

	gtk_grid_attach (GTK_GRID (grid), a->statusbar, 0, gridsize+1, gridsize, 1);

	a->id = gtk_statusbar_get_context_id (GTK_STATUSBAR (a->statusbar), "demo");

	for (a->j = 0; a->j < a->difficulty; a->j++)
	{
		for (a->i = 0; a->i < a->difficulty; a->i++)
		{
			text = g_strdup_printf ("%d", a->difficulty*a->j + a->i);

			a->button = gtk_button_new_with_label (text);

			GValue opacity = G_VALUE_INIT;
			g_value_init (&opacity, G_TYPE_FLOAT);
			g_value_set_float (&opacity, 1.0);
			g_object_set_property (G_OBJECT (a->button), "opacity", &opacity);
			g_value_unset (&opacity);

			g_free (text);

			a->context1 = gtk_widget_get_style_context (a->button);
			gtk_style_context_add_class (a->context1, "suggested-action");

			gtk_widget_set_hexpand (a->button, TRUE);
			gtk_widget_set_vexpand (a->button, TRUE);

			g_signal_connect (a->button, "clicked", G_CALLBACK (do_number), (gpointer) a);
			gtk_grid_attach (GTK_GRID (grid), a->button, a->i, a->j, 1, 1);
			a->gamecount = 0;
		}
	}

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_overlay_add_overlay (GTK_OVERLAY (a->overlay), vbox);
	gtk_overlay_set_overlay_pass_through (GTK_OVERLAY (a->overlay), vbox, TRUE);
	gtk_widget_set_halign (vbox, GTK_ALIGN_CENTER);
	gtk_widget_set_valign (vbox, GTK_ALIGN_CENTER);

	gtk_container_add (GTK_CONTAINER (a->window), a->overlay);

	gtk_widget_show_all (GTK_WIDGET (a->window));

	g_snprintf (a->msg, 256, "select the correct %02d | %02d | %02d | %02d | %02d | %02d", a->chosenNumber[0], a->chosenNumber[1], a->chosenNumber[2], a->chosenNumber[3], a->chosenNumber[4], a->chosenNumber[5]);
	gtk_statusbar_push (GTK_STATUSBAR (a->statusbar), a->id, a->msg);
}

static void construct_menu (GtkApplication *app, GtkWidget *box, gpointer data)
{
	GtkWidget *headerbar;

	// the application menu displayed in the GNOME panel
	GMenu *appmenu;
	GMenu *gamemenu;
	GMenu *helpmenu;
	GMenu *gamesubmenu;
	GMenu *helpsubmenu;
	GMenu *diffsubmenu;

	// the menu displayed as a popover below the gears button
	GMenu *gearmenu;
	GtkWidget *gearicon;

	widgets *a = (widgets *) data;

	// define keyboard accelerators
	const gchar *restart_accels[2] = 	{ "<Ctrl>R", NULL };
	const gchar *view_accels[2] = 		{ "<Ctrl>V", NULL };
	const gchar *quit_accels[2] =  		{ "<Ctrl>Q", NULL };
	const gchar *about_accels[2] =  	{ "<Ctrl>A", NULL };
	const gchar *help_accels[2] =   	{ "F1", NULL };

	// create and fill in the application menu in the GNOME panel
	appmenu = g_menu_new();
	g_menu_append (appmenu, "about", "app.about");
	g_menu_append (appmenu, "help", "app.help");
	g_menu_append (appmenu, "_quit", "app.quit");
	gtk_application_set_app_menu (GTK_APPLICATION (app), G_MENU_MODEL (appmenu));
	g_object_unref (appmenu);

	// create a headerbar
	headerbar = gtk_header_bar_new ();
	gtk_widget_show (headerbar);
	gtk_header_bar_set_title (GTK_HEADER_BAR (headerbar), "GTK GUI Task BEL2 - memory game");
	gtk_header_bar_set_subtitle (GTK_HEADER_BAR (headerbar), "el16b005");
	gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (headerbar), TRUE);
	gtk_window_set_titlebar (GTK_WINDOW (a->window), headerbar);

	// create the gear menu button
	a->gearmenubutton = gtk_menu_button_new();
	gearicon = gtk_image_new_from_icon_name ("emblem-system-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_button_set_image (GTK_BUTTON (a->gearmenubutton), gearicon);
	gtk_header_bar_pack_end (GTK_HEADER_BAR (headerbar), a->gearmenubutton);

	// create a menu for the gear button
	gearmenu = g_menu_new();

	gamemenu = g_menu_new();
	helpmenu = g_menu_new();

	diffsubmenu = g_menu_new();
	g_menu_append (diffsubmenu, "easy", "app.easy");
	g_menu_append (diffsubmenu, "medium", "app.medium");
	g_menu_append (diffsubmenu, "hard", "app.hard");

	helpsubmenu = g_menu_new();
	g_menu_append (helpsubmenu, "about", "app.about");
	g_menu_append (helpsubmenu, "_help", "app.help");

	gamesubmenu = g_menu_new();
	g_menu_append (gamesubmenu, "restart game", "app.restart");
	g_menu_append_submenu (gamesubmenu, "_change difficulty level", G_MENU_MODEL (diffsubmenu));
	g_menu_append (gamesubmenu, "_view highscore", "app.view");
	g_menu_append (gamesubmenu, "_quit game", "app.quit");

	g_menu_append_submenu (gamemenu, "game section", G_MENU_MODEL (gamesubmenu));
	g_menu_append_section (gearmenu, NULL, G_MENU_MODEL (gamemenu));
	g_menu_append_submenu (helpmenu, "help section", G_MENU_MODEL (helpsubmenu));
	g_menu_append_section (gearmenu, NULL, G_MENU_MODEL (helpmenu));

	gtk_menu_button_set_menu_model (GTK_MENU_BUTTON (a->gearmenubutton), G_MENU_MODEL (gearmenu));

	g_object_unref (gamemenu);
	g_object_unref (helpmenu);
	g_object_unref (gearmenu);

	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "app.restart", restart_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "app.view", view_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "app.quit", quit_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "app.about", about_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "app.help", help_accels);

	gtk_widget_set_sensitive (GTK_WIDGET (a->gearmenubutton), FALSE);

}

static void activate (GtkApplication *app, gpointer data)
{
	GtkWidget *easyButton, *mediumButton, *hardButton, *normalButton;
	GtkWidget *labelName;

	widgets *a = (widgets *) data;

	a->window = gtk_application_window_new (app);
	gtk_window_set_application (GTK_WINDOW (a->window), GTK_APPLICATION (app));
	gtk_window_set_title (GTK_WINDOW (a->window), "GNOME Menu");
	gtk_window_set_position (GTK_WINDOW (a->window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (a->window), 700, 660);
	gtk_window_set_resizable (GTK_WINDOW (a->window), FALSE);

	construct_menu (app, NULL, (gpointer) a);
	a->difficulty = 7;

	a->grid = gtk_grid_new();
	gtk_grid_set_column_homogeneous (GTK_GRID (a->grid), TRUE);
	gtk_grid_set_row_spacing (GTK_GRID (a->grid), 5);
	gtk_grid_set_column_spacing (GTK_GRID (a->grid), 5);
	gtk_widget_set_margin_start (a->grid, 5);
	gtk_widget_set_margin_end (a->grid, 5);
	gtk_widget_set_margin_top (a->grid, 5);
	gtk_widget_set_margin_bottom (a->grid, 5);
	gtk_container_add (GTK_CONTAINER (a->window), a->grid);

	labelName = gtk_label_new ("\n\nchoose your difficulty level!\n");
	gtk_widget_set_halign (labelName, GTK_ALIGN_CENTER);
	gtk_widget_set_size_request (labelName, 40, 30);
	gtk_grid_attach (GTK_GRID (a->grid), labelName, 1, 0, 3, 1);

	easyButton = gtk_button_new_with_mnemonic ("\n_EASY\n");
	gtk_grid_attach (GTK_GRID (a->grid), easyButton, 2, 2, 1, 1);
	g_signal_connect (easyButton, "clicked", G_CALLBACK (easy_button), (gpointer) a);

	mediumButton = gtk_button_new_with_mnemonic ("\n_MEDIUM\n");
	gtk_grid_attach (GTK_GRID (a->grid), mediumButton, 2, 3, 1, 1);
	g_signal_connect (mediumButton, "clicked", G_CALLBACK (medium_button), (gpointer) a);

	hardButton = gtk_button_new_with_mnemonic ("\n_HARD\n");
	gtk_grid_attach (GTK_GRID (a->grid), hardButton, 2, 4, 1, 1);
	g_signal_connect (hardButton, "clicked", G_CALLBACK (hard_button), (gpointer) a);

	labelName = gtk_label_new ("\nor start with all menu options\n");
	gtk_widget_set_halign (labelName, GTK_ALIGN_CENTER);
	gtk_widget_set_size_request (labelName, 40, 30);
	gtk_grid_attach (GTK_GRID (a->grid), labelName, 1, 5, 3, 1);

	normalButton = gtk_button_new_with_mnemonic ("\n_ALL OPTIONS\n");
	gtk_grid_attach (GTK_GRID (a->grid), normalButton, 2, 6, 1, 1);
	g_signal_connect (normalButton, "clicked", G_CALLBACK (normal_button), (gpointer) a);

	a->firstgame = 1;

	gtk_widget_show_all (GTK_WIDGET (a->window));
}

static void startup (GApplication *app, gpointer data)
{
	widgets *a = (widgets *) data;

	g_action_map_add_action_entries (G_ACTION_MAP (app), app_actions, G_N_ELEMENTS (app_actions), (gpointer) a);
}

int main (int argc, char **argv)
{
	int status;
	widgets *a = g_malloc (sizeof (widgets));

	a->app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
	g_signal_connect (a->app, "activate", G_CALLBACK (activate), (gpointer) a);
	g_signal_connect (a->app, "startup", G_CALLBACK (startup), (gpointer) a);
	status = g_application_run (G_APPLICATION (a->app), argc, argv);
	g_object_unref (a->app);

	g_free (a);
	return status;
}
