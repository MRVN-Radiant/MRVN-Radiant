#pragma once

#include "igl.h"

#include <functional>

#include "math/matrix.h"

struct vertex_p2_t
{
	float pos[2];
};

class RGTask
{
public:
	~RGTask() = default;
};

class RGGraphicsContext
{
public:
	RGGraphicsContext();

	void	UseProgram(const GLuint program);
	void	BindBuffer(const GLuint buffer);
	void	BindVertexArray(const GLuint vertexArray);
	void	SetLineWidth(const float width);
	void	SetUniformMatrix4(const char* const name, const Matrix4& matrix);
	void	SetUniformVector4(const char* const name, const Vector4& vector);
	void	SetUniformVector3(const char* const name, const Vector3& vector);
	void	Draw(const GLenum mode, const GLint first, const GLsizei count);
private:
	GLuint	m_boundProgram;
};

class RGGraphicsTask : public RGTask
{
public:
	void	Execute(const std::function<void(RGGraphicsContext& context)>& callback);
};

class RGBuilder
{
public:
							~RGBuilder();

	GLuint					CreateBuffer(const char* name, void* data, size_t size);
	GLuint					CreateVertexArray_p2(const GLuint buffer);

	RGGraphicsTask&			AddGraphicsTask(const char* name);

private:
	std::vector<std::unique_ptr<RGTask>>	m_tasks;
	std::vector<GLuint>						m_buffers;
	std::vector<GLuint>						m_vertex_arrays;
};