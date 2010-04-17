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

#ifndef PURPLE_PLUGINS
#define PURPLE_PLUGINS
#endif

#include "internal.h"
#include "util.h"

#include <plugin.h>
#include <version.h>

#include <util.h>
#include <gtkutils.h>
#include <gtkdebug.h>

#define GBOOLEAN_TO_POINTER(i) (GINT_TO_POINTER ((i) ? 2 : 1))
#define GPOINTER_TO_BOOLEAN(i) ((gboolean) ((GPOINTER_TO_INT(i) == 2) ? TRUE : FALSE))

PurplePlugin *plugin;

static void conversation_created_cb(PurpleConversation *conv) {
	PidginConversation *gtkconv;
	PurpleBlistNode *node = NULL;

	if(purple_conversation_get_type(conv) != PURPLE_CONV_TYPE_CHAT) return;

	if(!(gtkconv = PIDGIN_CONVERSATION(conv))) return;
	
	if(!(node = (PurpleBlistNode *)purple_blist_find_chat(conv->account, conv->name))) return;

	if(purple_blist_node_get_bool(node, "hide-on-join")) hide_conversation(gtkconv);

}

static void set_hide_on_join(PurpleBlistNode *node, gpointer data) {
	purple_blist_node_set_bool(node, "hide-on-join", GPOINTER_TO_BOOLEAN(data));
}

static void extended_buddy_menu_cb(PurpleBlistNode *node, GList **menu) {
	if(!PURPLE_BLIST_NODE_IS_CHAT(node)) return;
	
	if(purple_blist_node_get_flags(node) & PURPLE_BLIST_NODE_FLAG_NO_SAVE) return;

	if(purple_blist_node_get_bool(node, "hide-on-join")) {
		*menu = g_list_append(*menu, purple_menu_action_new(_("Do not hide on join"), PURPLE_CALLBACK(set_hide_on_join), GBOOLEAN_TO_POINTER(FALSE), NULL));
	} else {
		*menu = g_list_append(*menu, purple_menu_action_new(_("Hide on join"), PURPLE_CALLBACK(set_hide_on_join), GBOOLEAN_TO_POINTER(TRUE), NULL));
	}
}

static gboolean plugin_load(PurplePlugin *_plugin) {
	plugin = _plugin;
	
	purple_signal_connect(purple_blist_get_handle(), "blist-node-extended-menu", plugin, PURPLE_CALLBACK(extended_buddy_menu_cb), NULL);

	purple_signal_connect(purple_conversations_get_handle(), "conversation-created", plugin, PURPLE_CALLBACK(conversation_created_cb), NULL);

	purple_debug_info(PLUGIN_STATIC_NAME, _("loaded.\n"));
	
	return TRUE;
}

static gboolean plugin_unload(PurplePlugin *plugin) {
	purple_debug_info(PLUGIN_STATIC_NAME, _("unloaded.\n"));
	
	return TRUE;
}

static PurplePluginInfo info = {
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_STANDARD,				/**< type           */
	NULL,						/**< ui_requirement */
	0,						/**< flags          */
	NULL,						/**< dependencies   */
	PURPLE_PRIORITY_DEFAULT,			/**< priority       */

	PLUGIN_ID,					/**< id             */
	NULL,						/**< name           */
	PLUGIN_VERSION,					/**< version        */
	NULL,						/**  summary        */
				
	NULL,						/**  description    */
	PLUGIN_AUTHOR,					/**< author         */
	PLUGIN_WEBSITE,					/**< homepage       */

	plugin_load,					/**< load           */
	plugin_unload,					/**< unload         */
	NULL,						/**< destroy        */

	NULL,						/**< ui_info        */
	NULL,						/**< extra_info     */
	NULL,						/**< prefs_info     */
	NULL,						/**< actions        */
	/* padding */
	NULL,
	NULL,
	NULL,
	NULL
};

static void init_plugin(PurplePlugin *plugin) {
#ifdef ENABLE_NLS
	const char *str = "Hide Chat on Join";
	gchar *plugins_locale_dir;

	plugins_locale_dir = g_build_filename(purple_user_dir(), "locale", NULL);

	bindtextdomain(GETTEXT_PACKAGE, plugins_locale_dir);
	if(str == _(str)) {
		bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
	}
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");

	g_free(plugins_locale_dir);
#endif /* ENABLE_NLS */

        info.name        = _("Hide Chat on Join");
        info.summary     = _("This plugin enables you to hide some chats while auto-joining them.");
        info.description = _("This plugin adds an entry in the context menu of every channel you have in your buddy list to allow you to hide the chat window when you join the channel. (This is usefull when you have a huge amount of channels in your buddy list and don't want to have all of these poping up each time you start Pidgin.)");
}

PURPLE_INIT_PLUGIN(plugin, init_plugin, info)
