/*
 * Pidgin Hide Chat on Join
 * Copyright (C) 2010-2017 Konrad Gräfe <konradgraefe@aol.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02111-1301, USA.
 */

#include "config.h"
#include "internal.h"
#include "util.h"

#include "conversation_handler.h"

#include <gtkblist.h>
#include <gtkconv.h>
#include <gtkutils.h>
#include <gtkdebug.h>
#include <pidginstock.h>

static void (*create_conversation_ori)(PurpleConversation *conv);
static GHashTable *gcStartTimes = NULL;
static GList *miniDialogs = NULL;

static void gc_signed_on_cb(PurpleConnection *gc) {
	g_hash_table_insert(gcStartTimes, gc, GINT_TO_POINTER(time(0)));
}
static void gc_signed_off_cb(PurpleConnection *gc) {
	g_hash_table_remove(gcStartTimes, gc);
}

static void conv_placement_hidden_window_fnc(PidginConversation *gtkconv) {
	PidginWindow *win;

	win = pidgin_conv_window_new();
	pidgin_conv_window_hide(win);

	pidgin_conv_window_add_gtkconv(win, gtkconv);
}

static void mini_dialog_show_chat_cb(PurpleConversation *conv) {
	PurpleAccount *acc;
	PurpleBlistNode *node = NULL;

	acc = purple_conversation_get_account(conv);
	if(acc) {
		node = (PurpleBlistNode *)purple_blist_find_chat(acc, conv->name);
	}

	if(node) {
		purple_blist_node_set_int(node,
			"hide-chat-state", HIDE_CHAT_STATE_SHOW
		);
	}

	purple_conversation_present(conv);
}
static void mini_dialog_destroyed_cb(GtkWidget *w, GtkWidget **p) {
	miniDialogs = g_list_remove(miniDialogs, w);
}
static void mini_dialog_destroy(gpointer p, gpointer user_data) {
	gtk_widget_destroy((GtkWidget *)p);
}

