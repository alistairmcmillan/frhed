#ifndef FRHED_IDT_H_3C7D4FBFCA9C4071B414648DE7FA280F
#define FRHED_IDT_H_3C7D4FBFCA9C4071B414648DE7FA280F

class HexEditorWindow;

interface CDropTarget : public IDropTarget
{
private:
	ULONG m_cRefCount;

	DWORD LastKeyState;

	bool hdrop_present;
	IDataObject* pDataObj;
	HexEditorWindow &hexwnd;
public:
	//Members
	CDropTarget(HexEditorWindow &hexwnd);
	~CDropTarget();

	//IUnknown members
	STDMETHODIMP QueryInterface(REFIID iid, void** ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	//IDropTarget methods
	STDMETHODIMP DragEnter(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	STDMETHODIMP DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect);
	STDMETHODIMP DragLeave();
	STDMETHODIMP Drop(IDataObject* pDataObject, DWORD grfKeyState, POINTL pt, DWORD * pdwEffect);
};

#endif
