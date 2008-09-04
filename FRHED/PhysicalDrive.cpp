//    License (GPLv2+):
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but
//    WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
/////////////////////////////////////////////////////////////////////////////
/** 
 * @file  PhysicalDrive.cpp
 *
 * @brief Drive/partition information class implementations.
 *
 */
// ID line follows -- this is updated by SVN
// $Id$

#include "precomp.h"
#include "physicaldrive.h"
#include "pdrive95.h"
#include "pdrivent.h"

IPhysicalDrive *CreatePhysicalDriveInstance()
{
	OSVERSIONINFO osvi;
	ZeroMemory(&osvi, sizeof osvi);
	osvi.dwOSVersionInfoSize = sizeof osvi;
	if (!GetVersionEx(&osvi))
		osvi.dwPlatformId = 0;
	switch (osvi.dwPlatformId)
	{
	case VER_PLATFORM_WIN32_NT:
		return new PNtPhysicalDrive;
	case VER_PLATFORM_WIN32_WINDOWS:
		return new P9xPhysicalDrive;
	}
	return 0;
}

LPTSTR PartitionInfo::GetNameAsString(LPTSTR szFormat)
{
	if (m_bIsPartition)
		_stprintf(szFormat, _T("Drive %d, Partition %d (%s)"), m_dwDrive + 1, m_dwPartition + 1, GetSizeAsString());
	else
		_stprintf(szFormat, _T("Drive %d (%s)"), m_dwDrive + 1, GetSizeAsString());
	return szFormat;
}

LPTSTR PartitionInfo::GetSizeAsString(LPTSTR szFormat)
{
	double SizeInMB = (double) m_PartitionLength;
	SizeInMB /= (1000*1000);
	double SizeInGB = SizeInMB;
	SizeInGB /= (1000);
	if( SizeInGB >= 1.0 )
		_stprintf(szFormat, _T("%.2f GB"), SizeInGB);
	else
		_stprintf(szFormat, _T("%.2f MB"), SizeInMB);
	return szFormat;
}

void IPhysicalDrive::GetPartitionInfo(PList* lpList)
{
	lpList->DeleteContents();

	BYTE bLayoutInfo[20240];
	DISK_GEOMETRY dg;

	for( int iDrive = 0; iDrive < 8; iDrive++ )
	{
		if( !Open(iDrive) )
			continue;

		if( GetDriveGeometryEx( (DISK_GEOMETRY_EX*) bLayoutInfo, sizeof(bLayoutInfo) ) )
		{
			DISK_GEOMETRY& dgref = (((DISK_GEOMETRY_EX*)bLayoutInfo)->Geometry);
			dg = dgref;
			PartitionInfo* p = new PartitionInfo();
			p->m_dwDrive = (DWORD) iDrive;
			p->m_dwPartition = 0;
			p->m_bIsPartition = TRUE;
			p->m_dwBytesPerSector = dg.BytesPerSector;
			p->m_NumberOfSectors = dg.Cylinders.QuadPart;
			p->m_NumberOfSectors *= dg.SectorsPerTrack;
			p->m_NumberOfSectors *= dg.TracksPerCylinder;
			p->m_StartingOffset = 0;
			p->m_StartingSector = 0;
			p->m_PartitionLength = p->m_NumberOfSectors;
			p->m_PartitionLength *= dg.BytesPerSector;

			lpList->AddTail(p);
			if( GetDriveLayoutEx( bLayoutInfo, sizeof(bLayoutInfo) ) )
			{
				PDRIVE_LAYOUT_INFORMATION_EX pLI = (PDRIVE_LAYOUT_INFORMATION_EX)bLayoutInfo;
				for( DWORD iPartition = 0; iPartition < pLI->PartitionCount; iPartition++ )
				{
					PARTITION_INFORMATION_EX* pi = &(pLI->PartitionEntry[iPartition]);

					PartitionInfo* p = new PartitionInfo();
					p->m_dwDrive = (DWORD) iDrive;
					p->m_dwPartition = (DWORD) iPartition;
					p->m_bIsPartition = TRUE;
					p->m_dwBytesPerSector = dg.BytesPerSector;
					p->m_NumberOfSectors = pi->PartitionLength.QuadPart;
					p->m_NumberOfSectors /= dg.BytesPerSector;
					p->m_StartingOffset = pi->StartingOffset.QuadPart;
					p->m_StartingSector = p->m_StartingOffset;
					p->m_StartingSector /= dg.BytesPerSector;
					p->m_PartitionLength = pi->PartitionLength.QuadPart;
					lpList->AddTail(p);
				}
			}
		}
		else
		{
			if( GetDriveGeometry( &dg ) )
			{
				PartitionInfo* p = new PartitionInfo();
				p->m_dwDrive = (DWORD) iDrive;
				p->m_dwPartition = 0;
				p->m_bIsPartition = FALSE;
				p->m_dwBytesPerSector = dg.BytesPerSector;
				p->m_NumberOfSectors = dg.Cylinders.QuadPart;
				p->m_NumberOfSectors *= dg.SectorsPerTrack;
				p->m_NumberOfSectors *= dg.TracksPerCylinder;
				p->m_StartingOffset = 0;
				p->m_StartingSector = 0;
				p->m_PartitionLength = p->m_NumberOfSectors;
				p->m_PartitionLength *= dg.BytesPerSector;

				lpList->AddTail(p);

				if( GetDriveLayout( bLayoutInfo, sizeof(bLayoutInfo) ) )
				{
					PDRIVE_LAYOUT_INFORMATION pLI = (PDRIVE_LAYOUT_INFORMATION)bLayoutInfo;
					for( DWORD iPartition = 0; iPartition < pLI->PartitionCount; iPartition++ )
					{
						PARTITION_INFORMATION* pi = &(pLI->PartitionEntry[iPartition]);

						if( !pi->PartitionLength.QuadPart )
							continue;

						PartitionInfo* p = new PartitionInfo();
						p->m_dwDrive = (DWORD) iDrive;
						p->m_dwPartition = (DWORD) iPartition;
						p->m_bIsPartition = TRUE;
						p->m_dwBytesPerSector = dg.BytesPerSector;
						p->m_NumberOfSectors = pi->PartitionLength.QuadPart;
						p->m_NumberOfSectors /= dg.BytesPerSector;
						p->m_StartingOffset = pi->StartingOffset.QuadPart;
						p->m_StartingSector = p->m_StartingOffset;
						p->m_StartingSector /= dg.BytesPerSector;
						p->m_PartitionLength = pi->PartitionLength.QuadPart;
						lpList->AddTail(p);
					}
				}
			}
		}
		Close();
	}
}
