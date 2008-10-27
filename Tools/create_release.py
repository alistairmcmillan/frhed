#
# The MIT License
# Copyright (c) 2007-2008 Kimmo Varis
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files
# (the "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# $Id$

# This script prepares a Frhed release
# Tasks it does:
# - cleans previous build files from folders
# - sets version number for resources
# - builds libraries (ravio32, heksedit)
# - builds Frhed.exe
# - builds user manual
# - creates per-version distribution folder
# - exports SVN sources to distribution folder
# - creates binary distribution folder

#Tasks not done (TODO?):
# - creating packages from source and binary folders
# - creating installer
# - running virus check
# - creating SHA-1 hashes for distributed files

# Tools needed:
# - Python 2.5 :)
# - Subversion command line binaries

# CONFIGURATION:
# Set these variables to match your environment and folders you want to use

# Subversion binary - set this to absolute path to svn.exe
svn_binary = 'C:\\Program Files\\Subversion\\bin\\svn.exe'
# Visual Studio path
vs_path = 'C:\\Program Files\\Microsoft Visual Studio .NET 2003'
# NSIS installation folder
nsis_path = 'C:\\Program Files\\NSIS'
# Relative path where to create a release folder
dist_root_folder = 'distrib'

# END CONFIGURATION - you don't need to edit anything below...

from subprocess import *
import os
import os.path
import sys
import getopt
import shutil
import SetVersions

def get_vs_ide_bin():
    """Gets a full path to the Visual Studio IDE executable to run."""

    # These are identical for VS2003.Net, VS2005 and VS2008
    rel_path = 'Common7/IDE'
    vs_bin = 'devenv.com'

    vs_ide_path = os.path.join(vs_path, rel_path)
    vs_cmd_path = os.path.join(vs_ide_path, vs_bin)
    return vs_cmd_path

def cleanup_build():
    """Deletes all build files around folder structure"""

    print 'Delete old build files...'
    frhed_temp = 'BuildTmp'
    if os.path.exists(frhed_temp):
        print 'Remove folder %s' % frhed_temp
        shutil.rmtree(frhed_temp, True)
    else:
        print 'Skipping folder %s' % frhed_temp
    
    try:
        print 'Remove files'
        if os.path.exists('build/release/Frhed.exe'):
            os.remove('build/release/Frhed.exe')
        if os.path.exists('build/release/heksedit.dll'):
            os.remove('build/release/heksedit.dll')
        if os.path.exists('build/release/RAWIO32.dll'):
            os.remove('build/release/RAWIO32.dll')
        if os.path.exists('build/release/frhed.chm'):
            os.remove('build/release/frhed.chm')

        if os.path.exists('build/release/heksedit.lng'):
            shutil.rmtree('build/release/heksedit.lng', True)
        if os.path.exists('build/Help'):
            shutil.rmtree('build/Help', True)

    except EnvironmentError, einst:
        print 'Error deleting files: '
        print einst
        return False;
    except:
        print 'Error deleting files: '
        print sys.exc_info()[0]
        return False
    return True

def set_resource_version(file):
    """Sets the version number to the resource."""

    print 'Update version number to resource(s)...'
    SetVersions.process_versions(file)

# def setup_translations():
#     """Updates translation files by running scripts in Src/Languages."""
#
#     # Scripts must be run from the directory where they reside
#     curdir = os.getcwd()
#     os.chdir('Src/Languages')
#     call(['cscript', '/nologo', 'CreateMasterPotFile.vbs'])
#     call(['cscript', '/nologo', 'UpdatePoFilesFromPotFile.vbs'])
#     os.chdir(curdir)

def get_and_create_dist_folder(folder):
    """Formats a folder name for version-specific distribution folder
    and creates the folder."""

    abs_folder = os.path.realpath(dist_root_folder)
    dist_folder = os.path.join(abs_folder, folder)
    if os.path.exists(dist_folder):
        print 'Folder: ' + dist_folder + ' already exists!'
        print 'If you want to re-create this version, remove folder first!'
        return ''
    else:
        print 'Create distribution folder: ' + dist_folder
        os.mkdir(dist_folder)
        return dist_folder

