#pragma once
#include "bricks.h"

namespace bricks
{
	class xtree_cloner_t : public xtree_visitor_t
	{
	public:

		xtree_cloner_t();

		virtual bool start_element(const string& name, const properties_list_t& properties, const buffer_t& value) override;

		virtual bool end_element(const string& name) override;

		virtual bool clone(xtree_t* src,  optional<bricks_handle_t> src_h, const string_view& path, xtree_t* dst, optional<bricks_handle_t> dst_h);

	protected:

		xtree_t* dst = nullptr;

		list<bricks_handle_t> dst_handles;
	};

}

