#include "gnomemenu.h"
#include "menucallbacks.h"

const GActionEntry app_actions[] =
{
	{ "restart", 	restart_callback },
	{ "easy", 	easy_callback },
	{ "medium", 	medium_callback },
	{ "hard", 	hard_callback },
	{ "view", 	view_callback },
	{ "quit", 	quit_callback },
	{ "about", 	about_callback },
	{ "help", 	help_callback }
};

gboolean _label_update(gpointer data)
{
	widgets *a = (widgets *) data;
	++a->sec_expired;

	if (a->continue_timer)
	{
		g_snprintf (a->bufferStatusBar, 255, "time elapsed %03d s | countdown %03d s | spent klicks %d from %d", a->sec_expired, a->difficulty*2 - a->sec_expired, a->gamecount, a->difficulty / 2);
		gtk_statusbar_push (GTK_STATUSBAR (a->statusbar), a->id, a->bufferStatusBar);
	}

	if (a->sec_expired > a->difficulty*2)
	{
		a->continue_timer = FALSE;
		a->start_timer = FALSE;
		message_dialog_lostgame (NULL, NULL, (gpointer) a, "\ntime ran out, you lost...\nplay again to improve....");
	}

	return a->continue_timer;
}

void _start_timer (gpointer data)
{
	widgets *a = (widgets *) data;

	if(!a->start_timer)
	{
		g_timeout_add_seconds(1, _label_update, a);
		a->start_timer = TRUE;
		a->continue_timer = TRUE;
	}
}

void _pause_resume_timer (gpointer data)
{
	widgets *a = (widgets *) data;
	if(a->start_timer)
	{
		if(a->continue_timer)
		{
			g_timeout_add_seconds(1, _label_update, a);
		}
		else
		{
			a->sec_expired--;
		}
	}
}

void _reset_timer (gpointer data)
{
	widgets *a = (widgets *) data;

	a->sec_expired = -1;
	a->continue_timer = FALSE;
	a->start_timer = FALSE;
}

void normal_button (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;

	_pause_resume_timer((gpointer) a);
	_reset_timer((gpointer) a);

	a->continue_timer = FALSE;
	a->start_timer = FALSE;

	gtk_widget_set_sensitive (GTK_WIDGET (a->gearmenubutton), TRUE);
}

void high_button (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;

	_pause_resume_timer((gpointer) a);
	_reset_timer((gpointer) a);

	a->continue_timer = FALSE;
	a->start_timer = FALSE;

	gtk_widget_set_sensitive (GTK_WIDGET (a->gearmenubutton), TRUE);
	constructHighscorePage (a->app, (gpointer) a);
}

void easy_button (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;

	_pause_resume_timer((gpointer) a);
	_reset_timer((gpointer) a);

	a->continue_timer = FALSE;
	a->start_timer = FALSE;

	gtk_widget_set_sensitive (GTK_WIDGET (a->gearmenubutton), TRUE);
	gtk_container_remove (GTK_CONTAINER (a->window), a->grid);
	a->difficulty = EASY;
	message_dialog_startgame (NULL, NULL, (gpointer) a);
}

void medium_button (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;

	_pause_resume_timer((gpointer) a);
	_reset_timer((gpointer) a);

	a->continue_timer = FALSE;
	a->start_timer = FALSE;

	gtk_widget_set_sensitive (GTK_WIDGET (a->gearmenubutton), TRUE);
	gtk_container_remove (GTK_CONTAINER (a->window), a->grid);
	a->difficulty = MEDIUM;
	message_dialog_startgame (NULL, NULL, (gpointer) a);
}

void hard_button (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;

	_pause_resume_timer((gpointer) a);
	_reset_timer((gpointer) a);

	a->continue_timer = FALSE;
	a->start_timer = FALSE;

	gtk_widget_set_sensitive (GTK_WIDGET (a->gearmenubutton), TRUE);
	gtk_container_remove (GTK_CONTAINER (a->window), a->grid);
	a->difficulty = HARD;
	message_dialog_startgame (NULL, NULL, (gpointer) a);
}

void message_dialog_lostgame (GSimpleAction *action, GVariant *parameter, gpointer data, gchar *showText)
{
	GtkWidget *dialog;
	widgets *a = (widgets *) data;

	_pause_resume_timer((gpointer) a);
	_reset_timer((gpointer) a);

	a->continue_timer = FALSE;
	a->start_timer = FALSE;

	dialog = gtk_message_dialog_new (GTK_WINDOW (a->window),	 GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,
									 GTK_MESSAGE_INFO,
									 GTK_BUTTONS_OK,
									 "%s", showText);

	g_signal_connect (dialog, "response", G_CALLBACK (gtk_widget_destroy), NULL);
	gtk_widget_show (dialog);
	a->won = 1;
	gtk_container_remove (GTK_CONTAINER (a->window), a->overlay);
	constructWelcomePage (a->app, (gpointer) a);
}

