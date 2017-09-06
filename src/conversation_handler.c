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

#include <gtkconv.h>
#include <gtkdebug.h>

static void (*create_conversation_ori)(PurpleConversation *conv);

static void conv_placement_hidden_window_fnc(PidginConversation *gtkconv) {
	PidginWindow *win;

	win = pidgin_conv_window_new();
	pidgin_conv_window_hide(win);

	pidgin_conv_window_add_gtkconv(win, gtkconv);
}

static void create_conversation_hook(PurpleConversation *conv) {
	PurpleAccount *acc;
	PurpleBlistNode *node;
	PidginConvPlacementFunc place_ori;

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

	if(!purple_blist_node_get_bool(node, "hide-on-join")) {
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

void conversation_handler_init(void) {
	PurpleConversationUiOps *conversation_ui_ops;

	conversation_ui_ops = pidgin_conversations_get_conv_ui_ops();

	/* Let's hook into conversation between Pidgin and libpurple */
	create_conversation_ori = conversation_ui_ops->create_conversation;
	conversation_ui_ops->create_conversation = create_conversation_hook;
}

void conversation_handler_uninit(void) {
	PurpleConversationUiOps *conversation_ui_ops;

	conversation_ui_ops = pidgin_conversations_get_conv_ui_ops();

	conversation_ui_ops->create_conversation = create_conversation_ori;
}

