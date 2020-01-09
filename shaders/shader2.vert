#version 120

attribute vec3 vPositionModel; // in object space
attribute vec3 vNormalModel; // in object space

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 model_ti;
uniform vec3 eye;
uniform vec3 ka;


varying vec3 v_world;
varying vec3 v_normal;
varying vec3 v_color;

void main()
{
	v_color = ka;
	v_world = vec3(model * vec4(vPositionModel, 1.0));
	v_normal = normalize(vec3(model_ti * vec4(vNormalModel, 1.0)));
	gl_Position = projection * view * model * vec4(vPositionModel, 1.0);
}