def get_src_dist_folder(dist_folder, folder):
    """Format a source distribution folder path."""

    dist_src = os.path.join(dist_folder, folder + '-src')
    return dist_src

def svn_export(dist_src_folder):
    """Exports sources to distribution folder."""

    print 'Exporting sources to ' + dist_src_folder
    call([svn_binary, 'export', '--non-interactive', '.', dist_src_folder])

def build_libraries():
    """Builds library targets: rawio32 and heksedit."""

    vs_cmd = get_vs_ide_bin()
    cur_path = os.getcwd()

    print 'Build rawio32 library...'
    solution_path = os.path.join(cur_path, 'rawio32/RAWIO32.vcproj')
    #print solution_path
    call([vs_cmd, solution_path, '/rebuild', 'Release'], shell=True)

    print 'Build heksedit library...'
    solution_path = os.path.join(cur_path, 'FRHED/heksedit.vcproj')
    #print solution_path
    call([vs_cmd, solution_path, '/rebuild', 'Release'], shell=True)

def build_targets():
    """Builds all Frhed targets."""

    build_libraries()

    vs_cmd = get_vs_ide_bin()
    build_frhed(vs_cmd)

def build_frhed(vs_cmd):
    """Builds Frhed executable target."""

    cur_path = os.getcwd()
    solution_path = os.path.join(cur_path, 'FRHED/frhed.vcproj')
    #print sol_path

    print 'Build Frhed executable...'
    call([vs_cmd, solution_path, '/rebuild', 'Release'], shell=True)

def build_manual():
    """Builds manual's HTML Help (CHM) version for user install and
    HTML version for the Web. HTML version is created with ads."""

    curdir = os.getcwd()
    os.chdir('HELP')
    print 'Build HTML Help (CHM) manual...' 
    call(['build.bat'])
    
    # HTML manual not build in trunk.
    #print 'Build HTML manual for Web with ads...'
    #call(['build_html.bat', 'withads'])
    print 'Manual build finished.'
    os.chdir(curdir)

def build_nsis_installer():
    """Builds the NSIS installer for the Frhed."""

    nsis_exe = os.path.join(nsis_path, 'makensis')
    cur_path = os.getcwd()
    frhed_nsi = os.path.join(cur_path, '../Installer/NSIS/frhed.nsi')

    call([nsis_exe, frhed_nsi], shell=True)

def get_and_create_bin_folder(dist_folder, folder):
    """Formats and creates binary distribution folder."""

    bin_folder = os.path.join(dist_folder, folder + '-exe')
    print 'Create binary distribution folder: ' + bin_folder
    os.mkdir(bin_folder)
    return bin_folder

def create_bin_folders(bin_folder, dist_src_folder):
    """Creates binary distribution folders."""

    cur_path = os.getcwd()
    os.chdir(bin_folder)
    print 'Create binary distribution folder structure...'
    #lang_folder = os.path.join(bin_folder, 'Languages')
    #os.mkdir(lang_folder)
    doc_folder = os.path.join(bin_folder, 'Docs')
    os.mkdir(doc_folder)
    os.chdir(cur_path)

    print 'Copying files to binary distribution folder...'
    shutil.copy('build/release/Frhed.exe', bin_folder)

    shutil.copy('build/release/heksedit.dll', bin_folder)
    shutil.copy('build/release/RAWIO32.dll', bin_folder)
    shutil.copy('build/help/frhed.chm', bin_folder)

    #copy_po_files(lang_folder)

    shutil.copy('Docs/ChangeLog.txt', doc_folder)
    shutil.copy('Docs/Contributors.txt', doc_folder)
    shutil.copy('Docs/History.txt', doc_folder)
    shutil.copy('Installer/NSIS/GPL.txt', doc_folder)

