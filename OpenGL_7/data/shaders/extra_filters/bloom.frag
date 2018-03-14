// F9
// текстуры
uniform sampler2D colorTexture, depthTexture;

// граница фильтрации
uniform float Border;
// параметр выделения границ
uniform float LumThresh; //Luminance threshold

// параметры, полученные из вершинного шейдера
in Vertex
{
	vec2 texcoord;
} Vert;

layout(location = FRAG_OUTPUT0) out vec4 color;

uniform float PixOffset[10] = float[](0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0);
uniform float Weight[10];

const vec3 factor = vec3(0.2126, 0.7152, 0.0722);

// Pass to extract the bright parts
subroutine( RenderPassType )
vec4 pass2()
{
    vec4 val = texture(RenderTex, TexCoord);
    if( dot(factor, val.rgb) > LumThresh )
        return val * 0.5;
    else
        return vec4(0.0);
}

// First blur pass
subroutine( RenderPassType )
vec4 pass3()
{
    float dy = 1.0 / float(Height);

    vec4 sum = texture(BlurTex, TexCoord) * Weight[0];
    for( int i = 1; i < 10; i++ )
    {
         sum += texture( BlurTex, TexCoord + vec2(0.0,PixOffset[i]) * dy ) * Weight[i];
         sum += texture( BlurTex, TexCoord - vec2(0.0,PixOffset[i]) * dy ) * Weight[i];
    }
    return sum;
}

// Second blur and add to original
subroutine( RenderPassType )
vec4 pass4()
{
    float dx = 1.0 / float(Width);

    vec4 val = texture(RenderTex, TexCoord);
    vec4 sum = texture(BlurTex, TexCoord) * Weight[0];
    for( int i = 1; i < 10; i++ )
    {
       sum += texture( BlurTex, TexCoord + vec2(PixOffset[i],0.0) * dx ) * Weight[i];
       sum += texture( BlurTex, TexCoord - vec2(PixOffset[i],0.0) * dx ) * Weight[i];
    }
    return val + (sum * sum.a);
}

void main()
{
	
}
