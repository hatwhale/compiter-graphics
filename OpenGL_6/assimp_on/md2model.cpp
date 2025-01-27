#include "common_header.h"

#include "md2model.h"

anim_t CMD2Model::animlist[ 21 ] = 
{
	// first, last, fps
	{   0,  39,  9 },	// STAND
	{  40,  45, 10 },	// RUN
	{  46,  53, 10 },	// ATTACK
	{  54,  57,  7 },	// PAIN_A
	{  58,  61,  7 },	// PAIN_B
	{  62,  65,  7 },	// PAIN_C
	{  66,  71,  7 },	// JUMP
	{  72,  83,  7 },	// FLIP
	{  84,  94,  7 },	// SALUTE
	{  95, 111, 10 },	// FALLBACK
	{ 112, 122,  7 },	// WAVE
	{ 123, 134,  6 },	// POINTIING
	{ 135, 153, 10 },	// CROUCH_STAND
	{ 154, 159,  7 },	// CROUCH_WALK
	{ 160, 168, 10 },	// CROUCH_ATTACK
	{ 196, 172,  7 },	// CROUCH_PAIN
	{ 173, 177,  5 },	// CROUCH_DEATH
	{ 178, 183,  7 },	// DEATH_FALLBACK
	{ 184, 189,  7 },	// DEATH_FALLFORWARD
	{ 190, 197,  7 },	// DEATH_FALLBACKSLOW
	{ 198, 198,  5 },	// BOOM
};

string sMD2AnimationNames[MAX_ANIMATIONS] = 
{
	"Stand",
	"Run",
	"Attack",
	"Pain A",
	"Pain B",
	"Pain C",
	"Jump",
	"Flip",
	"Salute",
	"Fallback",
	"Wave",
	"Pointing",
	"Crouch Stand",
	"Crouch Walk",
	"Crouch Attack",
	"Crouch Pain",
	"Crouch Death",
	"Death Fallback",
	"Death Fall Forward",
	"Death Fallback Slow",
	"Boom"
};

#pragma warning( once : 4305 )

vec3_t anorms[NUMVERTEXNORMALS] = 
{
	#include "anorms.h"
};

CMD2Model::CMD2Model()
{
	bLoaded = false;
}

/*-----------------------------------------------

Name:	LoadModel

Params:	sFilename - md2 model file path

Result:	Loads MD2 model from file.

/*---------------------------------------------*/

bool CMD2Model::LoadModel(char* sFilename)
{
	FILE* fp = fopen(sFilename, "rb");
	
	if(!fp)return false;

	fread(&header, sizeof(md2_t), 1, fp); // Read header where all info about model is stored

	char* buffer = new char[header.num_frames * header.framesize]; // Read all frame data to one big buffer
	fseek(fp, header.ofs_frames, SEEK_SET);
	fread(buffer, sizeof(char), header.num_frames * header.framesize, fp);

	vVertices.resize(header.num_frames, vector<glm::vec3>(header.num_xyz)); // Allocate space for vertices
	vNormals.resize(header.num_frames, vector<int>(header.num_xyz)); // And normals

	// Extract vertices and normals from frame data
	FOR(i, header.num_frames)
	{
		frame_t* frame_ptr = (frame_t*)&buffer[header.framesize * i]; // Convert buffer to frame_t pointer

		FOR(j, header.num_xyz)
		{
			vVertices[i][j].x = frame_ptr->translate[0] + (float(frame_ptr->verts[j].v[0]) * frame_ptr->scale[0]);
			vVertices[i][j].y = frame_ptr->translate[1] + (float(frame_ptr->verts[j].v[1]) * frame_ptr->scale[1]);
			vVertices[i][j].z = frame_ptr->translate[2] + (float(frame_ptr->verts[j].v[2]) * frame_ptr->scale[2]);

			vNormals[i][j] = frame_ptr->verts[j].lightnormalindex;
		}
	}

	// Now let's read OpenGL rendering commands
	glCommands.resize(header.num_glcmds);
	fseek(fp, header.ofs_glcmds, SEEK_SET);
	fread(&glCommands[0], sizeof(int), header.num_glcmds, fp);
	
	int i = 0;
	int iTotalVertices = 0;

	// And start with creating VBOs for vertices, textue coordinates and normals
	vboFrameVertices.resize(header.num_frames);
	FOR(i, header.num_frames)vboFrameVertices[i].CreateVBO();
	vboTextureCoords.CreateVBO();
	
	while(1) // We while loop until we come to 0 value, which is the end of OpenGL commands
	{
		int action = glCommands[i];
		if(action == 0)break;

		int renderMode = action < 0 ? GL_TRIANGLE_FAN : GL_TRIANGLE_STRIP; // Extract rendering mode
		int numVertices = action < 0 ? -action : action; // And number of vertices
		i++;

		renderModes.push_back(renderMode); // Remember the values
		numRenderVertices.push_back(numVertices);

		FOR(j, numVertices)
		{
			float s = *((float*)(&glCommands[i++])); // Extract texture coordinates
			float t = *((float*)(&glCommands[i++]));
			t = 1.0f - t; // Flip t, because it is (from some reasons) stored from top to bottom
			int vi = glCommands[i++];

			vboTextureCoords.AddData(&s, 4); // Add texture coords to VBO
			vboTextureCoords.AddData(&t, 4);

			FOR(k, header.num_frames)
			{
				vboFrameVertices[k].AddData(&vVertices[k][vi], 12); // Add vertex to VBO
				vboFrameVertices[k].AddData(&anorms[vNormals[k][vi]], 12); // Add normal to VBO from normal table
			}
		}
	}

	// Now all necessary data are extracted, let's create VAO for rendering MD2 model

	glGenVertexArrays(1, &uiVAO);
	glBindVertexArray(uiVAO);

	FOR(i, header.num_frames)
	{
		vboFrameVertices[i].BindVBO();
		vboFrameVertices[i].UploadDataToGPU(GL_STATIC_DRAW);
	}

	vboFrameVertices[0].BindVBO(); // Vertex and normals data parameters
	
	// Vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(3); // Vertices for next keyframe, now we can set it to same VBO
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), 0);
	// Normal vectors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(4); // Normals for next keyframe, now we can set it to same VBO
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), (void*)(sizeof(glm::vec3)));

	// Texture coordinates
	vboTextureCoords.BindVBO();
	vboTextureCoords.UploadDataToGPU(GL_STATIC_DRAW);

	// Texture coordinates
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
	
	
	// I have read, that if you read the data from header.num_skins and header.ofs_skins,
	// these data are Quake2 specific paths. So usually you will find models on internet
	// with header.num_skins 0 and texture with the same filename as model filename

	// Find texture name (modelname.jpg, modelname.png...)

	string sPath = sFilename;
	int index = sPath.find_last_of("\\/");
	string sDirectory = index != -1 ? sPath.substr(0, index+1) : "";
	string sPureFilename = index != -1 ? sPath.substr(index+1) : sFilename;

	string sTextureExtensions[] = {"jpg", "jpeg", "png", "bmp", "tga"};
	index = sPureFilename.find_last_of(".");
	if(index != -1)
	{
		string sStripped = sPureFilename.substr(0, index+1);
		FOR(i, 5)
		{
			string sTry = sDirectory+sStripped+sTextureExtensions[i];
			if(tSkin.LoadTexture2D(sTry))
			{
				break;
			}
		}
	}
	
	bLoaded = true;
	return true;
}

