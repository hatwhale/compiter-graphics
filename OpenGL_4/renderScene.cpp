#include "common_header.h"

#include "win_OpenGLApp.h"

#include "shaders.h"
#include "texture.h"
#include "vertexBufferObject.h"

#include "flyingCamera.h"

#include "skybox.h"

#include "spotLight.h"
#include "dirLight.h"
#include "pointLight.h"

#include "objModel.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define NUMTEXTURES 5
#define FOG_EQUATION_LINEAR		0
#define FOG_EQUATION_EXP		1
#define FOG_EQUATION_EXP2		2

/* One VBO, where all static data are stored now,
in this tutorial vertex is stored as 3 floats for
position, 2 floats for texture coordinate and
3 floats for normal vector. */

CVertexBufferObject vboSceneObjects, vboCubeInd, vboCube;
UINT uiVAOs[1]; // Only one VAO now

CTexture tTextures[NUMTEXTURES];
CFlyingCamera cCamera;

CSkybox sbMainSkybox;
CObjModel mdlHouse;
CDirectionalLight dlSun;

#include "static_geometry.h"

namespace FogParameters
{
	float fDensity = 0.04f;
	float fStart = 10.0f;
	float fEnd = 75.0f;
	glm::vec4 vFogColor = glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
	int iFogEquation = FOG_EQUATION_EXP; // 0 = linear, 1 = exp, 2 = exp2
};

// Initializes OpenGL features that will be used.
// lpParam - Pointer to anything you want.
void InitScene(LPVOID lpParam)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Prepare all scene objects

	vboSceneObjects.CreateVBO();
	glGenVertexArrays(1, uiVAOs); // Create one VAO
	glBindVertexArray(uiVAOs[0]);

	vboSceneObjects.BindVBO();

	AddSceneObjects(vboSceneObjects);

	vboSceneObjects.UploadDataToGPU(GL_STATIC_DRAW);

	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), 0);
	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), (void*)sizeof(glm::vec3));
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3)+sizeof(glm::vec2), (void*)(sizeof(glm::vec3)+sizeof(glm::vec2)));

	if(!PrepareShaderPrograms())
	{
		PostQuitMessage(0);
		return;
	}
	// Load textures

	string sTextureNames[] = {"grass.png", "met_wall01a.jpg", "tower.jpg", "box.jpg", "ground.jpg"};

	FOR(i, NUMTEXTURES)
	{
		tTextures[i].LoadTexture2D("data\\textures\\"+sTextureNames[i], true);
		tTextures[i].SetFiltering(TEXTURE_FILTER_MAG_BILINEAR, TEXTURE_FILTER_MIN_BILINEAR_MIPMAP);
	}

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	
	cCamera = CFlyingCamera(glm::vec3(0.0f, 10.0f, 120.0f), glm::vec3(0.0f, 10.0f, 119.0f), glm::vec3(0.0f, 1.0f, 0.0f), 25.0f, 0.001f);
	cCamera.SetMovingKeys('W', 'S', 'A', 'D');

	sbMainSkybox.LoadSkybox("data\\skyboxes\\jajlands1\\", "jajlands1_ft.jpg", "jajlands1_bk.jpg", "jajlands1_lf.jpg", "jajlands1_rt.jpg", "jajlands1_up.jpg", "jajlands1_dn.jpg");

	dlSun = CDirectionalLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(sqrt(2.0f) / 2, -sqrt(2.0f) / 2, 0), 1.0f);

	mdlHouse.LoadModel("data\\models\\house\\house.obj", "house.mtl");
}

float fGlobalAngle;

