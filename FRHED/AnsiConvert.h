class AnsiConvert
{
private:
	BSTR m_bstr;
public:
	AnsiConvert(PCWSTR text, UINT codepage);
	~AnsiConvert() { SysFreeString(m_bstr); }
	operator PCSTR() { return (PCSTR)m_bstr; }
};
