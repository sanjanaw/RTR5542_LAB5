attribute vec4 position; 
attribute vec4 normal; 
attribute vec4 colour; 
attribute vec4 mambient; //material Ka
attribute vec4 mdiffuse; //material Kd
attribute vec4 mspecular; //material Ks
attribute vec2 texCoord;

//varying vec4 pcolor; 
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

uniform mat4 local2clip;
uniform mat4 local2eye;
uniform mat4 world2eye;
uniform mat4 normalm;
uniform mat4 lmodel;
vec4 lpos1;

uniform vec4 lambient; //Ia
uniform vec4 ldiffuse; //Id
uniform vec4 lspecular; //Is
uniform float shine; //shininess

uniform vec4 lposition; //light position
vec4 lpos;
vec4 vpos;

void main(){    
      	gl_Position = local2clip * position;
	//pcolor = vec4(0,0,1,1); 
     	//pcolor = colour; 
	lpos1 = lmodel * lposition;
	lpos = world2eye * lpos1;  
	vpos = local2eye * position;

 	N=normalize(vec3(normalm*normal));
	L=normalize(vec3(lpos-vpos));
	R=normalize(reflect(-L, N));
	V=normalize(vec3(-vpos));
	shinef=shine;
	lambient1=lambient;
	mambient1=mambient;
	ldiffuse1=ldiffuse;
	mdiffuse1=mdiffuse;
	lspecular1=lspecular;
	mspecular1=mspecular;
	pos_in_eye = vpos;
	FtexCoord = texCoord;
}
