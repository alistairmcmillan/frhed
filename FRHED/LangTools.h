class LangArray;
extern LangArray langArray;

void NTAPI TranslateDialog(HWND);
INT_PTR NTAPI ShowModalDialog(UINT, HWND, DLGPROC, LPVOID);
void NTAPI SetWindowText(HWND, PCWSTR);
void NTAPI SetDlgItemText(HWND, int, PCWSTR);
