////////////////////////////////////////////////////////////////////
//
// $Id: color.frag 2021/06/13 15:17:41 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#version 120

// Interpolated values from the vertex shaders
varying vec3 fragmentColor;

void main(){

	// Output color = color specified in the vertex shader, 
	// interpolated between all 3 surrounding vertices
	gl_FragColor = vec4(fragmentColor, 1);

}