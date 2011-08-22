
uniform float distortion;
uniform int night;
varying vec4 diffuse,ambientGlobal,ambient;
varying vec3 normal,lightDir,halfVector;
varying float dist;



void main() { 
	vec3 tnormal, tlightDir; 
	vec4 tdiffuse, tambient, globalAmbient;
	float NdotL;
	if(night == 1) {
		vec4 ecPos;
		vec3 aux;
		
		normal = normalize(gl_NormalMatrix * gl_Normal);
		
		/* these are the new lines of code to compute the light's direction */
		ecPos = gl_ModelViewMatrix * gl_Vertex;
		aux = vec3(gl_LightSource[1].position-ecPos);
		lightDir = normalize(aux);
		dist = length(aux);
	
		halfVector = normalize(gl_LightSource[1].halfVector.xyz);
		
		/* Compute the diffuse, ambient and globalAmbient terms */
		diffuse = gl_FrontMaterial.diffuse * gl_LightSource[1].diffuse;
		
		/* The ambient terms have been separated since one of them */
		/* suffers attenuation */
		ambient = gl_FrontMaterial.ambient * gl_LightSource[1].ambient;
		ambientGlobal = gl_LightModel.ambient * gl_FrontMaterial.ambient;
		gl_TexCoord[0] = gl_MultiTexCoord0;
		
			
		gl_Position = ftransform();


	} else {
		tnormal = normalize(gl_NormalMatrix * gl_Normal);
		tlightDir = normalize(vec3(gl_LightSource[0].position));
		NdotL = max(dot(tnormal, tlightDir), 0.0);
		tdiffuse = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse; 
		tambient = gl_FrontMaterial.ambient * gl_LightSource[0].ambient;
		globalAmbient = gl_LightModel.ambient * gl_FrontMaterial.ambient;
		gl_FrontColor =  NdotL * tdiffuse + globalAmbient + tambient;
		gl_TexCoord[0] = gl_MultiTexCoord0;
	
	
		gl_Position = ftransform();
}
}
	
	
	








