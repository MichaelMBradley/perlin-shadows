#version 330 core


struct material_t {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
const material_t material = material_t(
    vec3(1, 1, 1),
    vec3(1, 1, 1),
    vec3(1, 1, 1)
);

struct pointLight_t {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float specularPower;
    vec3 position;
};

const vec4 attenuation = vec4(0, 0, 1, 1);


uniform bool useColor;
uniform bool useLight;
uniform bool useShadows;
uniform vec3 camera;

uniform pointLight_t pointLight;

uniform samplerCube depthMap;
uniform float farPlane;

uniform float minHeight;
uniform float maxHeight;

in fragData {
    vec3 worldPos;
    vec3 normal;
} frag;


float fog() {
    return 1 - smoothstep(farPlane * 0.97, farPlane, length(frag.worldPos - camera));
}


bool inShadow(vec3 lightOffset, float bias) {
    // Retrieved 2024/03/31, modified to fit program
    // https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows
    // By Joey de Vries (https://twitter.com/JoeyDeVriez)
    // CC BY 4.0 (https://creativecommons.org/licenses/by/4.0/legalcode)
    float existingShadowDepth = texture(depthMap, lightOffset).r;
    return length(lightOffset) + bias > existingShadowDepth * farPlane;
}

float inLight() {
    if (!useShadows) {
        return 1.0;
    }
    vec3 lightOffset = frag.worldPos - pointLight.position;
    float bias = -clamp(0.5 / dot(normalize(frag.normal), normalize(lightOffset)), 0.5, 10);
    float lightAmount = 0;
    for (int x = -1; x < 2; ++x) {
        for (int y = -1; y < 2; ++y) {
            for (int z = -1; z < 2; ++z) {
                if (!inShadow(lightOffset + vec3(x, y, z) / 4, bias)) {
                    lightAmount += 1.0;
                }
            }
        }
    }
    return pow(lightAmount / 27, 0.25);
}


float attenuate(vec3 offset) {
    float distance = length(offset * attenuation.z);
    return 1 / (attenuation.w + attenuation.x * distance + attenuation.y * attenuation.y * distance);
}


vec3 objectColor() {
    if (useColor) {
        float relativeHeight = (frag.worldPos.z - minHeight) / (maxHeight - minHeight);
        return vec3(relativeHeight, relativeHeight, 0.875);
    } else {
        return vec3((1 + frag.normal.x) / 2, (1 + frag.normal.y) / 2, frag.normal.z);
    }
}


vec3 ambient() {
    return pointLight.ambient * material.ambient;
}

vec3 diffuse() {
    vec3 lightOffset = frag.worldPos - pointLight.position;
    vec3 lightVec = normalize(lightOffset);
    float factor = clamp(dot(frag.normal, -lightVec), 0, 1);
    return pointLight.diffuse * factor * material.diffuse * attenuate(lightOffset);
}

vec3 specular() {
    vec3 lightOffset = frag.worldPos - pointLight.position;
    vec3 lightVec = normalize(lightOffset);
    vec3 fragEyeOffset = camera - frag.worldPos;
    vec3 fragEyeVec = normalize(fragEyeOffset);
    vec3 reflectVec = normalize(reflect(lightVec, frag.normal));
    float factor = pow(clamp(dot(reflectVec, fragEyeVec), 0, 1), pointLight.specularPower);
    return pointLight.specular * factor * material.specular * attenuate(lightOffset) * attenuate(fragEyeOffset);
}

vec3 light() {
    if (useLight && frag.worldPos != pointLight.position) {
        return fog() * (ambient() + inLight() * (diffuse() + specular()));
    } else {
        return pointLight.diffuse;
    }
}


void main() {
    gl_FragColor = vec4(objectColor() * light(), 1);
}
