#pragma once

#include "scop.hpp"

struct VertexKey
{
	int pos, uv, normal;
	bool operator<(const VertexKey& other) const
	{
		if (pos!= other.pos)
			return pos < other.pos;
		if (uv != other.uv)
			return uv < other.uv;
		return normal < other.normal;
	}
};

void loadModelToGPU(scopData& data, const std::string& objPath);