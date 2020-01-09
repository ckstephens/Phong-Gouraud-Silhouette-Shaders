#version 120

uniform vec3 eye;
varying vec3 v_world;
varying vec3 v_normal;
varying vec3 v_color;

void main()
{
	vec3 f_color = v_color;
	vec3 E = normalize(eye - v_world);
	float NdotE = dot(v_normal, E);

	if (0.0 < NdotE && NdotE < 0.2)
		f_color = vec3(1.0);

	gl_FragColor = vec4(f_color, 1.0f);
}
