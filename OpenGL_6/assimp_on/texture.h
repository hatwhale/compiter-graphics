#pragma once

/********************************

Class:		CTexture

Purpose:	Wraps OpenGL texture
			object and performs
			their loading.

********************************/

class CTexture
{
public:
	void CreateEmptyTexture(int a_iWidth, int a_iHeight);
	void CreateDepthTexture(int a_iWidth, int a_iHeight);
	void CreateDepthTexture(int a_iWidth, int a_iHeight, int a_iDepth);
	void CreateRotationTexture(int a_iWidth, int a_iHeight);
	void CreateFromData(BYTE* bData, int a_iWidth, int a_iHeight, int a_iBPP, GLenum format);

	bool LoadTexture2D(string a_sPath);
	void BindTexture(int iTextureUnit = 0);

	UINT GetTextureID();

	string GetPath();

	void DeleteTexture();

	CTexture();
private:

	UINT uiTexture; // Texture name

	string sPath;
};

#define NUMTEXTURES 7
extern CTexture tTextures[NUMTEXTURES];
void LoadAllTextures();