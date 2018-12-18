/************************************************************
ビルトイン関数(一部)
	http://qiita.com/edo_m18/items/71f6064f3355be7e4f45
************************************************************/
#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable


/************************************************************
************************************************************/
uniform sampler2DRect texture_0;

/************************************************************
************************************************************/

/******************************
******************************/
void main(){
	/* */
	vec2 Coord = gl_TexCoord[0].xy;
	
	/* */
	vec4 color = texture2DRect( texture_0, Coord );
	
	float val = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
	color = vec4(val, val, val, color.a);
	
	gl_FragColor = color;
}

