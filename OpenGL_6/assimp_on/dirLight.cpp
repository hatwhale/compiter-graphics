#include "common_header.h"

#include "dirLight.h"

CDirectionalLight::CDirectionalLight()
{
	vColor = glm::vec3(1.0f, 1.0f, 1.0f);
	vDirection = glm::vec3(0.0f, -1.0f, 0.0f);

	fAmbient = 0.25f;
	bSkybox = false;
	bSwitch = true;
}

CDirectionalLight::CDirectionalLight(glm::vec3 a_vColor, float a_fAmbient)
{
	vColor = a_vColor;
	fAmbient = a_fAmbient;
	bSkybox = false;
	bSwitch = true;
}

/*-----------------------------------------------

Name:	SetUniformData

Params:	spProgram - shader program
		sLightVarName - name of directional light variable

Result:	Sets all directional light data.

/*---------------------------------------------*/

void CDirectionalLight::SetUniformData(CShaderProgram *spProgram, std::string sLightVarName)
{
	spProgram->SetUniform(sLightVarName+".vColor", vColor);
	spProgram->SetUniform(sLightVarName+".vDirection", vDirection);

	spProgram->SetUniform(sLightVarName+".fAmbient", fAmbient);
	spProgram->SetUniform(sLightVarName+".bSkybox", bSkybox);
}

void CDirectionalLight::SetUniformData(CShaderProgram *spProgram, std::string sLightVarName, int index)
{
	std::string sIndex = '[' + std::to_string(index) + ']';
	spProgram->SetUniform(sLightVarName + sIndex + ".vColor", vColor);
	spProgram->SetUniform(sLightVarName + sIndex + ".vDirection", vDirection);

	spProgram->SetUniform(sLightVarName + sIndex + ".fAmbient", fAmbient);
	spProgram->SetUniform(sLightVarName + sIndex + ".bSkybox", bSkybox);

	spProgram->SetUniform(sLightVarName + sIndex + ".bSwitch", bSwitch);
}