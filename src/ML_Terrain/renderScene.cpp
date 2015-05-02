#include "common_header.h"

#include "win_OpenGLApp.h"

#include "shaders.h"
#include "texture.h"
#include "vertexBufferObject.h"

#include "flyingCamera.h"

#include "freeTypeFont.h"

#include "skybox.h"
#include "dirLight.h"

#include "assimp_model.h"

#include "heightmap.h"


CVertexBufferObject vboSceneObjects;
UINT uiVAOSceneObjects;

CFreeTypeFont ftFont;

CSkybox sbMainSkybox;
CFlyingCamera cCamera;

CDirectionalLight dlSun;

CAssimpModel amModels[3];

CMultiLayeredHeightmap hmWorld;

/*-----------------------------------------------

Name:    InitScene

Params:  lpParam - Pointer to anything you want.

Result:  Initializes OpenGL features that will
         be used.

/*---------------------------------------------*/

void InitScene(LPVOID lpParam)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	if(!PrepareShaderPrograms(lpParam))
	{
		PostQuitMessage(0);
		return;
	}
	
	if (!LoadAllTextures(lpParam)) 
    {
		PostQuitMessage(0);
		return;
    }

	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);

	// Here we load font with pixel size 32 - this means that if we print with size above 32, the quality will be low
	ftFont.LoadSystemFont("arial.ttf", 32);
	ftFont.SetShaderProgram(&spFont2D);
	
	cCamera = CFlyingCamera(glm::vec3(0.0f, 30.0f, 100.0f), glm::vec3(0.0f, 30.0f, 99.0f), glm::vec3(0.0f, 1.0f, 0.0f), 25.0f, 0.1f);
	cCamera.SetMovingKeys('W', 'S', 'A', 'D');
    string dir, file, msg;
    if (lpParam) {
        COpenGLControl *ogl = (COpenGLControl *)lpParam;
        if (ogl->root) {
            dir = ogl->root;
            if (dir.rfind('\\') != dir.size())
               dir += "\\";
        }
    }
    file = dir;
    file += "data\\skyboxes\\elbrus\\";
	if (!sbMainSkybox.LoadSkybox(file, "elbrus_front.jpg", "elbrus_back.jpg", "elbrus_right.jpg", "elbrus_left.jpg", "elbrus_top.jpg", "elbrus_top.jpg"))
    {
        MessageBox(NULL,"Failed to LoadSkyBox()", "Critical Error", MB_ICONINFORMATION | MB_OK);
		PostQuitMessage(0);
        return;
    }

	dlSun = CDirectionalLight(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(sqrt(2.0f)/2, -sqrt(2.0f)/2, 0), 0.5f);
    file = dir;
    file += "data\\models\\Wolf\\Wolf.obj";
	if (!amModels[0].LoadModelFromFile((char *)file.c_str()))
    {
        msg = "Failed to load ";
        msg += file;
        MessageBox(NULL,msg.c_str(), "Critical Error", MB_ICONINFORMATION | MB_OK);
		PostQuitMessage(0);
        return;
    }
    file = dir;
    file += "data\\models\\house\\house.3ds";
	if (!amModels[1].LoadModelFromFile((char *)file.c_str()))
    {
        msg = "Failed to load ";
        msg += file;
        MessageBox(NULL,msg.c_str(), "Critical Error", MB_ICONINFORMATION | MB_OK);
		PostQuitMessage(0);
        return;
    }


	CAssimpModel::FinalizeVBO();

	if (!CMultiLayeredHeightmap::LoadTerrainShaderProgram(lpParam))
    {
		PostQuitMessage(0);
        return;
    }

    file = dir;
    file += "data\\worlds\\consider_this_question.bmp";
	if (!hmWorld.LoadHeightMapFromImage(file.c_str()))
    {
        msg = "Failed to load ";
        msg += file;
        MessageBox(NULL,msg.c_str(), "Critical Error", MB_ICONINFORMATION | MB_OK);
		PostQuitMessage(0);
        return;
    }
}

/*-----------------------------------------------

Name:    RenderScene

Params:  lpParam - Pointer to anything you want.

Result:  Renders whole scene.

/*---------------------------------------------*/

