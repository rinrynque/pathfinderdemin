struct s_light
{
    vec4 position;
    vec4 ambient, diffuse, specular;
};

uniform s_light lighting;

varying vec3 N;
varying vec3 v;
void main(void)
{
   v = vec3(gl_ModelViewMatrix*gl_Vertex);
   N = normalize(gl_NormalMatrix * gl_Normal);
   gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
   gl_FrontColor = gl_Color;
}