// Renders whole scene.
// lpParam - Pointer to anything you want.
void RenderScene(LPVOID lpParam)
{
	// Typecast lpParam to COpenGLControl pointer
	COpenGLControl* oglControl = (COpenGLControl*)lpParam;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	spMain.UseProgram();

	glm::mat4 mModelMatrix, mView;

	oglControl->ResizeOpenGLViewportFull();

	spMain.SetUniform("matrices.projMatrix", oglControl->GetProjectionMatrix());
	spMain.SetUniform("gSampler", 0);
	spMain.SetUniform("fogParams.iEquation", FogParameters::iFogEquation);
	spMain.SetUniform("fogParams.vFogColor", FogParameters::vFogColor);
	mView = cCamera.Look();
	spMain.SetUniform("matrices.viewMatrix", &mView);

	mModelMatrix = glm::translate(glm::mat4(1.0f), cCamera.vEye);
	
	spMain.SetUniform("matrices.modelMatrix", &mModelMatrix);
	spMain.SetUniform("matrices.normalMatrix", glm::transpose(glm::inverse(mView*mModelMatrix)));

	if (FogParameters::iFogEquation == FOG_EQUATION_LINEAR)
	{
		spMain.SetUniform("fogParams.fStart", FogParameters::fStart);
		spMain.SetUniform("fogParams.fEnd", FogParameters::fEnd);
	}
	else
		spMain.SetUniform("fogParams.fDensity", FogParameters::fDensity);

	CDirectionalLight dlSun2 = dlSun;

	// We set full ambient for skybox, so that its color isn't affected by directional light

	dlSun2.fAmbient = 1.0f;
	dlSun2.vColor = glm::vec3(1.0f, 1.0f, 1.0f);
	dlSun2.SetUniformData(&spMain, "sunLight");

	sbMainSkybox.RenderSkybox();

	glm::mat4 mModelToCamera;

	glBindVertexArray(uiVAOs[0]);

	dlSun.SetUniformData(&spMain, "sunLight");

	spMain.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	spMain.SetUniform("matrices.modelMatrix", glm::mat4(1.0f));
	spMain.SetUniform("matrices.normalMatrix", glm::mat4(1.0f));


	// Render ground
	tTextures[0].BindTexture();
	glDrawArrays(GL_TRIANGLES, 0, 6);

	FOR(i, 3)
	{
		glm::vec3 vPos = glm::vec3(0.0f, 0.0, 50 + i * -100.0f);
		mModelMatrix = glm::translate(glm::mat4(1.0), vPos);
		mModelMatrix = glm::scale(mModelMatrix, glm::vec3(8.0f, 8.0, 8.0f));
		spMain.SetUniform("matrices.normalMatrix", glm::transpose(glm::inverse(mModelMatrix)));
		spMain.SetUniform("matrices.modelMatrix", &mModelMatrix);
		mdlHouse.RenderModel();
	}

	cCamera.Update();

	if (FogParameters::iFogEquation == FOG_EQUATION_LINEAR)
	{
		if (Keys::Key(VK_ADD))
			FogParameters::fStart += appMain.sof(15.0f);
		if (Keys::Key(VK_SUBTRACT))
			FogParameters::fStart -= appMain.sof(15.0f);

		if (Keys::Key(VK_PRIOR))
			FogParameters::fEnd += appMain.sof(15.0f);
		if (Keys::Key(VK_NEXT))
			FogParameters::fEnd -= appMain.sof(15.0f);
	}
	else
	{
		if (Keys::Key(VK_ADD))
			FogParameters::fDensity += appMain.sof(0.01f);
		if (Keys::Key(VK_SUBTRACT))
			FogParameters::fDensity -= appMain.sof(0.01f);
	}

	if (Keys::Onekey('F'))
		FogParameters::iFogEquation = (FogParameters::iFogEquation + 1) % 3;

	if(Keys::Onekey(VK_ESCAPE))PostQuitMessage(0);
	fGlobalAngle += appMain.sof(1.0f);
	oglControl->SwapBuffers();
}

// Releases OpenGL scene.
// lpParam - Pointer to anything you want.
void ReleaseScene(LPVOID lpParam)
{
	FOR(i, NUMTEXTURES)tTextures[i].DeleteTexture();
	sbMainSkybox.DeleteSkybox();
	spMain.DeleteProgram();
	FOR(i, NUMSHADERS)shShaders[i].DeleteShader();

	glDeleteVertexArrays(1, uiVAOs);
	vboSceneObjects.DeleteVBO();
	vboCubeInd.DeleteVBO();
	vboCube.DeleteVBO();
	mdlHouse.DeleteModel();
}