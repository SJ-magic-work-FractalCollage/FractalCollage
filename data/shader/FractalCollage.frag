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

uniform float AveParts;
uniform int b_Enable_MosaicMix = 1; // true

uniform sampler2DRect texture_0; // fbo_Parts
uniform sampler2DRect texture_1; // fbo_Mosaic

/************************************************************
************************************************************/

/******************************
******************************/
void main(){
	/********************
	split imageの画素値(Gray)
	********************/
	vec2 Coord_2 = gl_TexCoord[0].xy;
	
	/* */
	int w1 = w0 - 2 * x_ofs;
	int h1 = h0 - 2 * y_ofs;
	
	float nw = float(w0) / w1;
	float nh = float(h0) / h1;
	
	vec2 Coord_1 = Coord_2 - vec2(x_ofs, y_ofs);
	
	/* */
	vec2 Coord_0;
	if(0 <= Coord_1.x)	Coord_0.x = mod(int(Coord_1.x), w1) * nw;
	else				Coord_0.x = (w1 - mod(int(abs(Coord_1.x)), w1)) * nw;
	
	if(0 <= Coord_1.y)	Coord_0.y = mod(int(Coord_1.y), h1) * nh;
	else				Coord_0.y = (h1 - mod(int(abs(Coord_1.y)), h1)) * nh;
	
	/* */
	vec4 color = texture2DRect( texture_0, Coord_0 );
	
	float val = 0.299 * color.r + 0.587 * color.g + 0.114 * color.b;
	
	/********************
	mosaic imageを反映
	********************/
	if(b_Enable_MosaicMix != 0){
		vec4 color_mosaic = texture2DRect(texture_1, Coord_2);
		val = val + (color_mosaic.r - AveParts);
		if(val < 0.0) val = 0;
		if(1.0 < val) val = 1.0;
	}
	
	color = vec4(val, val, val, 1.0);
	
	/********************
	********************/
	gl_FragColor = color;
}

