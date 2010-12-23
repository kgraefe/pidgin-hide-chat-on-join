/*
 * Pidgin Hide Chat on Join
 * Copyright (C) 2010 Konrad Gräfe
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

static void conversation_created_cb(PurpleConversation *conv) {
	PidginConversation *gtkconv;
	PurpleBlistNode *node = NULL;

	if(purple_conversation_get_type(conv) != PURPLE_CONV_TYPE_CHAT) return;

	if(!(gtkconv = PIDGIN_CONVERSATION(conv))) return;
	
	if(!(node = (PurpleBlistNode *)purple_blist_find_chat(conv->account, conv->name))) return;

	if(purple_blist_node_get_bool(node, "hide-on-join")) {
		/* You really don't want to close chats immediatly that aren't persistent.... */
		if(!purple_blist_node_get_bool(node, "gtk-persistent")) {
			purple_blist_node_set_bool(node, "gtk-persistent", TRUE);
		}

		gtk_button_clicked(GTK_BUTTON(gtkconv->close));
	}

}

void conversation_handler_init(PurplePlugin *plugin) {
	purple_signal_connect(purple_conversations_get_handle(), "conversation-created", plugin, PURPLE_CALLBACK(conversation_created_cb), NULL);
}



