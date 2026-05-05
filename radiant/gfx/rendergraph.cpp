#include "rendergraph.h"

RGGraphicsContext::RGGraphicsContext()
	: m_boundProgram(0)
{
}
void RGGraphicsContext::UseProgram(const GLuint program)
{
	gl().glUseProgram(program);
	m_boundProgram = program;
}

void RGGraphicsContext::BindBuffer(const GLuint buffer)
{
	gl().glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void RGGraphicsContext::BindVertexArray(const GLuint vertexArray)
{
	gl().glBindVertexArray(vertexArray);
}

void RGGraphicsContext::SetLineWidth(const float width)
{
	// Does not do anything, just a way for me to note where what was used when we were on opengl2
}

void RGGraphicsContext::SetUniformMatrix4(const char* const name, const Matrix4& matrix)
{
	unsigned int locator = gl().glGetUniformLocation(m_boundProgram, name);
	gl().glUniformMatrix4fv(locator, 1, GL_TRUE, reinterpret_cast<const float*>(&matrix));
}

void RGGraphicsContext::SetUniformVector4(const char* const name, const Vector4& vector)
{
	unsigned int locator = gl().glGetUniformLocation(m_boundProgram, name);
	gl().glUniform4fv(locator, 1, vector.data());
}

void RGGraphicsContext::SetUniformVector3(const char* const name, const Vector3& vector)
{
	unsigned int locator = gl().glGetUniformLocation(m_boundProgram, name);
	gl().glUniform3fv(locator, 1, vector.data());
}

void RGGraphicsContext::Draw(const GLenum mode, const GLint first, const GLsizei count)
{
	gl().glDrawArrays(mode, first, count);
}

void RGGraphicsTask::Execute(const std::function<void(RGGraphicsContext& context)>& callback)
{
	RGGraphicsContext context;
	callback(context);

	gl().glUseProgram(0);
}

RGBuilder::~RGBuilder()
{
	gl().glDeleteBuffers(m_buffers.size(), m_buffers.data());
	m_buffers.clear();

	gl().glDeleteVertexArrays(m_vertex_arrays.size(), m_vertex_arrays.data());
	m_vertex_arrays.clear();
}

GLuint RGBuilder::CreateBuffer(const char* name, void* data, size_t size)
{
	GLuint buffer;
	gl().glGenBuffers(1, &buffer);

	gl().glBindBuffer(GL_ARRAY_BUFFER, buffer);
	gl().glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	gl().glBindBuffer(GL_ARRAY_BUFFER, 0);

	GlobalOpenGL_debugAssertNoErrors();

	m_buffers.push_back(buffer);
	return buffer;
}

GLuint RGBuilder::CreateVertexArray_p2(const GLuint buffer)
{
	GLuint vertex_array;
	gl().glGenVertexArrays(1, &vertex_array);

	gl().glBindVertexArray(vertex_array);
	gl().glBindBuffer(GL_ARRAY_BUFFER, buffer);
	gl().glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex_p2_t), nullptr);
	gl().glEnableVertexAttribArray(0);
	gl().glBindVertexArray(0);

	GlobalOpenGL_debugAssertNoErrors();

	m_vertex_arrays.push_back(vertex_array);
	return vertex_array;
}

RGGraphicsTask& RGBuilder::AddGraphicsTask(const char* name)
{
	std::unique_ptr<RGGraphicsTask> new_task(new RGGraphicsTask());
	RGGraphicsTask* const task = new_task.get();
	m_tasks.emplace_back(new_task.release());
	return *task;
}