void message_dialog_wongame (GSimpleAction *action, GVariant *parameter, gpointer data)
{
	GtkWidget *dialog, *label;
	gint response;
	GtkWidget *content_area, *grid;
	
	widgets *a = (widgets *) data;

	_pause_resume_timer((gpointer) a);
	_reset_timer((gpointer) a);

	a->continue_timer = FALSE;
	a->start_timer = FALSE;
	
	dialog = gtk_dialog_new_with_buttons ("Highscore yes-no?", 	  
	                                      GTK_WINDOW (a->window),
										  GTK_DIALOG_MODAL| GTK_DIALOG_DESTROY_WITH_PARENT,
										  ("_OK"),
										  GTK_RESPONSE_OK,
										  "_Cancel",
										  GTK_RESPONSE_CANCEL,
										  NULL);

	content_area = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	grid = gtk_grid_new();
	gtk_grid_set_row_homogeneous (GTK_GRID (grid), TRUE);
	gtk_grid_set_column_homogeneous (GTK_GRID (grid), TRUE);
	gtk_grid_set_row_spacing (GTK_GRID (grid), 5);
	gtk_container_add (GTK_CONTAINER (content_area), grid);
	gtk_container_set_border_width (GTK_CONTAINER (content_area), 10);
	label = gtk_label_new ("you won, do you want to enter your data to the highscore list?\n\nyou can later watch your success from the menu");
	gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 1);	
		
	gtk_widget_show_all (dialog);
	response = gtk_dialog_run (GTK_DIALOG (dialog));
	
	if (response == GTK_RESPONSE_OK)
	{
		gtk_container_remove (GTK_CONTAINER (a->window), a->overlay);
		constructHighscorePage (a->app, (gpointer) a);
		gtk_widget_destroy (dialog);
	}
	else
	{
		gtk_container_remove (GTK_CONTAINER (a->window), a->overlay);
		constructWelcomePage (a->app, (gpointer) a);
		gtk_widget_destroy (dialog);
	}
	
}

void do_number (GtkButton *button, gpointer data)
{
	widgets *a = (widgets *) data;
	gchar *temp;
	unsigned int klick = 999;
	int m, correctcount = 0;

	a->gamecount++;

	temp = (gchar*) gtk_button_get_label (button);

	getInteger(temp, &klick);

	for (m = 0; m < (a->difficulty / 2); m++)
	{
		if (klick == a->chosenNumber[m])
		{
			a->goodcount = 0;
			a->chosenNumber[m] = 999;
			correctcount++;
			break;
		}
		else
		{
			a->goodcount = 1;
		}
	}

	if (a->gamecount == a->difficulty / 2)
	{
		a->continue_timer = FALSE;
		message_dialog_wongame (NULL, NULL, (gpointer) a);
		return;
	}

	if (a->goodcount == 1)
	{
		a->continue_timer = FALSE;
		message_dialog_lostgame (NULL, NULL, (gpointer) a, "\nyou LOST... keep training...\nand maybe check the help...");
		return;
	}
}

guint wait (gpointer data)
{
	widgets *a = (widgets *) data;

	_pause_resume_timer((gpointer) a);
	_reset_timer((gpointer) a);

	a->continue_timer = FALSE;
	a->start_timer = FALSE;
		
	gtk_container_remove (GTK_CONTAINER (a->window), a->overlay);
	construct_overlay_play (a->app, NULL, (gpointer) a);

	return FALSE;
}

void getInteger(char *input, unsigned int *numInteger)
{
	unsigned long int number = 0;
	char *pointToEnd = NULL;

	number = strtoul(input, &pointToEnd, 0);
	*numInteger = (unsigned int)number;
}


void construct_overlay_play (GtkApplication *app, GtkWidget *box, gpointer data)
{
	GtkWidget *grid;
	GtkWidget *vbox;
	char *text;
	time_t timer;
	time (&timer);
	srand ( (unsigned int) timer);
	int k;

	widgets *a = (widgets *) data;

	printf("CHEATER mode -");

	for (k = 0; k < (a->difficulty / 2); k++)
	{
		printf(" %03d |", a->chosenNumber[k]);
	}
	printf("\n");

	a->overlay = gtk_overlay_new ();
	grid = gtk_grid_new ();
	gtk_container_add (GTK_CONTAINER (a->overlay), grid);

	a->statusbar = gtk_statusbar_new();
	gtk_widget_set_size_request (a->statusbar, 300, 10);
	gtk_grid_attach (GTK_GRID (grid), a->statusbar, 0, a->difficulty+1, a->difficulty, 1);
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

	_start_timer((gpointer) a);

	g_snprintf (a->msg, 256, "klick the correct buttons in time...");
}

