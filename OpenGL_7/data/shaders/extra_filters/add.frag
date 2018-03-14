// ��������
uniform sampler2D originColorTex, originDepthTex;
uniform sampler2D newColorTex, newDepthTex;

// �������� �����-���������
uniform float Gamma;
// ��� ������ �����������
uniform float MixCoef;

// ���������, ���������� �� ���������� �������
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

vec3 filter(in vec2 texcoord)
{
	vec3 res = texture(originColorTex, texcoord).rgb + (1 - MixCoef) * texture(newColorTex, texcoord).rgb;
	return vec3(pow(res, vec3(1.0/Gamma)));
}

void main()
{
    vec3 texel = filter(Vert.texcoord);

	color = vec4(texel, 1.0);
}