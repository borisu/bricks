#include "pch.h"
#include "bricks_api.h"
#include "logger_api.h"

void
create_guid(guid_t buf)
{
	static_assert(sizeof(GUID) == sizeof(guid_t));

	GUID guid;
	HRESULT hr = S_OK ;
	if (FAILED(hr = CoCreateGuid(&guid)))
	{
		log1(BRICKS_FATAL,"Cannot create GUID hr=0x%8x",hr);
		throw;
	}

	memcpy(buf, &guid, BRICKS_GUID_SIZE);

}
