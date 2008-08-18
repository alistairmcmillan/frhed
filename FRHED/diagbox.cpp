#include "precomp.h"

//============================================================================================

#define NumBx(var) NumBox( #var , var, "NumBox" )
#define NBx(capt, var) NumBox( #var , var, capt )
void NumBox (char* varname, int x, char* caption)
{
	char buf[100];
	sprintf (buf, "%s = %d = 0x%x", varname, x, x);
	MessageBox (NULL, buf, caption, MB_OK);
}

#define TxtBx(var) TxtBox( #var , var)
void TxtBox (char* varname, char* s)
{
	char buf[100];
	sprintf (buf, "%s = %s", varname, s);
	MessageBox (NULL, buf, "TxtBox", MB_OK);
}

#define ChrBx(var) CharBox( #var , var)
void CharBox (char* varname, char c)
{
	char buf[100];
	sprintf (buf, "%s = %c", varname, c);
	MessageBox (NULL, buf, "CharBox", MB_OK);
}
