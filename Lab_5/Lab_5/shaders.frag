//varying vec4 pcolor; 
uniform int useTexture;

vec4 ambient;
vec4 diffuse;
vec4 specular;
varying vec4 lambient1;
varying vec4 mambient1;
varying vec4 ldiffuse1;
varying vec4 mdiffuse1;
varying vec4 lspecular1;
varying vec4 mspecular1;
varying vec3 N;
varying vec3 L;
varying vec3 R;
varying vec3 V;
varying float shinef;

varying vec2 FtexCoord;
varying vec4 pos_in_eye;

uniform sampler2D Tex1;
uniform samplerCube cubeMap;
uniform mat4 eye2world;

float NdotL; 
float RdotV;
vec3 N1, V1, L1, R1;

void main() { 
	vec4 light;
	vec3 view_vector;
	vec3 ref;
	vec4 texColour;
	ambient = lambient1 * mambient1;
	
	//N1 = normalize(N);
	//R1 = normalize(R);
	//L1 = normalize(L);
	//V1 = normalize(V);
	if (dot(N,L) <0.0) NdotL= 0.0; 
	else NdotL=dot(N,L);
	diffuse = ldiffuse1 * mdiffuse1 * NdotL;
	 
	RdotV= dot(R,V);
	if (NdotL== 0.0) RdotV = 0.0; 
	if (dot(R,V) < 0.0) RdotV = 0.0;
	specular = lspecular1 * mspecular1 * pow(RdotV,shinef);
	light = ambient+diffuse+specular;

	if(useTexture==0)
		gl_FragColor = light;
	if(useTexture == 1){
		texColour = texture2D(Tex1, FtexCoord);
		gl_FragColor = texColour;
	}
	if(useTexture==2){
		view_vector = normalize(vec3(vec4(0,0,0,1)-pos_in_eye));
		ref = normalize(reflect(-view_vector,N));
		ref = vec3(eye2world * vec4(ref,0));
		gl_FragColor = textureCube(cubeMap, ref);
	}
} 
 