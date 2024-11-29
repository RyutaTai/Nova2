#include "../../Nova/Shaders/FullscreenQuad.hlsli"

#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4

#define LINEAR 1

#define M_PI 3.14159265359
#define FFT_BLOCK_COUNT 2048

#define SCREEN_WIDTH  1980
#define SCREEN_HEIGHT 1080

// PROJECTION_MAPPING
SamplerState samplerStates[8] : register(s0);

cbuffer fftConstant : register(b10)
{
    //float data[2048];
    float4 data[FFT_BLOCK_COUNT / 4];
}

float4 capsule(float4 color, float4 background, float4 region, float2 uv);
float2 rotatePoint(float2 pointA, float2 center, float angle);
float4 bar(float4 color, float4 background, float2 position, float2 diemensions, float2 uv);
float4 rays(float4 color, float4 background, float2 position, float radius, float rays, float ray_length, float fft, float2 uv);

float4 main(VS_OUT pin):SV_TARGET
{
    //Prepare UV and background
    float aspect = SCREEN_WIDTH / SCREEN_HEIGHT;
    float2 texcoord = pin.texcoord;
    texcoord.x *= aspect;
    float4 color = lerp(float4(0.0, 1.0, 0.8, 1.0), float4(0.0, 0.3, 0.25, 1.0), distance(float2(aspect / 2.0, 0.5), texcoord));
    
    //VISUALIZER PARAMETERS
    const float RAYS = 96.0;    //  number of rays //Please, decrease this value if shader is working too slow
    float RADIUS = 0.4;         //  max circle radius
    float RAY_LENGTH = 0.3;     //  ray's max length //increased by 0.1
    
    //  fft
    uint t = texcoord.x * FFT_BLOCK_COUNT / 4;
    float4 amp = data[t];
    float offset = texcoord.x * FFT_BLOCK_COUNT;
    float fft = amp[(int) offset % 4];
    
    color = rays(float4(1, 1, 1, 1), color, float2(aspect / 2.0, 1.0 / 2.0), RADIUS, RAYS, RAY_LENGTH, fft, texcoord);

    return float4(color.xyz, 1.0);
}

float4 rays(float4 color, float4 background, float2 position, float radius, float rays, float ray_length, float fft, float2 uv)
{
    float inside = (1.0 - ray_length) * radius; //  empty part of circle
    float outside = radius - inside;            //  rest of circle
    float circle = 2.0 * M_PI * inside;         //  circle lenght
    for (int i = 1; float(i) <= rays; i++)
    {
        float len = outside * fft; //    length of actual ray
        background = bar(color, background, float2(position.x, position.y + inside), float2(circle / (rays * 2.0), len), rotatePoint(uv, position, 360.0 / rays * float(i))); //Added capsules
    }
    return background; //output
}

float4 bar(float4 color, float4 background, float2 position, float2 diemensions, float2 uv)
{
    return capsule(color, background, float4(position.x, position.y + diemensions.y / 2.0, diemensions.x / 2.0, diemensions.y / 2.0), uv); //Just transform rectangle a little
}

float4 capsule(float4 color, float4 background, float4 region, float2 uv) //    capsule
{
    if (uv.x > (region.x - region.z) && uv.x < (region.x + region.z) &&
       uv.y > (region.y - region.w) && uv.y < (region.y + region.w) ||
       distance(uv, region.xy - float2(0.0, region.w)) < region.z ||
       distance(uv, region.xy + float2(0.0, region.w)) < region.z)
        return color;
    return background;
}

float2 rotatePoint(float2 pointA, float2 center, float angle) //rotating point around the center
{
    float s = sin(radians(angle));
    float c = cos(radians(angle));
    
    pointA.
    x -= center.x;
    pointA.
    y -= center.y;
    
    float x = pointA.
    x * c - pointA.
    y * s;
    float y = pointA.
    x * s + pointA.
    y * c;
    
    pointA.
    x = x + center.x;
    pointA.
    y = y + center.y;
    
    return pointA;
}


