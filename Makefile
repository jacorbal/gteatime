# Makefile
#
# Program: `gteatime`
# Description: Lightweight GTK3 system-tray tea and coffee timer
# Author: J. A. Corbal <jacorbal@gmail.com>
# Last update: Sat May  9 00:00 UTC 2026

OBJ_D = obj
BIN_D = bin
INC_D = include
SRC_D = src
ICO_D ?= icons

# Program name
PROGRAM = gteatime

PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
DSTDIR ?=

# Icons directory
ICON_THEME ?= hicolor
ICONDIR ?= $(PREFIX)/share/icons/$(ICON_THEME)/scalable/apps
ICONS = \
	gteatime.svg \
	gteatime-on.svg \
	gteatime-off.svg

# Desktop entry
DESKTOPDIR ?= $(PREFIX)/share/applications
DESKTOP_FILE = gteatime.desktop

# Options
CC = cc
CSTD = -std=c99
COPTS = --pedantic --pedantic-errors
WARNINGS = -Wall -Wextra -Wpedantic -Werror=declaration-after-statement

CFLAGS = $(CSTD) $(WARNINGS) -I $(INC_D)
LDFLAGS = -s

# Needed flags
PKG_CONFIG = pkgconf
PKGS = gtk+-3.0 gio-2.0 glib-2.0 gobject-2.0 x11
PKG_CFLAGS = `$(PKG_CONFIG) --cflags $(PKGS)`
PKG_LIBS = `$(PKG_CONFIG) --libs $(PKGS)`

# Sources and objects
SRCS = \
	$(SRC_D)/main.c \
	$(SRC_D)/app_state.c \
	$(SRC_D)/notify.c \
	$(SRC_D)/presets.c \
	$(SRC_D)/timer.c \
	$(SRC_D)/tray.c \
	$(SRC_D)/window.c
OBJS = $(SRCS:$(SRC_D)/%.c=$(OBJ_D)/%.o)


# Makefile options
$(BIN_D)/$(PROGRAM): $(OBJS)
	@mkdir -pv $(BIN_D)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(PKG_LIBS)

$(OBJ_D)/%.o: $(SRC_D)/%.c
	@mkdir -pv $(OBJ_D)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(PKG_CFLAGS) -c $< -o $@

all: $(BIN_D)/$(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(COPTS) $(LDFLAGS) -o $(PROGRAM) $(OBJS) $(PKG_LIBS)

.c.o:
	$(CC) $(COPTS) $(CFLAGS) $(PKG_CFLAGS) -c $< -o $@

clean-bin:
	@rm -rfv $(BIN_D)

clean-obj:
	@rm -rfv $(OBJ_D)

clean: clean-obj clean-bin

install-bin: all
	@mkdir -pv $(DSTDIR)$(BINDIR)
	@cp -v $(BIN_D)/$(PROGRAM) $(DSTDIR)$(BINDIR)/$(PROGRAM)
	@chmod -v 0755 $(DSTDIR)$(BINDIR)/$(PROGRAM)

install-icons:
	@mkdir -pv $(DSTDIR)$(ICONDIR)
	@for icon in $(ICONS); do \
		if [ ! -f "$(ICO_D)/$$icon" ]; then \
			echo "Error: Icon '$(ICO_D)/$$icon' not found"; \
			exit 1; \
		fi; \
		cp -v $(ICO_D)/$$icon $(DSTDIR)$(ICONDIR)/$$icon; \
	done
	@if command -v gtk-update-icon-cache >/dev/null 2>&1; then \
		gtk-update-icon-cache -f \
            -t $(DSTDIR)$(PREFIX)/share/icons/$(ICON_THEME); \
	fi

install-desktop:
	@mkdir -pv $(DSTDIR)$(DESKTOPDIR)
	@if [ ! -f "$(DESKTOP_FILE)" ]; then \
		echo "Error: Desktop file '$(DESKTOP_FILE)' not found"; \
		exit 1; \
	fi
	@cp -v $(DESKTOP_FILE) $(DSTDIR)$(DESKTOPDIR)/$(DESKTOP_FILE)
	@chmod -v 0644 $(DSTDIR)$(DESKTOPDIR)/$(DESKTOP_FILE)

uninstall-bin:
	@rm -fv $(DSTDIR)$(BINDIR)/$(PROGRAM)

uninstall-icons:
	@for icon in $(ICONS); do \
		rm -fv $(DSTDIR)$(ICONDIR)/$$icon; \
	done
	@if command -v gtk-update-icon-cache >/dev/null 2>&1; then \
		gtk-update-icon-cache -f \
            -t $(DSTDIR)$(PREFIX)/share/icons/$(ICON_THEME); \
	fi

uninstall-desktop:
	@rm -fv $(DSTDIR)$(DESKTOPDIR)/$(DESKTOP_FILE)

install: install-bin install-icons install-desktop

uninstall: uninstall-bin uninstall-icons uninstall-desktop

help:
	@echo "OPTIONS:"
	@echo "  make all              Build project"
	@echo "  make clean-obj        Remove object directory '$(OBJ_D)'"
	@echo "  make clean-bin        Remove binary directory '$(BIN_D)'"
	@echo "  make install          Install binary, icons, and desktop entry"
	@echo "  make uninstall        Remove binary, icons, and desktop entry"
	@echo "  make install-bin      Install only the binary"
	@echo "  make install-icons    Install only the icons"
	@echo "  make install-desktop  Install only the desktop entry"
	@echo
	@echo "  Change installation point by using 'PREFIX' and 'BINDIR':"
	@echo "      e.g.,   make install PREFIX=/usr"
	@echo "      installs the binary in '/usr/bin' instead."


.PHONY: all clean clean-bin clean-obj help \
        install-bin install-icons install-desktop uninstall-bin \
        uninstall-icons uninstall-desktop install uninstall