void construct_overlay (GtkApplication *app, GtkWidget *box, gpointer data)
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

	gtk_grid_attach (GTK_GRID (grid), a->statusbar, 0, a->difficulty+1, a->difficulty, 1);

	a->id = gtk_statusbar_get_context_id (GTK_STATUSBAR (a->statusbar), "demo");

	while (1)
	{
		for (k = 0; k < a->difficulty / 2; k++)
		{
			a->chosenNumber[k] = rand() % (a->difficulty*a->difficulty);
		}

		for(k = a->difficulty / 2; k > 0; k--)
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
			a->buttons[a->difficulty*a->j + a->i] = gtk_button_new();

			GValue opacity = G_VALUE_INIT;
			g_value_init (&opacity, G_TYPE_FLOAT);
			g_value_set_float (&opacity, 1.0);
			g_object_set_property (G_OBJECT (a->buttons[a->difficulty*a->j + a->i]), "opacity", &opacity);
			g_value_unset (&opacity);

			a->context1 = gtk_widget_get_style_context (a->buttons[a->difficulty*a->j + a->i]);
			gtk_style_context_add_class (a->context1, "suggested-action");

			gtk_widget_set_hexpand (a->buttons[a->difficulty*a->j + a->i], TRUE);
			gtk_widget_set_vexpand (a->buttons[a->difficulty*a->j + a->i], TRUE);

			gtk_grid_attach (GTK_GRID (grid), a->buttons[a->difficulty*a->j + a->i], a->i, a->j, 1, 1);
		}
	}

	vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
	gtk_overlay_add_overlay (GTK_OVERLAY (a->overlay), vbox);
	gtk_overlay_set_overlay_pass_through (GTK_OVERLAY (a->overlay), vbox, TRUE);
	gtk_widget_set_halign (vbox, GTK_ALIGN_CENTER);
	gtk_widget_set_valign (vbox, GTK_ALIGN_CENTER);    	

	gtk_container_add (GTK_CONTAINER (a->window), a->overlay);

	gtk_widget_show_all (GTK_WIDGET (a->window));

	g_snprintf (a->msg, 256, "watch out the highlighted places for %d secondes and remember the location...", a->difficulty / 2);
	gtk_statusbar_push (GTK_STATUSBAR (a->statusbar), a->id, a->msg);	
	
	for (a->k = 0; a->k < (a->difficulty / 2); a->k++)
	{		
		GtkWidget *button_image = gtk_image_new_from_file("start-here.png");
		gtk_button_set_image(GTK_BUTTON(a->buttons[a->chosenNumber[a->k]]), button_image);
	}	
	
	g_timeout_add (a->difficulty*500, (GSourceFunc) wait, (gpointer) a);
}

