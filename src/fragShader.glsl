#version 330 core


struct material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
const material gMaterial = material(
    vec3(1, 1, 1),
    vec3(1, 1, 1),
    vec3(1, 1, 1)
);

struct pointLight {
    vec3 ambient;
    vec3 diffuse;
    float specular;
    vec3 position;
};
const pointLight gPointLight = pointLight(
    vec3(0.2, 0.2, 0.2),
    vec3(1, 1, 1),
    40,
    vec3(512, 512, 256)
);

const vec4 attenuation = vec4(0, 0, 1, 1);


uniform bool useColor;
uniform bool useLight;
uniform vec3 camera;

in fragData {
    vec3 worldPos;
    vec3 normal;
    vec3 color;
} frag;


float attenuate(vec3 offset) {
    float distance = length(offset * attenuation.z);
    return 1 / (attenuation.w + attenuation.x * distance + attenuation.y * attenuation.y * distance);
}


vec3 objectColor() {
    if (useColor) {
        return frag.color;
    } else {
        return vec3((1 + frag.normal.x) / 2, (1 + frag.normal.y) / 2, frag.normal.z);
    }
}


vec3 ambient() {
    return gPointLight.ambient * gMaterial.ambient;
}

vec3 diffuse() {
    vec3 lightOffset = frag.worldPos - gPointLight.position;
    vec3 lightVec = normalize(lightOffset);
    float factor = clamp(dot(frag.normal, -lightVec), 0, 1);
    return gPointLight.diffuse * factor * gMaterial.diffuse * attenuate(lightOffset);
}

vec3 specular() {
    vec3 lightOffset = frag.worldPos - gPointLight.position;
    vec3 lightVec = normalize(lightOffset);
    vec3 fragEyeOffset = camera - frag.worldPos;
    vec3 fragEyeVec = normalize(fragEyeOffset);
    vec3 reflectVec = normalize(reflect(lightVec, frag.normal));
    float factor = pow(clamp(dot(reflectVec, fragEyeVec), 0, 1), gPointLight.specular);
    return gPointLight.specular * factor * gMaterial.specular * attenuate(lightOffset) * attenuate(fragEyeOffset);
}

vec3 light() {
    if (useLight) {
        return ambient() + diffuse() + specular();
    } else {
        return vec3(1, 1, 1);
    }
}


void main() {
    gl_FragColor = vec4(objectColor() * light(), 1);
}
