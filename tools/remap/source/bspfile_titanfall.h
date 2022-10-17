#pragma once

#include "qmath.h"
#include <cstdint>
#include "remap.h"

void                        LoadR1BSPFile(const char* filename);
void                        WriteR1BSPFile(const char* filename);
void						CompileR1BSPFile();


namespace Titanfall {
    using Vertex_t = Vector3;

	namespace Bsp {
		inline std::vector<char> entities;
		inline std::vector<Vertex_t> vertices;
		inline std::vector<Vertex_t> vertexNormals;
		inline std::vector<uint16_t> meshIndices;
	}
}