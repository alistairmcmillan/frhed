#!/usr/bin/python
# -*- coding: utf-8 -*-;

# Python script to create the POT file for the website
#
# Copyright 2009 Tim Gerundt <tim@gerundt.de>
#
# This file is part of Frhed. Frhed is free software under the terms of the
# GNU General Public License. You should have received a copy of the license
# along with Frhed.  If not, see <http://www.gnu.org/licenses/>.
#
# $Id$

from os import sep
from os.path import abspath, isfile, join, splitext, walk
from time import strftime
import string
import re

def getPhpFiles(path):
    ''' Get all php files from a folder and his subfolders '''
    phpfiles = []
    walk(path, walkPhpFiles, phpfiles)
    return phpfiles

def walkPhpFiles(phpfiles, dirpath, itemnames):
    ''' Helper function for getPhpFiles() '''
    for itemname in itemnames: #For all dir items...
        fullitempath = abspath(join(dirpath, itemname))
        if isfile(fullitempath): #If a file...
            filename = splitext(itemname)
            if str.lower(filename[1]) == '.php' or str.lower(filename[1]) == '.inc': #If a PHP file...
                phpfiles.append(fullitempath)

def getTranslationsFromPhpFile(php_file, translations):
    ''' Get the translations from a php file '''
    rGettext = re.compile('_e?\([\'](.*?)[\']', re.IGNORECASE)

    i = 0
    lines = open(php_file, 'r')
    for line in lines: #For all lines...
        i += 1
        tmps = rGettext.findall(line)
        if tmps: #If found a gettext function...
            for tmp in tmps: #For all translations...
                translation = tmp
                if translation in translations: #If the translation is already exists...
                    translations[translation] += [(php_file, i)]
                else: #If the translation is NOT already exists...
                    translations[translation] = [(php_file, i)]

#Main routine...
translations = {}
php_dir = abspath('../../Web')
php_files = getPhpFiles(php_dir)
for php_file in php_files: #For all php files...
    getTranslationsFromPhpFile(php_file, translations)

potfile = open('en-US.pot', 'w')
potfile.write('# This file is part from Frhed <http://frhed.sf.net/>\n')
potfile.write('# Released under the "GNU General Public License"\n')
potfile.write('#\n')
potfile.write('# ID line follows -- this is updated by SVN\n')
potfile.write('# $' + 'Id' + '$\n')
potfile.write('#\n')
potfile.write('msgid ""\n')
potfile.write('msgstr ""\n')
potfile.write('"Project-Id-Version: Frhed\\n"\n')
potfile.write('"Report-Msgid-Bugs-To: http://sourceforge.net/tracker/?atid=1100163&group_id=236485\\n"\n')
potfile.write('"POT-Creation-Date: %s\\n"\n' % strftime('%Y-%m-%d %H:%M+0000'))
potfile.write('"PO-Revision-Date: \\n"\n')
potfile.write('"Last-Translator: \\n"\n')
potfile.write('"Language-Team: English <frhed-support@lists.sourceforge.net>\\n"\n')
potfile.write('"MIME-Version: 1.0\\n"\n')
potfile.write('"Content-Type: text/plain; charset=utf-8\\n"\n')
potfile.write('"Content-Transfer-Encoding: 8bit\\n"\n')
potfile.write('"X-Poedit-Language: English\\n"\n')
potfile.write('"X-Poedit-SourceCharset: utf-8\\n"\n')
potfile.write('"X-Poedit-Basepath: ../../\\n"\n')
potfile.write('\n')
for translation in translations: #For all translations...
    references = translations[translation]
    for reference in references: #For all references...
        potfile.write('#: %s:%u\n' % (reference[0].replace(php_dir + sep, ''), reference[1]))
    potfile.write('#, c-format\n')
    potfile.write('msgid "%s"\n' % (translation.replace('"', '\\"')))
    potfile.write('msgstr ""\n')
    potfile.write('\n')
potfile.close()
