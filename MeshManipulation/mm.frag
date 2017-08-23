#version 400

in vec3 normal;
in vec3 fragPos;

out vec4 outColor;

void main()
{
	vec3 ambient = vec3(0.8,0.1,0.1);
	vec3 baseColor = vec3(1.0,0.4,0.4);
	vec3 lightColor = vec3(1.0);
	vec3 lightPos = vec3( 0.0, 5.0, 0.0 );
	vec3 norm = normalize(normal);
	vec3 lightDir = normalize( lightPos - fragPos );

	float diff = max(dot(norm,lightDir),0.0);
	vec3 diffuse = diff * lightColor;
	vec3 final = (ambient+diffuse)*baseColor;

	outColor = vec4( final,1.0 );
}