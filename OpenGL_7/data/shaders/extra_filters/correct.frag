// F1
// ��������
uniform sampler2D colorTexture, depthTexture;

// ������� ����������
uniform float Border;
// �������������� �������
uniform mat3 Correct;

// ��������� ���������� �� ���������� �������
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

const vec3 factor = vec3(0.27, 0.67, 0.06);

vec3 filter(in vec2 texcoord)
{
	return vec3(Correct * texture(colorTexture, texcoord).rgb);
}

void main(void)
{
	vec3 texel = Vert.texcoord.x < Border ? filter(Vert.texcoord)
		: texture(colorTexture, Vert.texcoord).rgb;

	color = vec4(texel, 1.0);
}
