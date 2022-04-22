
#version 150

in vec4 vPosition;
in vec3 vNormalF;
in vec3 vNormalG;
in vec4 vColorA;
in vec4 vColorD;
in vec4 vColorS;
out vec4 color;

uniform mat4 model_view;
uniform mat4 projection;
uniform mat4 scaling;
uniform mat4 rotateY;
uniform mat4 rotateZ;
uniform mat4 translate1;
uniform mat4 translate2;

uniform vec4 LightPosition;
uniform vec4 LightAmbient;
uniform vec4 LightSpecular;
uniform vec4 LightDiffuse;
uniform float Shininess; 
uniform int shadingMode;

void main() {

    vec3 vNormal = vNormalF;
    if(shadingMode == 1){
        vNormal = vNormalG;
    }

    // Transform vertex  position into eye coordinates
    vec3 pos = (model_view * vPosition).xyz;
	
    //vector to light source
    vec3 L = normalize( (LightPosition).xyz - pos ); 

    //vector to eye-viewer
    vec3 E = normalize( -pos );

    //halfway between light vector and viewer
    vec3 H = normalize( L + E );

    // Transform vertex normal into eye coordinates
    vec3 N = normalize( model_view * vec4(vNormal, 0.0) ).xyz;

    //Ambient color
    vec4 ambient = vColorA;

    //diffuse color
    float Kd = max( dot(L, N), 0.0 );
    vec4  diffuse = Kd * vColorD;

    //Specular color
    float Ks = pow( max(dot(N, H), 0.0), Shininess );
    vec4  specular = Ks * vColorS;
    
    if( dot(L, N) < 0.0 ) {
	    specular = vec4(0.0, 0.0, 0.0, 1.0);
	    diffuse = vec4(0.0, 0.0, 0.0, 1.0);
    } 

    gl_Position = projection * model_view * rotateY * rotateZ * scaling * translate2 * vPosition;
    color = ambient + diffuse + specular;
    color.a = 1.0;
    if(shadingMode == 2){
        color = vColorS;
    }
}
