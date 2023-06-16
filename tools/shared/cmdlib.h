/*
   Copyright (C) 1999-2006 Id Software, Inc. and contributors.
   For a list of contributors, see the accompanying CONTRIBUTORS file.

   This file is part of GtkRadiant.

   GtkRadiant is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   GtkRadiant is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GtkRadiant; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#pragma once

#include "vfs/membuffer.h"

#include <cstdio>
#include <cstdlib>
#include <utility>


CVoidPtr safe_malloc( size_t size );
CVoidPtr safe_calloc( size_t size );

#define offsetof_array( TYPE, ARRAY_MEMBER, ARRAY_SIZE ) ( offsetof( TYPE, ARRAY_MEMBER[0] ) + sizeof( TYPE::ARRAY_MEMBER[0] ) * ARRAY_SIZE )

void Q_getwd( char *out );

int Q_filelength( FILE *f );

void    Q_mkdir( const char *path );

char *ExpandArg( const char *path );    // from cmd line


FILE    *SafeOpenWrite( const char *filename, const char *mode = "wb" );
FILE    *SafeOpenRead( const char *filename, const char *mode = "rb" );
void    SafeRead( FILE *f, CMemBuffer& buffer );
void    SafeWrite( FILE *f, const void *buffer, int count );

/// \brief loads file from absolute \p filename path or emits \c Error
CMemBuffer LoadFile( const char *filename );
void    SaveFile( const char *filename, const void *buffer, int count );
bool    FileExists( const char *filename );


short   BigShort( short l );
short   LittleShort( short l );
int     BigLong( int l );
int     LittleLong( int l );
float   BigFloat( float l );
float   LittleFloat( float l );


// sleep for the given amount of milliseconds
void Sys_Sleep( int n );
