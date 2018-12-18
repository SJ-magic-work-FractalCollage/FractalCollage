/************************************************************
ビルトイン関数(一部)
	http://qiita.com/edo_m18/items/71f6064f3355be7e4f45
************************************************************/
#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable


/************************************************************
************************************************************/
uniform int w0;
uniform int h0;
uniform int x_ofs;
uniform int y_ofs;

uniform sampler2DRect texture_0;

/************************************************************
************************************************************/

/******************************
******************************/
void main(){
	/* */
	vec2 Coord_2 = gl_TexCoord[0].xy;
	
	/* */
	int w1 = w0 - 2 * x_ofs;
	int h1 = h0 - 2 * y_ofs;
	
	vec2 Coord_1 = Coord_2 - vec2(x_ofs, y_ofs);
	
	/* */
	vec2 Coord_0;
	if(0 <= Coord_1.x)	Coord_0.x = (int(Coord_1.x) / w1) * w1;
	else				Coord_0.x = ( (int(abs(Coord_1.x) - 1) / w1) + 1 ) * (-w1);
	
	if(0 <= Coord_1.y)	Coord_0.y = (int(Coord_1.y) / h1) * h1;
	else				Coord_0.y = ( (int(abs(Coord_1.y) - 1) / h1) + 1 ) * (-h1);
	
	/* */
	Coord_0 += vec2(x_ofs, y_ofs);
	if(Coord_0.x < 0)	Coord_0.x = 0;
	if(w0 <= Coord_0.x)	Coord_0.x = w0 - 1;
	if(Coord_0.y < 0)	Coord_0.y = 0;
	if(h0 <= Coord_0.y)	Coord_0.y = h0 - 1;
	
	/* */
	vec4 color = texture2DRect( texture_0, Coord_0 );
	
	float val = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
	color = vec4(val, val, val, 1.0);
	
	gl_FragColor = color;
}

