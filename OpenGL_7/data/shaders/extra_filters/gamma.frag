// F8
// ��������
uniform sampler2D colorTexture, depthTexture;

// ������� ����������
uniform float Border;
// �������� �����-���������
uniform float Gamma;

// ���������, ���������� �� ���������� �������
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

vec3 filter(in vec2 texcoord)
{
	return vec3(pow(texture(colorTexture, texcoord).rgb, vec3(1.0/Gamma)));
}

void main()
{
    vec3 texel = Vert.texcoord.x < Border ? filter(Vert.texcoord)
		: texture(colorTexture, Vert.texcoord).rgb;

	color = vec4(texel, 1.0);
}
