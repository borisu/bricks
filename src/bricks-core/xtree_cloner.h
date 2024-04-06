#pragma once
#include "bricks.h"

namespace bricks
{
	class xtree_cloner_t : public xtree_visitor_t
	{
	public:

		xtree_cloner_t(const xtree_t* src, const xp_t& xp_src, xtree_t* dst, const xp_t& xp_dst);

		virtual bool start_element(const string& name, const property_list_t& properties, const vector_t& value) override;

		virtual bool end_element(const string& name) override;

		virtual bool clone();

	protected:

		xtree_t* dst = nullptr;

		const xtree_t* src = nullptr;
		
		const xp_t& xp_src;
		
		const xp_t& xp_dst;

		list<xp_t> dst_handles;
	};

}

