// F7
// текстуры
uniform sampler2D colorTexture, depthTexture;

// граница фильтрации
uniform float Border;

// параметры полученные из вершинного шейдера
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

const float PI = 3.14159265;
const float uD = 45.0, uR = 1.0;
const vec3 factor = vec3(0.27, 0.67, 0.06);

vec3 filter(in vec2 texcoord)
{
	vec2 pstep = vec2(1.0) / vec2(textureSize(colorTexture, 0));
	float Res = float( pstep.s ); // it’s a square texture image
	vec2 st = texcoord;
	float radius = Res * uR;
	vec2 xy = Res * st;
	vec2 dxy = xy - 0.5 * Res;
	float r = length(dxy);
	float beta = atan(dxy.x, dxy.y) + radians(uD) * (radius - r)/radius;
	vec2 xy1 = xy;
	if(r <= radius)
		xy1 = 0.5 * Res + r * vec2(cos(beta), sin(beta));
	st = xy1/Res;
	return vec3(texture(colorTexture, st).rgb);
}

void main(void)
{
	vec3 texel = Vert.texcoord.x < Border ? filter(Vert.texcoord)
		: texture(colorTexture, Vert.texcoord).rgb;

	color = vec4(texel, 1.0);
}
