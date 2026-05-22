#include "gl_helpers.h"

#include "container/array.h"
#include "os/file.h"
#include "stream/filestream.h"

void printShaderLog( GLuint shader )
{
	GLint log_length = 0;
	gl().glGetShaderiv( shader, GL_INFO_LOG_LENGTH, &log_length );

	Array<char> log( log_length );
	gl().glGetShaderInfoLog( shader, log_length, &log_length, log.data() );

	globalErrorStream() << StringRange( log.begin(), log_length ) << '\n';
}

void printProgramLog( GLuint program ){
	GLint log_length = 0;
	gl().glGetProgramiv( program, GL_INFO_LOG_LENGTH, &log_length );

	Array<char> log( log_length );
	gl().glGetProgramInfoLog( program, log_length, &log_length, log.data() );

	globalErrorStream() << StringRange( log.begin(), log_length ) << '\n';
}

void gl_shader_create( GLuint program, const char* filename, GLenum type )
{
	GLuint shader = gl().glCreateShader( type );
	GlobalOpenGL_debugAssertNoErrors();

	// load shader
	{
		std::size_t size = file_size( filename );
		FileInputStream file( filename );
		ASSERT_MESSAGE( !file.failed(), "failed to open " << makeQuoted( filename ) );
		Array<GLchar> buffer( size );
		size = file.read( reinterpret_cast<StreamBase::byte_type*>( buffer.data() ), size );

		const GLchar* string = buffer.data();
		GLint length = GLint( size );
		gl().glShaderSource( shader, 1, &string, &length );
	}

	// compile shader
	{
		gl().glCompileShader( shader );

		GLint compiled = 0;
		gl().glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

		if ( !compiled ) {
			printShaderLog( shader );
		}

		ASSERT_MESSAGE( compiled, "shader compile failed: " << makeQuoted( filename ) );
	}

	// attach shader
	gl().glAttachShader( program, shader );

	gl().glDeleteShader( shader );

	GlobalOpenGL_debugAssertNoErrors();
}

void gl_program_link( GLuint program )
{
	gl().glLinkProgram( program );

	GLint linked = false;
	gl().glGetProgramiv( program, GL_LINK_STATUS, &linked );

	if ( !linked ) {
		printProgramLog( program );
	}

	ASSERT_MESSAGE( linked, "program link failed" );
}

void gl_program_validate( GLuint program )
{
	gl().glValidateProgram( program );

	GLint validated = false;
	gl().glGetProgramiv( program, GL_VALIDATE_STATUS, &validated );

	if ( !validated ) {
		printProgramLog( program );
	}

	ASSERT_MESSAGE( validated, "program validation failed" );
}
