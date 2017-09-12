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

#ifndef _UTIL_H
#define _UTIL_H

#include <gtkconv.h>

enum HideChatState {
	HIDE_CHAT_STATE_UNSET = 0,
	HIDE_CHAT_STATE_SHOW  = 1,
	HIDE_CHAT_STATE_HIDE  = 2,
};

#define error(...)   purple_debug_error(PLUGIN_STATIC_NAME, __VA_ARGS__)
#define warning(...) purple_debug_warning(PLUGIN_STATIC_NAME, __VA_ARGS__)
#define info(...)    purple_debug_info(PLUGIN_STATIC_NAME, __VA_ARGS__)

#endif /* _UTIL_H */
