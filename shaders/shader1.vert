#version 120

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

attribute vec3 vPositionModel; // in object space
attribute vec3 vNormalModel; // in object space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model_ti;

uniform vec3 eye;

struct lightStruct
{
	vec3 position;
	vec3 color;
};

#define NUM_LIGHTS 2

uniform lightStruct lights[NUM_LIGHTS];

uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;

varying vec3 color;

struct spotLightStruct {
	bool enabled;
	mat4 viewMatrix;
	vec3 position;
	vec3 direction;
	float angle;
	float power;
};
uniform spotLightStruct spotLight;

void main()
{
	vec3 v_world = vec3(model * vec4(vPositionModel, 1.0));
	vec3 v_normal = normalize(vec3(model_ti * vec4(vNormalModel, 1.0)));
	vec3 E = normalize(eye - v_world);
	color = ka;
	
	float spotCoeff = 1.0;
	if (spotLight.enabled) {
		vec3 S = normalize(spotLight.position - v_world);
		vec3 A = normalize(spotLight.position - spotLight.direction);
		float theta = dot(S, A);
		spotCoeff = (theta < cos(radians(spotLight.angle))) ? 0.0 : pow(theta, spotLight.power);
	}

	for (int i = 0; i < NUM_LIGHTS; i++) {
		vec3 L = normalize(lights[i].position - v_world);
		vec3 R = normalize(reflect(-L, v_normal));
		float diffuse = max(dot(L, v_normal), 0.0);
		float specular = max(dot(R, E), 0.0);
		//float specular = 0.0;
		color += lights[i].color * (kd * diffuse + ks * pow(specular, s)) * spotCoeff;
	}

	gl_Position = projection * view * model * vec4(vPositionModel, 1.0);
}
