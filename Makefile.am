CPPFLAGS = -I./libopentime

SUBDIRS = fmodgetver libopentime src

EXTRA_DIST = release/data release/create_linux_release.sh build/linux/reconf.sh libopenmusic INSTALL.linux

install-data-local:
	cp -f -r $(abs_top_srcdir)/release/data $(pkgdatadir); \
	chmod -R 755 $(pkgdatadir)

uninstall-local:
	rm -f -r $(pkgdatadir) 
