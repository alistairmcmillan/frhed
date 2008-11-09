template <class T>
struct StringTable
{
	operator T *() { return reinterpret_cast<T *>(this); }
	T DiffListItemFormat;
	T AboutFrhed;
};

extern StringTable<LPWSTR> S;
extern StringTable<WORD> IDS;