def copy_po_files(dest_folder):
    """Copies all PO files to destination folder."""

    lang_folder = 'Src/Languages'
    files = os.listdir(lang_folder)

    print 'Copying PO files to binary folder...'
    for cur_file in files:
        fullpath = os.path.join(lang_folder, cur_file)
        if os.path.isfile(fullpath):
            file_name, file_ext = os.path.splitext(cur_file)
            if (file_ext == '.po'):
                shutil.copy(fullpath, dest_folder)

def find_frhed_root():
    """Find Frhed tree root folder from where to run rest of the script."""
    
    # If we find FRHED and Installer -subfolders we are in root 
    if os.path.exists('FRHED') and os.path.exists('Installer'):
        return True
    
    # Check if we are in /Tools
    if os.path.exists('../FRHED') and os.path.exists('../Installer'):
        os.chdir('../')
        return True
    
    return False

def check_tools():
    """Check that needed external tools can be found."""

    if not os.path.exists(svn_binary):
        print 'Subversion binary could not be found from:'
        print svn_binary
        print 'Please check script configuration and/or make sure Subversion is installed.'
        return False

    vs_cmd = get_vs_ide_bin()
    if not os.path.exists(vs_cmd):
        print 'Cannot find Visual Studio IDE binary from:'
        print vs_cmd
        print 'Please check script configuration.'
        return False
    return True

def usage():
    print 'WinMerge release script.'
    print 'Usage: create_release [-h] [-v: n] [-c] [-l]'
    print '  where:'
    print '    -h, --help print this help'
    print '    -v: n, --version= n set release version'
    print '    -c, --cleanup clean up build files (temp files, libraries, executables)'
    print '    -l, --libraries build libraries (expat, scew, pcre) only'
    print '  For example: create_release -v: 2.7.7.1'

def main(argv):
    version = '0.0.0.0'
    ver_file = ''
    if len(argv) > 0:
        opts, args = getopt.getopt(argv, "hclv:f:", [ "help", "cleanup", "libraries",
                                                    "version=", "file="])
        
        for opt, arg in opts:
            if opt in ("-h", "--help"):
                usage()
                sys.exit()
            if opt in ("-v", "--version"):
                version = arg
                print "Start building WinMerge release version " + version
            if opt in ("-c", "--cleanup"):
                if cleanup_build() == True:
                    print 'Cleanup done.'
                sys.exit()
            if opt in ("-l", "--libraries"):
                build_libraries()
                sys.exit()
            if opt in ("-f", "--file"):
                ver_file = arg

    # Check all required tools are found (script configuration)
    if check_tools() == False:
        sys.exit()

    # Check we are running from correct folder (and go to root if found)
    if find_frhed_root() == False:
        print 'ERROR: Cannot find Frhed root folder!'
        print 'The script must be run from Frhed tree\'s root folder'
        print '(which has FRHED- and Installer -folders as subfolders) or from'
        print 'Tools -folder (where this script is located).'
        sys.exit()

    # Create the distribution folder if it doesn't exist
    try:
        if not os.path.exists(dist_root_folder):
            os.mkdir(dist_root_folder)
    except EnvironmentError, einst:
        print 'Error creating distribution folder: ' + dist_root_folder
        print einst
        sys.exit()

    # Remove old build's files
    if cleanup_build() == False:
        sys.exit()

    if len(ver_file) > 0:
        set_resource_version(ver_file)

    version_folder = 'Frhed-' + version
    dist_folder = get_and_create_dist_folder(version_folder)
    if dist_folder == '':
        sys.exit(1)
    dist_src_folder = get_src_dist_folder(dist_folder, version_folder)
    svn_export(dist_src_folder)

    #setup_translations()
    
    build_targets()
    build_manual()
    build_nsis_installer()

    dist_bin_folder = get_and_create_bin_folder(dist_folder, version_folder)
    create_bin_folders(dist_bin_folder, dist_src_folder)

    # runtimes_folder = get_and_create_runtimes_folder(dist_folder, version)
    # create_runtime_folder(runtimes_folder)

    print 'Frhed release script ready!'


### MAIN ###
if __name__ == "__main__":
    main(sys.argv[1:])
