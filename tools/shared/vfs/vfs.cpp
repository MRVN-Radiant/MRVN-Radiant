/* -------------------------------------------------------------------------------

   Copyright (C) 2022-2023 MRVN-Radiant and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of MRVN-Radiant.

   MRVN-Radiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   MRVN-Radiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

   ------------------------------------------------------------------------------- */

#include "vfs.h"
#include "inout.h"

/* ------------------------------------------------------------------------------- */
// List of initalized directories
unordered_set<string> g_directories;

/* ------------------------------------------------------------------------------- */

//------------------------------------------------------------
// Purpose: Initilazes a directory
// Input  : *dir - The directory
// Output : True if this is the first time dir is initilazed
//------------------------------------------------------------
bool vfsInitDirectory( const char* dir )
{
	if( g_directories.find(dir) != g_directories.end() )
	{
		Sys_Warning( "VFS: Directory: %s has already been initilazed!\n", dir );
		return false;
	}

	g_directories.insert( dir );
	Sys_Printf( "  VFS Init: %s\n", dir );

	return true;
}

//------------------------------------------------------------
// Purpose: Checks whether a file exists
// Input  : *file
// Output : True if the file exists in one of the vfs dirs
//------------------------------------------------------------
bool vfsFileExists( const char* file )
{
	for( const string &dir : g_directories )
	{
		for( const auto &entry : fs::recursive_directory_iterator{dir} )
		{
			if( fs::path(entry).string() == file )
				return true;
		}
	}

	return false;
}

//------------------------------------------------------------
// Purpose: Returns the number of occurrences of file
// Input  : *file
// Output : The number of occurrences
//------------------------------------------------------------
int vfsGetFileOccurrences( const char* file )
{
	int iOccurences = 0;

	for( const string &dir : g_directories )
	{
		for( const auto &entry : fs::recursive_directory_iterator{dir} )
		{
			if( fs::path(entry).string() == file )
				iOccurences++;
		}
	}

	return iOccurences;
}

//------------------------------------------------------------
// Purpose: Gets a list of all files with passed extension
//          Uses directory_iterator
// Input  : *dir - 
//          *ext - Extension with the dot
// Output : Vector of all files with extension
//------------------------------------------------------------
vector<string> vfsGetListOfFiles( const char* dir, const char* ext )
{
	vector<string> files;

	for( const string &root : g_directories )
	{
		for( const auto &entry : fs::directory_iterator{fs::path(root) / dir} )
		{
			if( fs::path(entry).extension() == ext )
			{
				files.push_back( fs::relative(entry, fs::path(root) / dir).string() );
			}
		}
	}

	return files;
}

//------------------------------------------------------------
// Purpose: Gets a list of all files with passed extension
//          Uses recursive_directory_iterator
// Input  : *dir -
//          *ext - Extension with the dot
// Output : Vector of all files with extension
//------------------------------------------------------------
vector<string> vfsGetListOfFilesRecursive( const char* dir, const char* ext )
{
	vector<string> files;

	for( const string &root : g_directories )
	{
		for( const auto &entry : fs::recursive_directory_iterator{fs::path(root) / dir} )
		{
			if( fs::path(entry).extension() == ext )
			{
				files.push_back( fs::relative(entry, fs::path(root) / dir).string() );
			}
		}
	}

	return files;
}

//------------------------------------------------------------
// Purpose: Loads a file into MemBuffer
// Input  : *filename -
// Output : The loaded file
//------------------------------------------------------------
CMemBuffer vfsLoadFile( const char* filename )
{
	CMemBuffer buffer;

	for( const string &root : g_directories )
	{
		FILE *f = fopen( ( root / fs::path(filename) ).string().c_str(), "rb");
		if ( f != NULL ) {
			fseek( f, 0, SEEK_END );
			buffer = CMemBuffer( ftell( f ) );
			rewind( f );

			if ( fread( buffer.data(), 1, buffer.size(), f ) != buffer.size() ) {
					buffer = CMemBuffer();
			}
			fclose( f );
			return buffer;
		}
		else
		{
			Sys_Warning( "Failed to open file %s\n", ( root / fs::path(filename) ).string().c_str() );
		}
	}

	return CMemBuffer();
}