static void create_conversation_hook(PurpleConversation *conv) {
	PurpleAccount *acc;
	PurpleBlistNode *node;
	PidginConvPlacementFunc place_ori;
	int gcStartTime;
	double gcDuration;
	GtkWidget *miniDialog;
	gchar *msg;

	if(purple_conversation_get_type(conv) != PURPLE_CONV_TYPE_CHAT) {
		goto show_conversation;
	}

	acc = purple_conversation_get_account(conv);
	if(!acc) {
		goto show_conversation;
	}

	node = (PurpleBlistNode *)purple_blist_find_chat(acc, conv->name);
	if(!node) {
		warning(
			"Chat %s on %s (%s) is not in the buddy list and will not be hidden.\n",
			conv->name,
			purple_account_get_username(acc),
			purple_account_get_protocol_name(acc)
		);
		goto show_conversation;
	}

	gcStartTime = GPOINTER_TO_INT(g_hash_table_lookup(gcStartTimes, acc->gc));
	gcDuration = difftime(time(0), gcStartTime);
	if(gcDuration > 2.0) {
		goto show_conversation;
	}

	switch(purple_blist_node_get_int(node, "hide-chat-state")) {
	case HIDE_CHAT_STATE_SHOW:
		goto show_conversation;

	case HIDE_CHAT_STATE_UNSET:
		msg = g_strdup_printf(
			/* Translators: The first parameter is the name of the chat, second
			 *              is the name of the account and third is the name of
			 *              the protocol.
			 */
			_("%s on %s (%s) has been hidden the first time.\n\nHow to proceed in the future?"),
			conv->name,
			purple_account_get_username(acc),
			purple_account_get_protocol_name(acc)
		);
		miniDialog = pidgin_make_mini_dialog(
			NULL, PIDGIN_STOCK_DIALOG_QUESTION,
			_("Hide Chat on Join"), msg,
			conv,
			_("Hide"), NULL,
			_("Show"), PURPLE_CALLBACK(mini_dialog_show_chat_cb),
			NULL
		);
		free(msg);

		g_signal_connect(
			G_OBJECT(miniDialog), "destroy",
			G_CALLBACK(mini_dialog_destroyed_cb), &miniDialog
		);
		pidgin_blist_add_alert(miniDialog);
		purple_blist_set_visible(TRUE);

		miniDialogs = g_list_append(miniDialogs, miniDialog);
		
		purple_blist_node_set_int(node, "hide-chat-state", HIDE_CHAT_STATE_HIDE);
		break;

	case HIDE_CHAT_STATE_HIDE:
		break;

	default:
		error(
			"Unknown state %d for chat %s on %s (%s)\n",
			purple_blist_node_get_int(node, "hide-chat-state"), conv->name,
			purple_account_get_username(acc),
			purple_account_get_protocol_name(acc)
		);
		goto show_conversation;
	}


	info(
		"Hiding chat %s on %s (%s)\n",
		conv->name,
		purple_account_get_username(acc),
		purple_account_get_protocol_name(acc)
	);

	/* In order to avoid flickering, we place the new conversation in a new
	 * hidden conversation window. Therefore, we need to register our own
	 * placement function. Unfortunately that does not work if the user
	 * deactivated tabs in the settings.
	 */
	place_ori = pidgin_conv_placement_get_current_func();
	pidgin_conv_placement_set_current_func(conv_placement_hidden_window_fnc);
	if(!pidgin_conv_placement_get_current_func()) {
		warning("Activate tabs to avoid flickering!\n");
	}

	/* Actually create conversation */
	create_conversation_ori(conv);

	/* Reset to previous placement function */
	pidgin_conv_placement_set_current_func(place_ori);

	/* Make sure to set the chat persistent. Otherwise it would be parted when
	 * closing it automatically.
	 */
	if(!purple_blist_node_get_bool(node, "gtk-persistent")) {
		purple_blist_node_set_bool(node, "gtk-persistent", TRUE);
	}

	/* "Close" persistent chat to get it moved over in Pidgin's hidden
	 * conversation window
	 */
	gtk_button_clicked(GTK_BUTTON(PIDGIN_CONVERSATION(conv)->close));

	return;

show_conversation:
	create_conversation_ori(conv);
	return;
}

void conversation_handler_init(PurplePlugin *plugin) {
	PurpleConversationUiOps *conversation_ui_ops;

	gcStartTimes = g_hash_table_new(NULL, NULL);

	purple_signal_connect(
		purple_connections_get_handle(), "signed-on",
		plugin, PURPLE_CALLBACK(gc_signed_on_cb), NULL
	);
	purple_signal_connect(
		purple_connections_get_handle(), "signed-off",
		plugin, PURPLE_CALLBACK(gc_signed_off_cb), NULL
	);

	conversation_ui_ops = pidgin_conversations_get_conv_ui_ops();

	/* Let's hook into conversation between Pidgin and libpurple */
	create_conversation_ori = conversation_ui_ops->create_conversation;
	conversation_ui_ops->create_conversation = create_conversation_hook;
}

void conversation_handler_uninit(PurplePlugin *plugin) {
	PurpleConversationUiOps *conversation_ui_ops;

	conversation_ui_ops = pidgin_conversations_get_conv_ui_ops();
	conversation_ui_ops->create_conversation = create_conversation_ori;

	purple_signal_disconnect(
		purple_connections_get_handle(), "signed-on",
		plugin, PURPLE_CALLBACK(gc_signed_on_cb)
	);
	purple_signal_disconnect(
		purple_connections_get_handle(), "signed-off",
		plugin, PURPLE_CALLBACK(gc_signed_off_cb)
	);

	g_list_foreach(miniDialogs, mini_dialog_destroy, NULL);
	g_list_free(miniDialogs);

	g_hash_table_destroy(gcStartTimes);
}

