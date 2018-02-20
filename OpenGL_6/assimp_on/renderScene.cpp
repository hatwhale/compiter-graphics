#include "common_header.h"

#include "win_OpenGLApp.h"

#include "shaders.h"
#include "texture.h"
#include "vertexBufferObject.h"

#include "flyingCamera.h"

#include "freeTypeFont.h"

#include "skybox.h"
#include "dirLight.h"
#include "material.h"

#include "assimp_model.h"
#include "md2model.h"

#include "heightmap.h"

#include "static_geometry.h"

#include "particle_system_tf.h"

CVertexBufferObject vboSceneObjects;
UINT uiVAOSceneObjects;

CFreeTypeFont ftFont;

CSkybox sbMainSkybox;
CFlyingCamera cCamera;

CDirectionalLight dlSun;

CMaterial matShiny;
CAssimpModel amModels[4];

CMultiLayeredHeightmap hmWorld;

int iTorusFaces;

bool bDisplayNormals = false; // Do not display normals by default

CParticleSystemTransformFeedback psMainParticleSystem;
CMD2Model md2Models[1];
animState_t animationStateMain;

#include "framebuffer.h"

CFramebuffer fboShadowMap;
bool bShadowsOn = true;
bool bDisplayShadowMap = true;
int iShadowMapTextureSize = 1024;

CVertexBufferObject vboShadowMapQuad;
UINT uiVAOShadowMapQuad;

CTexture rotationTexture;

/*-----------------------------------------------

Name:    InitScene

Params:  lpParam - Pointer to anything you want.

Result:  Initializes OpenGL features that will
         be used.

/*---------------------------------------------*/

void InitScene(LPVOID lpParam)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if(!PrepareShaderPrograms())
	{
		PostQuitMessage(0);
		return;
	}
	
	LoadAllTextures();

	vboSceneObjects.CreateVBO();
	glGenVertexArrays(1, &uiVAOSceneObjects); // Create one VAO
	glBindVertexArray(uiVAOSceneObjects);

	vboSceneObjects.BindVBO();

	iTorusFaces = GenerateTorus(vboSceneObjects, 7.0f, 2.0f, 20, 20);
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


	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	// Here we load font with pixel size 32 - this means that if we print with size above 32, the quality will be low
	ftFont.LoadSystemFont("arial.ttf", 32);
	ftFont.SetShaderProgram(&spFont2D);
	
	cCamera = CFlyingCamera(glm::vec3(0.0f, 30.0f, 100.0f), glm::vec3(0.0f, 30.0f, 99.0f), glm::vec3(0.0f, 1.0f, 0.0f), 25.0f, 0.1f);
	cCamera.SetMovingKeys('W', 'S', 'A', 'D');

	sbMainSkybox.LoadSkybox("data\\skyboxes\\delirious\\", "delirious_front.jpg", "delirious_back.jpg", "delirious_right.jpg", "delirious_left.jpg", "delirious_top.jpg", "delirious_top.jpg");

	dlSun = CDirectionalLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(sqrt(2.0f)/2, -sqrt(2.0f)/2, 0), 0.5f, 0);

	amModels[0].LoadModelFromFile("data\\models\\treasure_chest_obj\\treasure_chest.obj");
	amModels[1].LoadModelFromFile("data\\models\\Arrow\\Arrow.obj");
	amModels[2].LoadModelFromFile("data\\models\\fountainOBJ\\fountain.obj");
	amModels[3].LoadModelFromFile("data\\models\\ConcreteColumn_OBJ\\ConcreteColumn.obj");
	
	CAssimpModel::FinalizeVBO();
	CMultiLayeredHeightmap::LoadTerrainShaderProgram();
	hmWorld.LoadHeightMapFromImage("data\\worlds\\world.bmp");
	hmWorld.SetRenderSize(300.0f, 25.0f, 300.0f);

	matShiny = CMaterial(1.0f, 32.0f);

	psMainParticleSystem.InitalizeParticleSystem();


	psMainParticleSystem.SetGeneratorProperties(
		glm::vec3(-98.76f, 43.02f, 1.34f), // Where the particles are generated
		glm::vec3(-10, 0, -10), // Minimal velocity
		glm::vec3(10, 20, 10), // Maximal velocity
		glm::vec3(0, -20, 0), // Gravity force applied to particles
		glm::vec3(0.0f, 0.25f, 1.0f), // Color (dark blue)
		1.5f, // Minimum lifetime in seconds
		3.0f, // Maximum lifetime in seconds
		0.25f, // Rendered size
		0.02f, // Spawn every 0.02 seconds
		30); // And spawn 30 particles

	md2Models[0].LoadModel("data/models/HoboGoblin/Model.MD2");
	animationStateMain = md2Models[0].StartAnimation(STAND);

	glm::vec3 vShadowMapQuad[] = 
	{
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 0.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	};

	glm::vec2 vShadowMapQuadTC[] =
	{
		glm::vec2(0.0f, 1.0f),
		glm::vec2(0.0f, 0.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f)
	};

	vboShadowMapQuad.CreateVBO();
	FOR(i, 4)
	{
		vboShadowMapQuad.AddData(&vShadowMapQuad[i], sizeof(glm::vec3));
		vboShadowMapQuad.AddData(&vShadowMapQuadTC[i], sizeof(glm::vec2));
	}

	glGenVertexArrays(1, &uiVAOShadowMapQuad);
	glBindVertexArray(uiVAOShadowMapQuad);

	vboShadowMapQuad.BindVBO();
	vboShadowMapQuad.UploadDataToGPU(GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3)+sizeof(glm::vec2), 0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec3)+sizeof(glm::vec2), (void*)sizeof(glm::vec3));

	fboShadowMap.CreateFrameBufferForDepthShadow(iShadowMapTextureSize, iShadowMapTextureSize);

	rotationTexture.CreateRotationTexture(64, 64);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

