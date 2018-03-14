// ��������
uniform sampler2D originColorTex, originDepthTex;
uniform sampler2D newColorTex, newDepthTex;

// ������� ����������
uniform float Border;
// �������� �����-���������
uniform float Gamma;
// �������� ��������
uniform float MixCoef;

// ���������, ���������� �� ���������� �������
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

vec3 mix_filter(in vec2 texcoord)
{
	vec3 res = mix(texture(newColorTex, texcoord).rgb, texture(originColorTex, texcoord).rgb, MixCoef);
	return vec3(pow(res, vec3(1.0/Gamma)));
}

vec3 add_filter(in vec2 texcoord)
{
	vec3 res = texture(originColorTex, texcoord).rgb + (MixCoef - 1.0) * texture(newColorTex, texcoord).rgb;
	return vec3(pow(res, vec3(1.0/Gamma)));
}

void main()
{
    vec3 texel = Vert.texcoord.x < Border ? MixCoef < 1.0 ? mix_filter(Vert.texcoord) : add_filter(Vert.texcoord)
		: texture(originColorTex, Vert.texcoord).rgb;
	color = vec4(texel, 1.0);
}
