// текстуры
uniform sampler2D colorTexture, depthTexture;

// граница фильтрации
uniform float Border;
// параметр выделения границ
uniform float Threshold;

// параметры, полученные из вершинного шейдера
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

const vec3 factor = vec3(0.27, 0.67, 0.06);

vec3 filter(in vec2 texcoord)
{
	vec3 res = texture(colorTexture, Vert.texcoord).rgb;
	if(dot(factor, res) > Threshold)
		return vec3(res);
	else
		return vec3(0.0);
}

void main()
{
    vec3 texel = Vert.texcoord.x < Border ? filter(Vert.texcoord)
		: texture(colorTexture, Vert.texcoord).rgb;

	color = vec4(texel, 1.0);
}