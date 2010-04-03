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

#include "util.h"

#include <debug.h>
#include <gtkconv.h>


static PidginWindow *hidden_convwin = NULL;

/*
 * This is REALLY ugly...
 * If you know a better way to get hidden_convwin and/or hiding conversations, please tell me!
 */
static gboolean get_hidden_convwin(PidginConversation *gtkconv) {
	PurpleConversation *conv, *hidden_im;
	const gchar *old_value;

	conv = gtkconv->active_conv;
	if(!conv || !conv->account || !conv->account->gc) return FALSE;

	old_value = purple_prefs_get_string(PIDGIN_PREFS_ROOT "/conversations/im/hide_new");
	purple_prefs_set_string(PIDGIN_PREFS_ROOT "/conversations/im/hide_new", "always");

	serv_got_im(conv->account->gc, "RobinHood", "dummy", PURPLE_MESSAGE_NO_LOG, 0);

	/* now Pidgin is running...and running..and finally it created a hidden conversation \o/ */

	purple_prefs_set_string(PIDGIN_PREFS_ROOT "/conversations/im/hide_new", old_value);

	hidden_im = purple_find_conversation_with_account(PURPLE_CONV_TYPE_IM, "RobinHood", conv->account);

	if(!PIDGIN_IS_PIDGIN_CONVERSATION(hidden_im)) return FALSE;

	hidden_convwin = pidgin_conv_get_window(PIDGIN_CONVERSATION(hidden_im));

	/* clean up */
	purple_conversation_destroy(hidden_im);

	return TRUE;
}

void hide_conversation(PidginConversation *gtkconv) {
	if(!hidden_convwin && !get_hidden_convwin(gtkconv)) return;

        purple_signal_emit(pidgin_conversations_get_handle(), "conversation-hiding", gtkconv);

	pidgin_conv_window_remove_gtkconv(gtkconv->win, gtkconv);
	pidgin_conv_window_add_gtkconv(hidden_convwin, gtkconv);
}
