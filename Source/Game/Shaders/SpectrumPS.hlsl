#include "../../Nova/Shaders/GltfModel.hlsli"

#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4

#define LINEAR 1

// PROJECTION_MAPPING
Texture2D fftTexture          : register(t16);

SamplerState samplerStates[8] : register(s0);

float4 main(VS_OUT pin) : SV_TARGET
{
    const float fftColorIntensity = 10;
    float3 fftColor = 0;
    const float bands = 30.0;
    const float segs = 40.0;
    float2 p;
#if 0
        p.x = projectionTexturePosition.x;
        p.y = projectionTexturePosition.y;
#else
    p.x = pin.texcoord.x; //  スペクトラムが出るが、位置を調整したい
    p.y = pin.texcoord.y;
#endif

    //  LEDColor
    //float4 projectionTextureColor = float4(lerp(float3(0.0, 2.0, 0.0), float3(2.0, 0.0, 0.0), sqrt(pin.texcoord.y)), 1.0);
    float4 fftTextureColor = float4(1, 0, 0, 1);
    float fft = fftTexture.Sample(samplerStates[LINEAR], float2(p.x, 0.0)).x;
    //return float4(fft, 0, 0, 1);
        
    // mask for bar graph
    float mask = (p.y < fft) ? 1.0 : 0.1;
    
    // led shape
    float2 d = frac((pin.texcoord - p) * float2(bands, segs)) - 0.5;
    float led = smoothstep(0.5, 0.35, abs(d.x)) *
                smoothstep(0.5, 0.35, abs(d.y));
    fftTextureColor *= mask;
    //fftTextureColor *= mask * led * fftColorIntensity;
    return fftTextureColor;
   
    fftColor = fftTextureColor.rgb * fftTextureColor.a * fftColorIntensity;
    
    // output final color
    //return float4(fftColor, 1.0);
    return 1;
    //return fftTextureColor;

}