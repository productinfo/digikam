#!/bin/bash

# Copyright (c) 2015-2019, Gilles Caulier, <caulier dot gilles at gmail dot com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

########################################################################

# Absolute path where are downloaded all tarballs to compile.
DOWNLOAD_DIR="/digikam.appimage/download"

# Absolute path where are compiled all tarballs
BUILDING_DIR="/digikam.appimage/build"

# Absolute path where are installed all software components
INSTALL_DIR="/digikam.appimage/install"

# Setup variables needed to help everything find what we build

export PATH=$INSTALL_DIR/bin:$PATH
export LD_LIBRARY_PATH=$INSTALL_DIR/lib:$INSTALL_DIR/lib64:$LD_LIBRARY_PATH
export PKG_CONFIG_PATH=$INSTALL_DIR/share/pkgconfig:$INSTALL_DIR/lib/pkgconfig::$INSTALL_DIR/lib64/pkgconfig/usr/lib/pkgconfig:$PKG_CONFIG_PATH
export CMAKE_PREFIX_PATH=$INSTALL_DIR:$CMAKE_PREFIX_PATH

########################################################################

# URL to git repository to checkout digiKam source code
# Read/write url is an alternative when official read only server is down.
#DK_GITURL="git://anongit.kde.org/digikam.git"
DK_GITURL="git@git.kde.org:digikam"

# Location to build source code.
DK_BUILDTEMP=$BUILDING_DIR/dktemp

# digiKam tag version from git. Official tarball do not include extra shared libraries.
# The list of tags can be listed with this url: https://quickgit.kde.org/?p=digikam.git&a=tags
# If you want to package current implemntation from git, use "master" as tag.
#DK_VERSION=v6.0.0
DK_VERSION=master
#DK_VERSION=development/dplugins

# Installer sub version to differentiates newer updates of the installer itself, even if the underlying application hasn’t changed.
#DK_EPOCH="-01"
# Epoch with time-stamp for pre-release bundle in ISO format
DK_EPOCH="-`date "+%Y%m%dT%H%M%S"`"

# Installer will include or not digiKam debug symbols
DK_DEBUG=1

# Sign bundles with GPG. Passphrase must be hosted in ~/.gnupg/dkorg-gpg-pwd.txt
DK_SIGN=0

# Upload automatically bundle to files.kde.org (pre-release only).
DK_UPLOAD=1
DK_UPLOADURL="digikam@milonia.kde.org"
DK_UPLOADDIR="/srv/archives/files/digikam/"
