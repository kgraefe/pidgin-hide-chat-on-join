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

#include "conversation_handler.h"

#include <gtkconv.h>
#include <gtkdebug.h>

static void (*create_conversation_ori)(PurpleConversation *conv);

void conv_placement_fnc(PidginConversation *gtkconv) {
	PidginWindow *win;

	/* make a hidden conversation window, it will be destroyed soon */
	win = pidgin_conv_window_new();
	pidgin_conv_window_hide(win);

	pidgin_conv_window_add_gtkconv(win, gtkconv);
}

static void create_conversation_hook(PurpleConversation *conv) {
	PurpleBlistNode *node = NULL;
	PidginConvPlacementFunc place_ori;

	if(
		purple_conversation_get_type(conv) == PURPLE_CONV_TYPE_CHAT && 				/* is it a chat? */
		(node = (PurpleBlistNode *)purple_blist_find_chat(conv->account, conv->name)) &&	/* is it on our buddy list? */
		purple_blist_node_get_bool(node, "hide-on-join")					/* does it have to be hidden? */
	) {
		/* hide it! */

		/* Let's register our own placement function to place the conversation in a hidden conversation window to avoid flickering */
		place_ori = pidgin_conv_placement_get_current_func();
		pidgin_conv_placement_set_current_func(conv_placement_fnc);

		if(!pidgin_conv_placement_get_current_func()) {
			purple_debug_warning(PLUGIN_STATIC_NAME, _("Activate tabs to avoid flickering!\n"));
		}

		create_conversation_ori(conv);

		pidgin_conv_placement_set_current_func(place_ori);

		/* You really don't want to close chats immediatly that aren't persistent.... */
		if(!purple_blist_node_get_bool(node, "gtk-persistent")) {
			purple_blist_node_set_bool(node, "gtk-persistent", TRUE);
		}

		/* "close" persistent chat to get it moved over in Pidgin's hidden conversation window */
		gtk_button_clicked(GTK_BUTTON(PIDGIN_CONVERSATION(conv)->close));
	} else {
		create_conversation_ori(conv);
	}
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