void RenderScene(LPVOID lpParam)
{
	// Typecast lpParam to COpenGLControl pointer
	COpenGLControl* oglControl = (COpenGLControl*)lpParam;
	oglControl->ResizeOpenGLViewportFull();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	spMain.UseProgram();

	spMain.SetUniform("matrices.projMatrix", oglControl->GetProjectionMatrix());
	spMain.SetUniform("matrices.viewMatrix", cCamera.Look());

	spMain.SetUniform("gSampler", 0);

	spMain.SetUniform("matrices.modelMatrix", glm::mat4(1.0));
	spMain.SetUniform("matrices.normalMatrix", glm::mat4(1.0));
	spMain.SetUniform("vColor", glm::vec4(1, 1, 1, 1));

	// This values will set the darkness of whole scene, that's why such name of variable :D
	static float fAngleOfDarkness = 45.0f;
	// You can play with direction of light with '+' and '-' key
	if(Keys::Key(VK_ADD))fAngleOfDarkness += appMain.sof(90);
	if(Keys::Key(VK_SUBTRACT))fAngleOfDarkness -= appMain.sof(90);
	// Set the directional vector of light
	dlSun.vDirection = glm::vec3(-sin(fAngleOfDarkness*3.1415f/180.0f), -cos(fAngleOfDarkness*3.1415f/180.0f), 0.0f);
	dlSun.SetUniformData(&spMain, "sunLight");

	spMain.SetUniform("matrices.modelMatrix", glm::translate(glm::mat4(1.0), cCamera.vEye));
	sbMainSkybox.RenderSkybox();

	spMain.SetUniform("matrices.modelMatrix", glm::mat4(1.0));
	
	// Render a house

	CAssimpModel::BindModelsVAO();

	glm::mat4 mModel = glm::translate(glm::mat4(1.0), glm::vec3(40.0f, 17.0f, 0));
	mModel = glm::scale(mModel, glm::vec3(8, 8, 8)); // Casino :D

	spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
	amModels[1].RenderModel();

	// ... and also ONE wolf now only :P

	mModel = glm::translate(glm::mat4(1.0), glm::vec3(-20.0f, 22.0f, 50));
	mModel = glm::scale(mModel, glm::vec3(2.8f, 2.8f, 2.8f));

	spMain.SetModelAndNormalMatrix("matrices.modelMatrix", "matrices.normalMatrix", mModel);
	amModels[0].RenderModel();


	// Now we're going to render terrain

	hmWorld.SetRenderSize(300.0f, 35.0f, 300.0f);
	CShaderProgram* spTerrain = CMultiLayeredHeightmap::GetShaderProgram();

	spTerrain->UseProgram();

	spTerrain->SetUniform("matrices.projMatrix", oglControl->GetProjectionMatrix());
	spTerrain->SetUniform("matrices.viewMatrix", cCamera.Look());

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

	// ... and finally render heightmap
	hmWorld.RenderHeightmap();

	cCamera.Update();

	// Print something over scene
	
	spFont2D.UseProgram();
	glDisable(GL_DEPTH_TEST);
	spFont2D.SetUniform("matrices.projMatrix", oglControl->GetOrthoMatrix());

	int w = oglControl->GetViewportWidth(), h = oglControl->GetViewportHeight();
	
	spFont2D.SetUniform("vColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	ftFont.Print("www.mbsoftworks.sk", 20, 20, 24);

	ftFont.PrintFormatted(20, h-30, 20, "FPS: %d", oglControl->GetFPS());
	ftFont.PrintFormatted(20, h-80, 20, "Heightmap size: %dx%d", hmWorld.GetNumHeightmapRows(), hmWorld.GetNumHeightmapCols());

	glEnable(GL_DEPTH_TEST);
	if(Keys::Onekey(VK_ESCAPE))PostQuitMessage(0);

	oglControl->SwapBuffers();
}

/*-----------------------------------------------

Name:    ReleaseScene

Params:  lpParam - Pointer to anything you want.

Result:  Releases OpenGL scene.

/*---------------------------------------------*/

void ReleaseScene(LPVOID lpParam)
{
	FOR(i, NUMTEXTURES)tTextures[i].DeleteTexture();
	sbMainSkybox.DeleteSkybox();

	spMain.DeleteProgram();
	spOrtho2D.DeleteProgram();
	spFont2D.DeleteProgram();
	FOR(i, NUMSHADERS)shShaders[i].DeleteShader();
	ftFont.DeleteFont();

	glDeleteVertexArrays(1, &uiVAOSceneObjects);
	vboSceneObjects.DeleteVBO();

	hmWorld.ReleaseHeightmap();
	CMultiLayeredHeightmap::ReleaseTerrainShaderProgram();
}