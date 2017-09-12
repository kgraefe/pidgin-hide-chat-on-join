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

#include "context_menu.h"

#include <gtkconv.h>

#include "util.h"

static void set_hide_on_join(PurpleBlistNode *node, gpointer data) {
	purple_blist_node_set_int(node, "hide-chat-state", GPOINTER_TO_INT(data));
}

static void extended_buddy_menu_cb(PurpleBlistNode *node, GList **menu) {
	if(!PURPLE_BLIST_NODE_IS_CHAT(node)) return;
	
	if(purple_blist_node_get_flags(node) & PURPLE_BLIST_NODE_FLAG_NO_SAVE)
		return;

	switch(purple_blist_node_get_int(node, "hide-chat-state")) {
	case HIDE_CHAT_STATE_HIDE:
		*menu = g_list_append(*menu,
			purple_menu_action_new(
				_("Do not hide on join"), PURPLE_CALLBACK(set_hide_on_join),
				GINT_TO_POINTER(HIDE_CHAT_STATE_SHOW), NULL
			)
		);
		break;

	case HIDE_CHAT_STATE_UNSET:
	case HIDE_CHAT_STATE_SHOW:
	default:
		*menu = g_list_append(*menu,
			purple_menu_action_new(
				_("Hide on join"), PURPLE_CALLBACK(set_hide_on_join),
				GINT_TO_POINTER(HIDE_CHAT_STATE_HIDE), NULL
			)
		);
	}
}

void context_menu_init(PurplePlugin *plugin) {
	purple_signal_connect(
		purple_blist_get_handle(), "blist-node-extended-menu", plugin,
		PURPLE_CALLBACK(extended_buddy_menu_cb), NULL
	);
}

void context_menu_uninit(PurplePlugin *plugin) {
	purple_signal_disconnect(
		purple_blist_get_handle(), "blist-node-extended-menu", plugin,
		PURPLE_CALLBACK(extended_buddy_menu_cb)
	);
}

