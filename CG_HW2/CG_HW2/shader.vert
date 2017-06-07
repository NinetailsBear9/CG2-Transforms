attribute vec4 av4position;
attribute vec3 av3color;
uniform int Cstate;

varying vec3 vv3color;

uniform mat4 mvp;

void main() {
	// NOTE!! column major
	
	mat3 colorFilter[4];
	colorFilter[0] = mat3(
		vec3(1, 0, 0),
		vec3(0, 1, 0),
		vec3(0, 0, 1)
	);
	colorFilter[1] = mat3(
		vec3(1, 0, 0),
		vec3(0, 0, 0),
		vec3(0, 0, 0)
	);
	colorFilter[2] = mat3(
		vec3(0, 0, 0),
		vec3(0, 1, 0),
		vec3(0, 0, 0)
	);
	colorFilter[3] = mat3(
		vec3(0, 0, 0),
		vec3(0, 0, 0),
		vec3(0, 0, 1)
	);
	vv3color = colorFilter[Cstate] * av3color;
	gl_Position = mvp * av4position;
}
