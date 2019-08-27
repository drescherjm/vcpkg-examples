/////////////////////////////////////////////////////////////////////////////////////////

struct ColumnFormat
{
public:
	enum DataType {
		DT_NONE   = 0x0000,
		DT_INT    = 0x0001,
		DT_DOUBLE = 0x0002,
		DT_STRING = 0x0004,
		DT_DATE   = 0x0008,
	};

	ColumnFormat(DataType dt = DT_INT, float nWidth = LXW_DEF_COL_WIDTH) : m_dt{ dt },
		m_nWidth{ nWidth } {}

public:
	float		m_nWidth;
	DataType	m_dt;
};

/////////////////////////////////////////////////////////////////////////////////////////