#if 0
float4 main(VS_OUT pin) : SV_TARGET
{
    float2 texcoord = float2(pin.texcoord.x, pin.texcoord.y);
    // quantize coordinates
    const float bands = 30.0;
    const float segs = 40.0;
    float2 p;
    p.x = floor(texcoord.x * bands) / bands;
    p.y = floor(texcoord.y * segs) / segs;

    // read frequency data from first row of texture
    uint t = texcoord.x * FFT_BLOCK_COUNT / 4;
    float4 amp = data[t];
    float offset = texcoord.x * FFT_BLOCK_COUNT;
    float fft = amp[(int) offset % 4];
    
    
    float3 fft_color = float3(1.0, 0.0, 0.0);
    //float3 fft_color = float3(0.8, 0.6, 0.3);
    if (pin.texcoord.y > 0.5)
    {
        float y = (pin.texcoord.y - 0.5) * 2.0;
        //return float4(step(y, fft), 0, 0, 1);
        fft_color *= step(y, fft);
        
    }
    else if (pin.texcoord.y < 0.5)
    {
        float y = (0.5 - pin.texcoord.y) * 2.0;
        //return float4(step(y, fft), 0, 0, 1);
        fft_color *= step(y, fft);

    }
    
    return float4(fft_color, 0.5);
    
    
    
    
    //float fft = texture(iChannel0, vec2(p.x, 0.0)).x;
    // led color
    float3 color = lerp(float3(0.0, 2.0, 0.0), float3(2.0, 0.0, 0.0), sqrt(texcoord.y));

    // mask for bar graph
    float mask = (p.y < fft) ? 1.0 : 0.1;

    // led shape
    float2 d = frac((texcoord - p) * float2(bands, segs)) - 0.5;
    float led = smoothstep(0.5, 0.35, abs(d.x)) *
                smoothstep(0.5, 0.35, abs(d.y));
    float3 ledColor = led * color * mask;

    // output final color
    return float4(ledColor, 1.0);
    
    //uint t = pin.texcoord.x * FFT_BLOCK_COUNT / 4;
    //float4 amp = data[t];
    //float offset = pin.texcoord.x * 2048;
    //float v = amp[(int) offset % 4];
    //return float4(v, 0, 0, 1);
    ////float amp = data[pin.position.x / 4];
    
    
    if (pin.position.x % 4 == 0)        amp = data[t].x;
    else if (pin.position.x % 4 == 1)   amp = data[t].y;
    else if (pin.position.x % 4 == 2)   amp = data[t].z;
    else if (pin.position.x % 4 == 3)   amp = data[t].w;
    //return float4(1,0,0, 1);      
    //return float4(amp, amp, amp, 1);
    
    //const float fftColorIntensity = 10.0;
    //const float bands = 30.0;
    //const float segs = 40.0;
    //float3 fftColor = 0;
    //float2 p;
    
    //p.x = pin.texcoord.x;
    //p.y = pin.texcoord.y;

    ////  LEDColor
    //float4 fftTextureColor = float4(lerp(float3(0.0, 2.0, 0.0), float3(2.0, 0.0, 0.0), sqrt(pin.texcoord.y)), 1.0);
    ////float4 fftTextureColor = float4(1, 0, 0, 1);
    //float fft = amp;
    ////return float4(fft, 0, 0, 1);
    
    //// mask for bar graph
    //float mask = (p.y < fft) ? 1.0 : 0.1;
    
    //// led shape
    //float2 d = frac((pin.texcoord - p) * float2(bands, segs)) - 0.5;
    //float led = smoothstep(0.5, 0.35, abs(d.x)) *
    //            smoothstep(0.5, 0.35, abs(d.y));
    //fftTextureColor *= mask;
    ////fftTextureColor *= mask * led * fftColorIntensity;

    //fftColor = fftTextureColor.rgb * fftTextureColor.a * fftColorIntensity ;
    
    //// output final color
    //return float4(fftColor, 1.0);
    ////return 1;
    //return fftTextureColor;

}
#endif