/*-----------------------------------------------

Name:	RenderModel

Params:	animState - structure with animation state - 
		if it's NULL, then only static model is rendered

Result:	Renders model.

/*---------------------------------------------*/

void CMD2Model::RenderModel(animState_t* animState)
{
	glBindVertexArray(uiVAO);

	int iTotalOffset = 0;
	tSkin.BindTexture();
	if(animState == NULL)
	{
		glEnableVertexAttribArray(0);
		vboFrameVertices[0].BindVBO();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), 0);
		spMD2Animation.SetUniform("fInterpolation", -1.0f); // Set interpolation to negative number, so that vertex shader knows

		FOR(i, ESZ(renderModes)) // Just render using previously extracted render modes
		{
			glDrawArrays(renderModes[i], iTotalOffset, numRenderVertices[i]);
			iTotalOffset += numRenderVertices[i];
		}
	}
	else
	{
		// Change vertices pointers to current frame
		glEnableVertexAttribArray(0);
		vboFrameVertices[animState->curr_frame].BindVBO();
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), 0);

		glEnableVertexAttribArray(3);
		vboFrameVertices[animState->next_frame].BindVBO();
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), 0);

		// Change normal pointers to current frame

		glEnableVertexAttribArray(2);
		vboFrameVertices[animState->curr_frame].BindVBO();
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), 0);

		glEnableVertexAttribArray(4);
		vboFrameVertices[animState->next_frame].BindVBO();
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 2*sizeof(glm::vec3), 0);

		spMD2Animation.SetUniform("fInterpolation", animState->interpol);

		FOR(i, ESZ(renderModes))
		{
			glDrawArrays(renderModes[i], iTotalOffset, numRenderVertices[i]);
			iTotalOffset += numRenderVertices[i];
		}
	}
}

/*-----------------------------------------------

Name:	StartAnimation

Params:	type - one of 19 types of MD2 animation
		(RUN, ATTACK, STAND...)

Result:	Returns animState_t structure with data
		at start of animation.

/*---------------------------------------------*/

animState_t CMD2Model::StartAnimation(animType_t type)
{
	animState_t res;
	res.startframe = animlist[type].first_frame;
	res.endframe = animlist[type].last_frame;
	res.curr_frame = animlist[type].first_frame;
	res.next_frame = animlist[type].first_frame + 1;

	res.fps = animlist[type].fps;
	res.type = type;

	res.curr_time = 0.0f;
	res.old_time = 0.0f;

	res.interpol = 0.0f;

	res.iterations = 0;

	return res;
}

/*-----------------------------------------------

Name:	UpdateAnimation

Params:	animState - animation state to update
		fTimePassed - time passed since the last
		frame

Result:	Updates data in animation structure (performs
		animation basically).

/*---------------------------------------------*/

void CMD2Model::UpdateAnimation(animState_t* animState, float fTimePassed)
{
	animState->curr_time += fTimePassed;

	if(animState->curr_time - animState->old_time > (1.0f / float(animState->fps)))
	{
		animState->old_time = animState->curr_time;

		animState->curr_frame = animState->next_frame;
		animState->next_frame++;
		if(animState->next_frame > animState->endframe)
		{
			animState->next_frame = animState->startframe;
			animState->iterations++;
		}
	}

	animState->interpol = float(animState->fps) * (animState->curr_time - animState->old_time);
}

/*-----------------------------------------------

Name:	DeleteModel

Params:	none

Result:	Deletes all OpenGL model data.

/*---------------------------------------------*/

void CMD2Model::DeleteModel()
{
	if(!bLoaded)return;
	
	FOR(i, ESZ(vboFrameVertices))vboFrameVertices[i].DeleteVBO();
	vboTextureCoords.DeleteVBO();
	glDeleteVertexArrays(1, &uiVAO);

}