/*-----------------------------------------------

Name:    RenderScene

Params:  lpParam - Pointer to anything you want.

Result:  Renders whole scene.

/*---------------------------------------------*/

glm::vec3 vModelPosition = glm::vec3(0, 20, 0);
float fModelRotation;
animType_t atCurrentAnimation = STAND;

struct SShootedArrow
{
	glm::vec3 vPos;
	glm::vec3 vDir;
	float fRotAngle;
	float fLifeTime;
};

std::vector<SShootedArrow> arrows;

void RenderScene(LPVOID lpParam)
{
	// Typecast lpParam to COpenGLControl pointer
	COpenGLControl* oglControl = (COpenGLControl*)lpParam;

	// This values set the darkness of whole scene (direction of light), that's why such name of variable :D
	static float fAngleOfDarkness = 45.0f;

	glm::mat4 mDepthBiasMVP;
	glm::mat4 mModel;

	if(bShadowsOn) // So if the shadows are on
	{
		// We are going to render scene from the light's point of view
		fboShadowMap.BindFramebuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		spShadowMapper.UseProgram();
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		spShadowMapper.UseProgram();

		// Because we have a directional light, we just set it high enough (vLightPos) so that it sees all objects on scene
		// We also create orthographics projection matrix for the purposes of rendering shadows
		const float fRangeX = 150, fRangeY = 150, fMinZ = 0.125f, fMaxZ = 512;
		glm::mat4 mPROJ = glm::ortho<float>(-fRangeX, fRangeX, -fRangeY, fRangeY, fMinZ, fMaxZ);
		glm::vec3 vLightPos = -dlSun.vDirection*256.0f;
		glm::mat4 mViewFromLight = glm::lookAt(vLightPos, glm::vec3(0,0,0), glm::cross(glm::vec3(0.0f, 0.0f, -1.0f), dlSun.vDirection));

		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0, 
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
			);

		// Calculate depth bias matrix to calculate shadow coordinates in shader programs
		mDepthBiasMVP = biasMatrix * mPROJ * mViewFromLight;

		CAssimpModel::BindModelsVAO();

		// Render fountain

		mModel = glm::translate(glm::mat4(1.0), glm::vec3(-97.87f, 25.15f, 3.27f));
		mModel = glm::scale(mModel, glm::vec3(0.5f, 0.5f, 0.5f));

		spShadowMapper.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
		glm::mat4 depthMVP = mPROJ * mViewFromLight * mModel;
		spShadowMapper.SetUniform("depthMVP", depthMVP);
		amModels[2].RenderModel();

		// Render some pillars

		FOR(row, 3)FOR(column, 2)
		{
			glm::vec3 vPosition = glm::vec3(100.0f+column*40.0f, 0.0f, -140.0f+row*33.0f+column*16.0f);
			vPosition.y = hmWorld.GetHeightFromRealVector(vPosition);

			mModel = glm::translate(glm::mat4(1.0), vPosition);
			mModel = glm::scale(mModel, glm::vec3(0.5f, 0.5f, 0.5f));
			depthMVP = mPROJ * mViewFromLight * mModel;
			spShadowMapper.SetUniform("depthMVP", depthMVP);
			spShadowMapper.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
			amModels[3].RenderModel();
		}

		// Render set of treasures

		FOR(i, 3)
		{
			glm::vec3 vPosition = glm::vec3(-100.0f+i*47.0f, 0.0f, -30.0f-i*47.0f);
			vPosition.y = hmWorld.GetHeightFromRealVector(vPosition);
			mModel = glm::translate(glm::mat4(1.0), vPosition);
			mModel = glm::scale(mModel, glm::vec3(1.0f, 1.0f, 1.0f));

			depthMVP = mPROJ * mViewFromLight * mModel;
			spShadowMapper.SetUniform("depthMVP", depthMVP);
			spShadowMapper.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
			amModels[0].RenderModel();
		}

		FOR(i, ESZ(arrows)) // Iterate through all arrows and render them
		{
			if(arrows[i].fLifeTime < 0.0f)
			{
				arrows.erase(arrows.begin()+i);
				i--;
				continue;
			}
			mModel = glm::translate(glm::mat4(1.0), arrows[i].vPos);
			mModel = glm::rotate(mModel, arrows[i].fRotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
			mModel = glm::scale(mModel, glm::vec3(12.0f, 12.0f, 6.0f));
			
			depthMVP = mPROJ * mViewFromLight * mModel;
			spShadowMapper.SetUniform("depthMVP", depthMVP);
			spShadowMapper.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
			amModels[1].RenderModel();
		}

		// Render extra arrow that shows direction of light

		mModel = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 80.0f, 0.0f));
		mModel = glm::rotate(mModel, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
		mModel = glm::rotate(mModel, -fAngleOfDarkness, glm::vec3(0.0f, 1.0f, 0.0f));
		mModel = glm::scale(mModel, glm::vec3(20.0f, 20.0f, 10.0f));

		depthMVP = mPROJ * mViewFromLight * mModel;
		spShadowMapper.SetUniform("depthMVP", depthMVP);
		spShadowMapper.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
		amModels[1].RenderModel();

		// Render the Hobo Goblin

		mModel = glm::translate(glm::mat4(1.0), glm::vec3(vModelPosition));
		mModel = glm::rotate(mModel, fModelRotation, glm::vec3(0, 1, 0));
		mModel = glm::rotate(mModel, -90.0f, glm::vec3(1, 0, 0));
		mModel = glm::scale(mModel, glm::vec3(0.35f, 0.35f, 0.35f));

		depthMVP = mPROJ * mViewFromLight * mModel;
		spShadowMapper.SetUniform("depthMVP", depthMVP);
		spShadowMapper.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);

		md2Models[0].RenderModel(&animationStateMain);

		// ... and finally render heightmap

		mModel = hmWorld.GetScaleMatrix();

		depthMVP = mPROJ * mViewFromLight * mModel;
		spShadowMapper.SetUniform("depthMVP", depthMVP);
		spShadowMapper.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);

		hmWorld.RenderHeightmapToShadowMap();

		// Now the shadows are rendered, we can go back to normal rendering
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	oglControl->ResizeOpenGLViewportFull();

	// Render skybox

	spSkybox.UseProgram();

	spSkybox.SetUniform("matrices.projMatrix", oglControl->GetProjectionMatrix());
	spSkybox.SetUniform("matrices.viewMatrix", cCamera.Look());
	spSkybox.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", glm::mat4(1.0));

	spSkybox.SetUniform("vColor", glm::vec4(1, 1, 1, 1));
	spSkybox.SetUniform("gSampler", 0);	
	spSkybox.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", glm::translate(glm::mat4(1.0), cCamera.vEye));

	sbMainSkybox.RenderSkybox();

	spMain.UseProgram();

	spMain.SetUniform("matrices.projMatrix", oglControl->GetProjectionMatrix());
	spMain.SetUniform("matrices.viewMatrix", cCamera.Look());

	spMain.SetUniform("gSampler", 0);

	spMain.SetUniform("matrices.modelMatrix", glm::mat4(1.0));
	spMain.SetUniform("matrices.normalMatrix", glm::mat4(1.0));
	spMain.SetUniform("vColor", glm::vec4(1, 1, 1, 1));

	// You can play with direction of light with '+' and '-' key
	if(Keys::Key(VK_ADD))fAngleOfDarkness += appMain.sof(90);
	if(Keys::Key(VK_SUBTRACT))fAngleOfDarkness -= appMain.sof(90);

	if(fAngleOfDarkness < -90.0f) fAngleOfDarkness = -90.0f;
	if(fAngleOfDarkness > 90.0f) fAngleOfDarkness = 90.0f;

	// Set the directional vector of light
	dlSun.vDirection = glm::vec3(-sin(fAngleOfDarkness*3.1415f/180.0f), -cos(fAngleOfDarkness*3.1415f/180.0f), 0.0f);
	dlSun.SetUniformData(&spMain, "sunLight");

	spMain.SetUniform("vEyePosition", cCamera.vEye);
	// I'm always using this shiny material, no matter what I render, it would be nice to change it sometimes :P
	matShiny.SetUniformData(&spMain, "matActive"); 

	spMain.SetUniform("matrices.depthBiasMVP", mDepthBiasMVP);
	// Bind shadow map to 5th texture unit (index is 5, if we count from 1 it's 6th)
	// You can use whichever texture unit you want, I used 5 because of 5 textures in terrain and 6th first available
	spMain.SetUniform("shadowMap", 5);
	fboShadowMap.BindFramebufferTexture(5, false);
	spMain.SetUniform("bShadowsOn", bShadowsOn ? 1 : 0);
	spMain.SetUniform("rotationTexture", 6);
	rotationTexture.BindTexture(6);
	spMain.SetUniform("scale", 1.0f / 64.0f);
	spMain.SetUniform("radius", (iShadowMapTextureSize / 1024.0f) / iShadowMapTextureSize);

	CAssimpModel::BindModelsVAO();

	spMain.UseProgram();
	
	// Render fountain

	mModel = glm::translate(glm::mat4(1.0), glm::vec3(-97.87f, 25.15f, 3.27f));
	mModel = glm::scale(mModel, glm::vec3(0.5f, 0.5f, 0.5f));

	spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
	amModels[2].RenderModel();

	// Render some pillars

	FOR(row, 3)FOR(column, 2)
	{
		glm::vec3 vPosition = glm::vec3(100.0f+column*40.0f, 0.0f, -140.0f+row*33.0f+column*16.0f);
		vPosition.y = hmWorld.GetHeightFromRealVector(vPosition);

		mModel = glm::translate(glm::mat4(1.0), vPosition);
		mModel = glm::scale(mModel, glm::vec3(0.5f, 0.5f, 0.5f));

		spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
		amModels[3].RenderModel();
	}

	// Render set of treasures

	FOR(i, 3)
	{
		glm::vec3 vPosition = glm::vec3(-100.0f+i*47.0f, 0.0f, -30.0f-i*47.0f);
		vPosition.y = hmWorld.GetHeightFromRealVector(vPosition);
		mModel = glm::translate(glm::mat4(1.0), vPosition);
		mModel = glm::scale(mModel, glm::vec3(1.0f, 1.0f, 1.0f));

		spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
		amModels[0].RenderModel();
	}

	FOR(i, ESZ(arrows)) // Iterate through all arrows, render them and update them
	{
		if(arrows[i].fLifeTime < 0.0f)
		{
			arrows.erase(arrows.begin()+i);
			i--;
			continue;
		}
		mModel = glm::translate(glm::mat4(1.0), arrows[i].vPos);
		mModel = glm::rotate(mModel, arrows[i].fRotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
		mModel = glm::scale(mModel, glm::vec3(12.0f, 12.0f, 6.0f));

		spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
		amModels[1].RenderModel();

		arrows[i].vPos += arrows[i].vDir*appMain.sof(92.0f);
		arrows[i].fLifeTime -= appMain.sof(1.0f);
	}

	// Render extra arrow that shows direction of light

	mModel = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 80.0f, 0.0f));
	mModel = glm::rotate(mModel, 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	mModel = glm::rotate(mModel, -fAngleOfDarkness, glm::vec3(0.0f, 1.0f, 0.0f));
	mModel = glm::scale(mModel, glm::vec3(20.0f, 20.0f, 10.0f));

	spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
	amModels[1].RenderModel();

	// Render Hobo Goblin

	spMD2Animation.UseProgram();
	spMD2Animation.SetUniform("matrices.projMatrix", oglControl->GetProjectionMatrix());
	spMD2Animation.SetUniform("matrices.viewMatrix", cCamera.Look());

	spMD2Animation.SetUniform("gSampler", 0);
	spMD2Animation.SetUniform("vColor", glm::vec4(1, 1, 1, 1));

	dlSun.SetUniformData(&spMD2Animation, "sunLight");
	matShiny.SetUniformData(&spMD2Animation, "matActive");

	mModel = glm::translate(glm::mat4(1.0), glm::vec3(vModelPosition));
	mModel = glm::rotate(mModel, fModelRotation, glm::vec3(0, 1, 0));
	mModel = glm::rotate(mModel, -90.0f, glm::vec3(1, 0, 0));
	mModel = glm::scale(mModel, glm::vec3(0.35f, 0.35f, 0.35f));

	spMD2Animation.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);

	spMD2Animation.SetUniform("matrices.depthBiasMVP", mDepthBiasMVP);

	spMD2Animation.SetUniform("shadowMap", 5);
	fboShadowMap.BindFramebufferTexture(5, false);
	spMD2Animation.SetUniform("bShadowsOn", 1);
	spMD2Animation.SetUniform("rotationTexture", 6);
	rotationTexture.BindTexture(6);
	spMD2Animation.SetUniform("scale", 1.0f / 64.0f);
	spMD2Animation.SetUniform("radius", (iShadowMapTextureSize / 1024.0f) / iShadowMapTextureSize);

	md2Models[0].RenderModel(&animationStateMain);
	md2Models[0].UpdateAnimation(&animationStateMain, appMain.sof(1.0f));

	// Now we're going to render terrain

	CShaderProgram* spTerrain = CMultiLayeredHeightmap::GetShaderProgram();

	spTerrain->UseProgram();

	spTerrain->SetUniform("matrices.projMatrix", oglControl->GetProjectionMatrix());
	spTerrain->SetUniform("matrices.viewMatrix", cCamera.Look());

	spTerrain->SetUniform("vEyePosition", cCamera.vEye);

	spTerrain->SetUniform("shadowMap", 5);	
	fboShadowMap.BindFramebufferTexture(5, false);
	spTerrain->SetUniform("bShadowsOn", 1);
	spTerrain->SetUniform("rotationTexture", 6);
	rotationTexture.BindTexture(6);
	spTerrain->SetUniform("scale", 1.0f / 64.0f);
	spTerrain->SetUniform("radius", (iShadowMapTextureSize / 1024.0f) / iShadowMapTextureSize);

	matShiny.SetUniformData(spTerrain, "matActive");

	// We bind all 5 textures - 3 of them are textures for layers, 1 texture is a "path" texture, and last one is
	// the places in heightmap where path should be and how intense should it be
	FOR(i, 5)
	{
		char sSamplerName[256];
		sprintf(sSamplerName, "gSampler[%d]", i);
		tTextures[i].BindTexture(i);
		spTerrain->SetUniform(sSamplerName, i);
	}

	// ... set some uniforms
	spTerrain->SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", glm::mat4(1.0));
	spTerrain->SetUniform("vColor", glm::vec4(1, 1, 1, 1));

	dlSun.SetUniformData(spTerrain, "sunLight");

	spTerrain->SetUniform("matrices.depthBiasMVP", mDepthBiasMVP);

	// ... and finally render heightmap
	hmWorld.RenderHeightmap();

	// Render particles (water in fountain in this tutorial)

	tTextures[6].BindTexture(); 

	psMainParticleSystem.SetMatrices(oglControl->GetProjectionMatrix(), cCamera.vEye, cCamera.vView, cCamera.vUp);
	psMainParticleSystem.UpdateParticles(appMain.sof(1.0f));
	psMainParticleSystem.RenderParticles();

	cCamera.Update();

	// Print something over scene
	
	spFont2D.UseProgram();
	glDisable(GL_DEPTH_TEST);
	spFont2D.SetUniform("matrices.projMatrix", oglControl->GetOrthoMatrix());

	int w = oglControl->GetViewportWidth(), h = oglControl->GetViewportHeight();
	
	spFont2D.SetUniform("vColor", glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
	ftFont.Print("www.mbsoftworks.sk", 20, 20, 24);
	ftFont.PrintFormatted(20, h-30, 20, "FPS: %d", oglControl->GetFPS());
	ftFont.PrintFormatted(20, h-55, 20, "Shadows: %s ('R' to toggle)", bShadowsOn ? "On" : "Off");
	ftFont.PrintFormatted(20, h-80, 20, "Display Shadow Map: %s ('M' to toggle)", bDisplayShadowMap ? "Yes" : "Nope");
	ftFont.PrintFormatted(20, h-105, 20, "Shadow Map Texture Size: %dx%d (Change with PGUP and PGDN)", iShadowMapTextureSize, iShadowMapTextureSize);

	ftFont.PrintFormatted(20, h-155, 20, "Move with arrow keys, shoot with SPACE ;)");
	ftFont.PrintFormatted(20, h-180, 20, "Use + and - to play with direction of light");
	ftFont.PrintFormatted(20, h-205, 20, "(the arrow in the sky shows direction of light)");

	if(bDisplayShadowMap)
	{
		// Display shadow map
		glViewport(0, 0, 512, 512);
		spShadowMapRender.UseProgram();
		glBindVertexArray(uiVAOShadowMapQuad);
		fboShadowMap.BindFramebufferTexture();
		spShadowMapRender.SetUniform("shadowMap", 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	bool bRunning = false;
	static bool bLastFrameRunning = false;

	if(Keys::Key(VK_UP))
	{
		float fSine = float(sin((fModelRotation+90)*3.1415f/180.0f));
		float fCosine = float(cos((fModelRotation+90)*3.1415f/180.0f));

		glm::vec3 vMoveVector(fSine, 0, fCosine);

		vModelPosition += vMoveVector*appMain.sof(50.0f);

		if(animationStateMain.type != RUN)animationStateMain = md2Models[0].StartAnimation(RUN);
		bRunning = true;
	}
	if(Keys::Key(VK_DOWN))
	{
		float fSine = float(sin((fModelRotation+90)*3.1415f/180.0f));
		float fCosine = float(cos((fModelRotation+90)*3.1415f/180.0f));

		glm::vec3 vMoveVector(fSine, 0, fCosine);

		vModelPosition -= vMoveVector*appMain.sof(50.0f);

		if(animationStateMain.type != RUN)animationStateMain = md2Models[0].StartAnimation(RUN);
		bRunning = true;
	}
	if(Keys::Key(VK_LEFT))
		fModelRotation += appMain.sof(135.0f);
	if(Keys::Key(VK_RIGHT))
		fModelRotation -= appMain.sof(135.0f);

	vModelPosition.y = hmWorld.GetHeightFromRealVector(vModelPosition)+8.0f;

	if(!bRunning && bLastFrameRunning)animationStateMain = md2Models[0].StartAnimation(atCurrentAnimation);
	bLastFrameRunning = bRunning;

	if(Keys::Onekey(VK_SPACE))
	{
		SShootedArrow newArrow;
		newArrow.fLifeTime = 4.0f;
		newArrow.fRotAngle = fModelRotation+90;

		float fSine = float(sin((fModelRotation+90)*3.1415f/180.0f));
		float fCosine = float(cos((fModelRotation+90)*3.1415f/180.0f));

		newArrow.vDir = glm::vec3(fSine, 0, fCosine);
		newArrow.vPos = vModelPosition;

		arrows.push_back(newArrow);

		if(!bRunning)
			animationStateMain = md2Models[0].StartAnimation(ATTACK);
	}
	if(!bRunning && animationStateMain.type == ATTACK && animationStateMain.iterations >= 1)
		animationStateMain = md2Models[0].StartAnimation(STAND);

	if(Keys::Onekey('R'))bShadowsOn = !bShadowsOn;
	if(Keys::Onekey('M'))bDisplayShadowMap = !bDisplayShadowMap;

	bool bRecreate = false;
	if(Keys::Onekey(VK_PRIOR))
	{
		iShadowMapTextureSize <<= 1;
		if(iShadowMapTextureSize == 8192)iShadowMapTextureSize = 1024;
		bRecreate = true;
	}
	if(Keys::Onekey(VK_NEXT))
	{
		iShadowMapTextureSize >>= 1;
		if(iShadowMapTextureSize == 512)iShadowMapTextureSize = 4096;
		bRecreate = true;
	}
	if(bRecreate)
	{
		fboShadowMap.DeleteFramebuffer();
		fboShadowMap.CreateFrameBufferForDepthShadow(iShadowMapTextureSize, iShadowMapTextureSize);
	}

	glEnable(GL_DEPTH_TEST);	
	if(Keys::Onekey(VK_ESCAPE))PostQuitMessage(0);

	oglControl->SwapBuffers();
}

/*-----------------------------------------------

Name:    ReleaseScene

Params:  lpParam - Pointer to OpenGL control.

Result:  Releases OpenGL scene.

/*---------------------------------------------*/

void ReleaseScene(LPVOID lpParam)
{
	FOR(i, NUMTEXTURES)tTextures[i].DeleteTexture();
	sbMainSkybox.DeleteSkybox();

	DeleteAllShaderPrograms();

	ftFont.DeleteFont();

	glDeleteVertexArrays(1, &uiVAOSceneObjects);
	vboSceneObjects.DeleteVBO();

	CAssimpModel::ReleaseAllModelData();
	md2Models[0].DeleteModel();

	hmWorld.ReleaseHeightmap();
	CMultiLayeredHeightmap::ReleaseTerrainShaderProgram();

	fboShadowMap.DeleteFramebuffer();
	psMainParticleSystem.DeleteParticleSystem();

	rotationTexture.DeleteTexture();
}