// F9
// текстуры
uniform sampler2D colorTexture, depthTexture;

// граница фильтрации
uniform float Border;
// параметр выделения границ
uniform float EdgeThreshold;

// параметры, полученные из вершинного шейдера
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

const vec3 factor = vec3(0.2126, 0.7152, 0.0722);

vec3 filter(in vec2 texcoord)
{
    float s00 = dot(factor, textureOffset(colorTexture, texcoord, ivec2(-1, 1)).rgb);
    float s10 = dot(factor, textureOffset(colorTexture, texcoord, ivec2(-1, 0)).rgb);
    float s20 = dot(factor, textureOffset(colorTexture, texcoord, ivec2(-1, -1)).rgb);
    float s01 = dot(factor, textureOffset(colorTexture, texcoord, ivec2(0, 1)).rgb);
    float s21 = dot(factor, textureOffset(colorTexture, texcoord, ivec2(0, -1)).rgb);
    float s02 = dot(factor, textureOffset(colorTexture, texcoord, ivec2(1, 1)).rgb);
    float s12 = dot(factor, textureOffset(colorTexture, texcoord, ivec2(1, 0)).rgb);
    float s22 = dot(factor, textureOffset(colorTexture, texcoord, ivec2(1, -1)).rgb);

    float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
    float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);

    float dist = sx * sx + sy * sy;

    return vec3(dist > EdgeThreshold * EdgeThreshold);
}

void main()
{
    vec3 texel = Vert.texcoord.x < Border ? filter(Vert.texcoord)
		: texture(colorTexture, Vert.texcoord).rgb;

	color = vec4(texel, 1.0);
}
