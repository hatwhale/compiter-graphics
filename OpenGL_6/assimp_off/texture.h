#pragma once

enum ETextureFiltering
{
	TEXTURE_FILTER_MAG_NEAREST = 0, // Nearest criterion for magnification
	TEXTURE_FILTER_MAG_BILINEAR, // Bilinear criterion for magnification
	TEXTURE_FILTER_MIN_NEAREST, // Nearest criterion for minification
	TEXTURE_FILTER_MIN_BILINEAR, // Bilinear criterion for minification
	TEXTURE_FILTER_MIN_NEAREST_MIPMAP, // Nearest criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_BILINEAR_MIPMAP, // Bilinear criterion for minification, but on closest mipmap
	TEXTURE_FILTER_MIN_TRILINEAR, // Bilinear criterion for minification on two closest mipmaps, then averaged
};

// Wraps OpenGL texture object and perform their loading.
class CTexture
{
public:
	void CreateEmptyTexture(int a_iWidth, int a_iHeight, GLenum format);
	void CreateDepthTexture(int a_iWidth, int a_iHeight);
	void CreateFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format, bool bGenerateMipMaps = false);
	void CreateRotationTexture(int a_iWidth, int a_iHeight);

	bool ReloadTexture();
	
	bool LoadTexture2D(string a_sPath, bool bGenerateMipMaps = false);
	void BindTexture(int iTextureUnit = 0);

	void SetFiltering(int a_tfMagnification, int a_tfMinification);

	void SetSamplerParameter(GLenum parameter, GLenum value);

	void SetWrap();

	int GetMinificationFilter();
	int GetMagnificationFilter();

	int GetWidth();
	int GetHeight();
	int GetBPP();

	UINT GetTextureID();

	string GetPath();

	void DeleteTexture();

	CTexture();
private:

	int iWidth, iHeight, iBPP; // Texture width, height, and bytes per pixel
	UINT uiTexture; // Texture name
	UINT uiSampler; // Sampler name
	bool bMipMapsGenerated;

	int tfMinification, tfMagnification;

	string sPath;
};

#define NUMTEXTURES 5
extern CTexture tTextures[NUMTEXTURES];
void LoadAllTextures();