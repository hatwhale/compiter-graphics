#include "Texture.h"

// формат заголовка TGA-файла
#pragma pack(1)
struct TGAHeader
{
	uint8_t  idlength;
	uint8_t  colormap;
	uint8_t  datatype;
	uint8_t  colormapinfo[5];
	uint16_t xorigin;
	uint16_t yorigin;
	uint16_t width;
	uint16_t height;
	uint8_t  bitperpel;
	uint8_t  description;
};
#pragma pack()

GLuint TextureCreateFromTGA(const char *fileName)
{
	ASSERT(fileName);

	TGAHeader *header;
	uint8_t   *buffer;
	uint32_t  size;
	GLint     format, internalFormat;
	GLuint    texture;

	// попытаемся загрузить текстуру из файла
	if (!LoadFile(fileName, true, &buffer, &size))
		return 0;

	// если размер файла заведомо меньше заголовка TGA
	if (size <= sizeof(TGAHeader))
	{
		LOG_ERROR("Too small file '%s'\n", fileName);
		delete[] buffer;
		return 0;
	}

	header = (TGAHeader*)buffer;

	// проверим формат TGA-файла - несжатое RGB или RGBA изображение
	if (header->datatype != 2 || (header->bitperpel != 24 && header->bitperpel != 32))
	{
		LOG_ERROR("Wrong TGA format '%s'\n", fileName);
		delete[] buffer;
		return 0;
	}

	// получим формат текстуры
	format = (header->bitperpel == 24 ? GL_BGR : GL_BGRA);
	internalFormat = (format == GL_BGR ? GL_RGB8 : GL_RGBA8);

	// запросим у OpenGL свободный индекс текстуры
	glGenTextures(1, &texture);

	// сделаем текстуру активной
	glBindTexture(GL_TEXTURE_2D, texture);

	// установим параметры фильтрации текстуры - линейная фильтрация
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// установим параметры "оборачиваниея" текстуры - отсутствие оборачивания
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// загрузим данные о цвете в текущую автивную текстуру
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, header->width, header->height, 0, format,
		GL_UNSIGNED_BYTE, GL_OFFSET(buffer + sizeof(TGAHeader) + header->idlength));

	// после загрузки в текстуру данные о цвете в памяти нам больше не нужны
	delete[] buffer;

	// проверим на наличие ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return texture;
}

GLuint TextureCreateEmpty(GLint internalFormat, GLenum format, GLenum type, GLsizei width, GLsizei height)
{
	ASSERT(width);
	ASSERT(height);

	GLuint texture;

	// запросим у OpenGL свободный индекс текстуры
	glGenTextures(1, &texture);

	// сделаем текстуру активной
	glBindTexture(GL_TEXTURE_2D, texture);

	// установим параметры фильтрации текстуры - линейная фильтрация
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// установим параметры "оборачиваниея" текстуры - отсутствие оборачивания
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// создаем "пустую" текстуру под depth-данные (данные глубины)
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, NULL);

	// проверим на наличие ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return texture;
}

GLuint TextureCreateDepth(GLsizei width, GLsizei height)
{
	ASSERT(width);
	ASSERT(height);

	// создаем пустую текстуру
	GLuint texture = TextureCreateEmpty(GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_FLOAT, width, height);

	// необходимо для использования depth-текстуры как shadow map
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	// проверим на наличие ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return texture;
}

GLuint TextureCreateFake()
{
	GLuint  texture;
	GLubyte fake[3] = {255, 255, 255};

	// запросим у OpenGL свободный индекс текстуры
	glGenTextures(1, &texture);

	// сделаем текстуру активной
	glBindTexture(GL_TEXTURE_2D, texture);

	// установим параметры фильтрации текстуры - линейная фильтрация
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// установим параметры "оборачиваниея" текстуры - отсутствие оборачивания
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// создаем фейковую текстуру
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 1, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, fake);

	// проверим на наличие ошибок
	OPENGL_CHECK_FOR_ERRORS();

	return texture;
}

void TextureDestroy(GLuint texture)
{
	// освободим занятый индекс текстуры
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &texture);
}

void TextureSetup(GLuint program, GLint unit, const GLchar *name, GLuint texture)
{
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(program, name), unit);
}
