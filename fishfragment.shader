

uniform sampler2D tex;
uniform float distortion;
uniform int night;
varying vec4 diffuse,ambientGlobal, ambient;
varying vec3 normal,lightDir,halfVector;
varying float dist;



void main(){
	
	
	if (night == 1) {
		vec3 n,halfV,viewV,ldir;
		float NdotL,NdotHV;
		vec4 color = ambientGlobal+texture2D(tex,gl_TexCoord[0].st);
		
		float att;
		float spotEffect;
		
		/* a fragment shader can't write a varying variable, hence we need
		a new variable to store the normalized interpolated normal */
		n = normalize(normal);
		
		color = (ambient+texture2D(tex,gl_TexCoord[0].st))/(dist/1000.0);
	
	/* compute the dot product between normal and ldir */
		NdotL = max(dot(n,normalize(lightDir)),0.0);
		

		if (NdotL > 0.0) {
	
			spotEffect = dot(normalize(gl_LightSource[1].spotDirection), normalize(-lightDir));
			if (spotEffect > gl_LightSource[1].spotCosCutoff) {
		
		

			spotEffect = pow(spotEffect, gl_LightSource[1].spotExponent);
			att = spotEffect / (gl_LightSource[1].constantAttenuation +
					gl_LightSource[1].linearAttenuation * dist +
					gl_LightSource[1].quadraticAttenuation * dist * dist);
			
			color += att * (diffuse * NdotL + ambient);
	
		} else {
			/* If the spotlight isn't on the fish, then light it up, a little */
			color += (NdotL*ambient)/(dist/1000.0);
			  
		}
	
		
	}
	gl_FragColor = color;
	} else {
		gl_FragColor = gl_Color+texture2D(tex,gl_TexCoord[0].st);
	}
		

}




	
