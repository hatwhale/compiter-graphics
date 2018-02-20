#pragma once

#include "texture.h"
#include "shaders.h"

/********************************

Class:		CFramebuffer

Purpose:	Manages framebuffer objects.

********************************/

class CFramebuffer
{
public:

	bool CreateFrameBufferForDepthShadow(int a_iWidth, int a_iHeight);
	

	bool CreateFramebufferWithTexture(int a_iWidth, int a_iHeight);

	bool AddDepthBuffer();
	void BindFramebuffer(bool bSetFullViewport = true);
	void BindFramebufferShadowMap();
	
	void BindFramebufferTexture(int iTextureUnit = 0, bool bRegenMipMaps = false);

	glm::mat4 CalculateProjectionMatrix(float fFOV, float fNear, float fFar);
	glm::mat4 CalculateOrthoMatrix();

	void DeleteFramebuffer();

	int GetWidth();
	int GetHeight();

	CFramebuffer();
private:
	int iWidth, iHeight;
	UINT uiFramebuffer;
	UINT uiDepthRenderbuffer;
	CTexture tFramebufferTex;
	UINT depthTexture;
};
