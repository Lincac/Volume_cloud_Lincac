#version 430 core
out vec4 FragColor;
in vec2 TexCoords;

uniform vec2 resolution;
uniform float scale;
uniform float iTime = 0.0;

float random(vec2 p){
    return fract(sin(dot(p,vec2(12.9898,78.233)) + iTime) * 43758.5453123);
}

vec2 random2(vec2 p){
    return fract(
                sin(
                    vec2(
                        dot(p,vec2(127.9898,311.233)),
                        dot(p,vec2(269.3265,124.7643))
                    ) + iTime
                ) * 43758.5453123
            ) * 2. - 1.;
}

float perlin_noise(vec2 p){
    vec2 i = floor(p); // 返回一个值等于或小于参数的最接近的整数 获取网格索引
    vec2 f = fract(p); // 返回一个值等于参数的小数部分  获取网格内位置

    float p0 = random( i );
    float p1 = random( i + vec2( 1.0, 0.0 ) );
    float p2 = random( i + vec2( 0.0, 1.0 ) );
    float p3 = random( i + vec2( 1.0, 1.0 ) );

    vec2 u = f*f*(3.0-2.0*f); // 差值系数

    return mix(mix(p0,p1,u.x),mix(p2,p3,u.x),u.y); 
}

float perlin_noise2(vec2 p){
    vec2 i = floor(p); 
    vec2 f = fract(p); 

    float p0 = dot(random2( i ),f);
    float p1 = dot(random2( i + vec2( 1.0, 0.0 ) ),f - vec2( 1.0, 0.0 ) );
    float p2 = dot(random2( i + vec2( 0.0, 1.0 ) ),f - vec2( 0.0, 1.0 ) );
    float p3 = dot(random2( i + vec2( 1.0, 1.0 ) ),f - vec2( 1.0, 1.0 ) );

    vec2 u = f*f*(3.0-2.0*f);

    return mix(mix(p0,p1,u.x),mix(p2,p3,u.x),u.y); 
}

float noise(vec2 p){ // fbm细节添加，通过频率和振幅的叠加添加细节
    float noiseValue = 0.0;

    float a = 0.5;
    float f = 0.8;

    for (int i = 0; i < 5; i++) { // 四阶
        noiseValue += a * perlin_noise(p * f);

        a *= 0.5;
        f *= 2.0;
    }

    return noiseValue;
}

void main(){
    vec2 seed = (gl_FragCoord.xy) / resolution;
    seed *= scale;
    float t = noise(seed);
    FragColor = vec4(t,t,t,1.0);
}