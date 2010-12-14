#ifndef FRHED_PDRIVE95_H_0BE3497DEDEB43E89828D24BCC844DAD
#define FRHED_PDRIVE95_H_0BE3497DEDEB43E89828D24BCC844DAD

#include "RAWIO32.h"
#include "physicaldrive.h"

/*
For more info on partition types, please refer to
http://www.win.tue.nl/~aeb/partitions/partition_types-1.html
*/

class P9xPartitionInfo : public PNode
{
public:
	P9xPartitionInfo(PARTITION_INFORMATION* pi):m_pi(*pi){}

	PARTITION_INFORMATION m_pi;
};

class P9xPhysicalDrive : public IPhysicalDrive
{
public:
	P9xPhysicalDrive();
	virtual ~P9xPhysicalDrive();

	//Path must look like this: "\\.\PhysicalDrive0" (of course, \ maps to \\, and \\ to \\\\)
	BOOL Open( int iDrive );
	void Close();
	BOOL GetDriveGeometry( DISK_GEOMETRY* lpDG );
	BOOL GetDriveGeometryEx( DISK_GEOMETRY_EX* lpDG, DWORD dwSize );
	BOOL GetDriveLayout( LPBYTE lpbMemory, DWORD dwSize );
	BOOL GetDriveLayoutEx( LPBYTE lpbMemory, DWORD dwSize );
	BOOL ReadAbsolute( LPBYTE lpbMemory, DWORD dwSize, INT64 Sector );
	BOOL IsOpen();
	BOOL ReadPartitionInfoRecursive(DWORD dwSector,INT64 TotalOffset,int depth = 0);

	HANDLE m_hDevice;
	BYTE m_bDriveNumber;
	PList m_PartitionInfo;
private:
	static struct RAWIO32 *RAWIO32;
};

#endif
