plugindir = $(libdir)/pidgin
DEFS = -DLOCALEDIR=\"$(localedir)\"

plugin_LTLIBRARIES = hide_chat.la
hide_chat_la_SOURCES = \
	conversation_handler.c \
	context_menu.c \
	hide_chat.c
	

hide_chat_la_LDFLAGS = -module -avoid-version