void construct_menu (GtkApplication *app, gpointer data)
{
	GtkWidget *headerbar;
	GMenu *appmenu;
	GMenu *gamemenu;
	GMenu *helpmenu;
	GMenu *gamesubmenu;
	GMenu *helpsubmenu;
	GMenu *diffsubmenu;
	GMenu *gearmenu;
	GtkWidget *gearicon;

	widgets *a = (widgets *) data;

	const gchar *restart_accels[2] = 	{ "<Ctrl>R", NULL };
	const gchar *view_accels[2] = 		{ "<Ctrl>V", NULL };
	const gchar *quit_accels[2] =  		{ "<Ctrl>Q", NULL };
	const gchar *about_accels[2] =  	{ "<Ctrl>A", NULL };
	const gchar *help_accels[2] =   	{ "F1", NULL };

	appmenu = g_menu_new();
	g_menu_append (appmenu, "about", "app.about");
	g_menu_append (appmenu, "help", "app.help");
	g_menu_append (appmenu, "_quit", "app.quit");
	gtk_application_set_app_menu (GTK_APPLICATION (app), G_MENU_MODEL (appmenu));
	g_object_unref (appmenu);

	headerbar = gtk_header_bar_new ();
	gtk_widget_show (headerbar);
	gtk_header_bar_set_title (GTK_HEADER_BAR (headerbar), "GTK GUI Task BEL2 - memory game");
	gtk_header_bar_set_subtitle (GTK_HEADER_BAR (headerbar), "el16b005 - helmut resch - www.dwell.at");
	gtk_header_bar_set_show_close_button (GTK_HEADER_BAR (headerbar), TRUE);
	gtk_window_set_titlebar (GTK_WINDOW (a->window), headerbar);

	a->gearmenubutton = gtk_menu_button_new();
	gearicon = gtk_image_new_from_icon_name ("emblem-system-symbolic", GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_button_set_image (GTK_BUTTON (a->gearmenubutton), gearicon);
	gtk_header_bar_pack_end (GTK_HEADER_BAR (headerbar), a->gearmenubutton);

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

void constructWelcomePage (GtkApplication *app, gpointer data)
{
	GtkWidget *easyButton, *mediumButton, *hardButton, *normalButton, *highButton;
	GtkWidget *labelName;

	widgets *a = (widgets *) data;

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
	
	labelName = gtk_label_new ("\nor check the highscores\n");
	gtk_widget_set_halign (labelName, GTK_ALIGN_CENTER);
	gtk_widget_set_size_request (labelName, 40, 30);
	gtk_grid_attach (GTK_GRID (a->grid), labelName, 1, 7, 3, 1);

	highButton = gtk_button_new_with_mnemonic ("\n_HIGHSCORE\n");
	gtk_grid_attach (GTK_GRID (a->grid), highButton, 2, 8, 1, 1);
	g_signal_connect (highButton, "clicked", G_CALLBACK (high_button), (gpointer) a);	

	gtk_widget_show_all (GTK_WIDGET (a->window));
}

void constructHighscorePage (GtkApplication *app, gpointer data)
{
	GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);	
	GtkWidget *sw;
/*	widgets *a = (widgets *) data;*/
	GtkTextIter iter;
  	GtkTextIter start, end;
	GtkTextBuffer *buffer;
	GtkWidget *view1;
	
	gtk_container_set_border_width (GTK_CONTAINER (window), 25);
        gtk_window_set_title (GTK_WINDOW (window), "HIGHSCORE");
        gtk_window_set_modal (GTK_WINDOW (window), TRUE);
        gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
        gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
	gtk_window_set_default_size (GTK_WINDOW (window), 500, 400);
	
	
        buffer = gtk_text_buffer_new (NULL);
        
	gtk_text_buffer_create_tag (buffer, "not_editable",
                              "editable", FALSE, NULL);

  	gtk_text_buffer_create_tag (buffer, "word_wrap",
                              "wrap_mode", GTK_WRAP_WORD, NULL);
        
	gtk_text_buffer_create_tag (buffer, "heading",
                              "weight", PANGO_WEIGHT_BOLD,
                              "size", 15 * PANGO_SCALE,
                              NULL);
	view1 = gtk_text_view_new_with_buffer (buffer);

	sw = gtk_scrolled_window_new (NULL, NULL);
	      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
                                      GTK_POLICY_AUTOMATIC,
                                      GTK_POLICY_AUTOMATIC);

	gtk_container_add (GTK_CONTAINER (sw), view1);
	
	gtk_window_set_focus (GTK_WINDOW (window), sw);	
	
	gtk_text_buffer_get_iter_at_offset (buffer, &iter, 0);

  	gtk_text_buffer_insert (buffer, &iter,
      	"The text widget can display text with all kinds of nifty attributes. "
      	"It also supports multiple views of the same buffer; this demo is "
      	"showing the same buffer in two places.\n\n", -1);

	gtk_text_buffer_get_bounds (buffer, &start, &end);
	gtk_text_buffer_apply_tag_by_name (buffer, "word_wrap", &start, &end);
	gtk_text_buffer_apply_tag_by_name (buffer, "not_editable", &start, &end);
	
/*	gtk_text_buffer_apply_tag_by_name (buffer, "heading", &start, &end);*/
	gtk_container_add (GTK_CONTAINER (window), sw);
	
	gtk_widget_show_all (GTK_WIDGET (window));
}

void activate (GtkApplication *app, gpointer data)
{
	widgets *a = (widgets *) data;

	a->continue_timer = FALSE;
	a->start_timer = FALSE;

	a->window = gtk_application_window_new (app);
	gtk_window_set_application (GTK_WINDOW (a->window), GTK_APPLICATION (app));
	gtk_window_set_title (GTK_WINDOW (a->window), "GNOME Menu");
	gtk_window_set_position (GTK_WINDOW (a->window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size (GTK_WINDOW (a->window), 700, 660);
	gtk_window_set_resizable (GTK_WINDOW (a->window), FALSE);

	a->difficulty = 7;
	a->firstgame = 1;

	construct_menu (app, (gpointer) a);
	constructWelcomePage (app, (gpointer) a);

	gtk_widget_show_all (GTK_WIDGET (a->window));
}

void startup (GApplication *app, gpointer data)
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
