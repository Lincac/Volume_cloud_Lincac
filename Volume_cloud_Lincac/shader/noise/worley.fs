#version 430 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec2 resolution;
uniform float scale;
uniform float iTime = 0.0;

vec2 random2(vec2 p){
    return fract(
                sin(
                    vec2(
                        dot(p,vec2(127.9898,311.233)),
                        dot(p,vec2(269.3265,183.7643))
                    ) + iTime
                ) * 43758.5453123
            );
}

float worley_noise(vec2 p){
    vec2 i = floor(p); // 返回一个值等于或小于参数的最接近的整数 获取网格索引
    vec2 f = fract(p); // 返回一个值等于参数的小数部分  获取网格内位置

    float feature = 1.0;
    for(int m=-1;m<=1;m++){
        for(int n=-1;n<=1;n++){
            vec2 offset = vec2(float(m),float(n));
            vec2 point = random2(i + offset); // 特征点位置
            float d = length(point + offset - f);
            feature = min(feature,d);
        }
    }
    return feature;
}

float noise(vec2 p){
    float noiseValue = 0.0;

    float a = 0.5;
    float f = 0.8;

    for (int i = 0; i < 5; i++) { // 四阶
        noiseValue += a * worley_noise(p * f);

        a *= 0.5;
        f *= 2.0;
    }

    return noiseValue * noiseValue;
}

void main(){
    vec2 seed = (gl_FragCoord.xy) / resolution;
    seed *= scale;
    FragColor = vec4(vec3(worley_noise(seed)), 1.0);
}