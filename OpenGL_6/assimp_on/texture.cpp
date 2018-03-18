#include "common_header.h"

#include "texture.h"

#include <FreeImage.h>

#ifdef _DEBUG
#pragma comment(lib, "FreeImaged.lib")
#else
#pragma comment(lib, "FreeImage.lib")
#endif

CTexture::CTexture()
{
}

/*-----------------------------------------------

Name:	CreateEmptyTexture

Params:	a_iWidth, a_iHeight - dimensions
		format - format of data

Result:	Creates texture from provided data.

/*---------------------------------------------*/

void CTexture::CreateEmptyTexture(int a_iWidth, int a_iHeight)
{
	glGenTextures(1, &uiTexture);
	glBindTexture(GL_TEXTURE_2D, uiTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int gl_max_texture_max_anisotropy_ext;

	glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_texture_max_anisotropy_ext);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, a_iWidth, a_iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

	glGenerateMipmap(GL_TEXTURE_2D);
}

void CTexture::CreateDepthTexture(int a_iWidth, int a_iHeight)
{
	glGenTextures(1, &uiTexture);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, a_iWidth, a_iHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void CTexture::CreateDepthTexture(int a_iWidth, int a_iHeight, int a_iDepth)
{
	glGenTextures(1, &uiTexture);
	glBindTexture(GL_TEXTURE_2D_ARRAY, uiTexture);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	FOR(l_i, a_iDepth) glTexImage3D(GL_TEXTURE_2D_ARRAY, l_i, GL_DEPTH_COMPONENT24, a_iWidth, a_iHeight, a_iDepth, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
}

void CTexture::CreateRotationTexture(int a_iWidth, int a_iHeight)
{
	glGenTextures(1, &uiTexture);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int size = a_iWidth * a_iHeight;

	glm::vec4 *data = new glm::vec4[size];

	srand(GetTickCount());

	float randomAngle = 3.14f * 2.0f * (float)rand() / (float)RAND_MAX;

	for(int i = 0; i < size; i++)
	{
		data[i].x = cos(randomAngle);
		data[i].y = sin(randomAngle);
		data[i].z = -sin(randomAngle);
		data[i].w = cos(randomAngle);

		data[i] *= 0.5f;
		data[i] += 0.5f;

		randomAngle += 3.14f * 2.0f * (float)rand() / (float)RAND_MAX;
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, a_iWidth, a_iHeight, 0, GL_RGBA, GL_FLOAT, data);

	delete [] data;
}

void CTexture::CreateFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format)
{
	glGenTextures(1, &uiTexture);
	glBindTexture(GL_TEXTURE_2D, uiTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	if(format == GL_RGBA || format == GL_BGRA || format == GL_RGB || format == GL_BGR)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	}
	else
	{
		glTexImage2D(GL_TEXTURE_2D, 0, format, a_iWidth, a_iHeight, 0, format, GL_UNSIGNED_BYTE, bData);
	}
	
	sPath = "";
}

/*-----------------------------------------------

Name:	LoadTexture2D

Params:	a_sPath - path to the texture
		bGenerateMipMaps - whether to create mipmaps

Result:	Loads texture from a file, supports most
		graphics formats.

/*---------------------------------------------*/

bool CTexture::LoadTexture2D(string a_sPath)
{
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(a_sPath.c_str(), 0); // Check the file signature and deduce its format

	if(fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(a_sPath.c_str());
	
	if(fif == FIF_UNKNOWN) // If still unknown, return failure
		return false;

	if(FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, a_sPath.c_str());
	if(!dib)
		return false;

	BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

	// If somehow one of these failed (they shouldn't), return failure
	if(bDataPointer == NULL || FreeImage_GetWidth(dib) == 0 || FreeImage_GetHeight(dib) == 0)
		return false;

	GLenum format = 0;
	
	if(FreeImage_GetBPP(dib) == 32) format = GL_BGRA;
	if(FreeImage_GetBPP(dib) == 24) format = GL_BGR;
	if(FreeImage_GetBPP(dib) == 8) format = GL_LUMINANCE;
	
	if(format != 0)
	{
		glGenTextures(1, &uiTexture);
		glBindTexture(GL_TEXTURE_2D, uiTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		int gl_max_texture_max_anisotropy_ext;

		glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_texture_max_anisotropy_ext);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, FreeImage_GetWidth(dib), FreeImage_GetHeight(dib), 0, format, GL_UNSIGNED_BYTE, FreeImage_GetBits(dib));

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	
	FreeImage_Unload(dib);

	if(format == 0)
		return false;

	sPath = a_sPath;

	return true; // Success
}

void CTexture::BindTexture(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0+iTextureUnit);
	glBindTexture(GL_TEXTURE_2D, uiTexture);
}

void CTexture::BindTextureArray(int iTextureUnit)
{
	glActiveTexture(GL_TEXTURE0 + iTextureUnit);
	glBindTexture(GL_TEXTURE_2D_ARRAY, uiTexture);
}

void CTexture::DeleteTexture()
{
	glDeleteTextures(1, &uiTexture);
}

UINT CTexture::GetTextureID()
{
	return uiTexture;
}

string CTexture::GetPath()
{
	return sPath;
}

CTexture tTextures[NUMTEXTURES];

void LoadAllTextures()
{
	// Load textures

	string sTextureNames[] = {"fungus.dds", "sand_grass_02.jpg", "rock_2_4w.jpg", "sand.jpg", "path.png", "scifi_metal.jpg", "particle.bmp"};

	FOR(i, NUMTEXTURES)
	{
		tTextures[i].LoadTexture2D("data\\textures\\"+sTextureNames[i]);
	}
}