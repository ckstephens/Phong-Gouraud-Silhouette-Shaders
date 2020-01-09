#version 120

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

varying vec3 v_world;
varying vec3 v_normal;
varying vec3 v_color;

struct spotLightStruct {
	bool enabled;
	vec3 position;
	vec3 direction;
	float angle;
	float power;
};

uniform spotLightStruct spotLight;

void main()
{
	vec3 f_color = v_color;
	vec3 E = normalize(eye - v_world);

	vec3 S = normalize(spotLight.position - v_world);
	vec3 A = normalize(spotLight.position - spotLight.direction);
	float theta = dot(S, A);
	float spotCoeff = 1.0;
	if (spotLight.enabled)
		spotCoeff = (theta < cos(radians(spotLight.angle))) ? 0.0 : pow(theta, spotLight.power);

	for (int i = 0; i < NUM_LIGHTS; i++) {
		vec3 L = normalize(lights[i].position - v_world);
		vec3 R = normalize(reflect(-L, v_normal));
		float diffuse = max(dot(L, v_normal), 0.0);
		float specular = max(dot(R, E), 0.0);
		//float specular = 0.0;
		f_color += lights[i].color * (kd * diffuse + ks * pow(specular, s)) * spotCoeff;
	}
	gl_FragColor = vec4(f_color, 1.0f);
}
