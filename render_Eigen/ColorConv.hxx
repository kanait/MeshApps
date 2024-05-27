////////////////////////////////////////////////////////////////////
//
// $Id: ColorConv.hxx 2021/06/20 13:41:26 kanai Exp $
//
// Copyright (c) 2021 Takashi Kanai
// Released under the MIT license
//
////////////////////////////////////////////////////////////////////

#ifndef _COLORCONV_HXX
#define _COLORCONV_HXX 1

#include <cmath>
using namespace std;

#include "myEigen.hxx"

class ColorConv {

public:

  ColorConv(){
    Eigen::Vector3f a( 0.0f, .0f, 1.0f );
    Eigen::Vector3f b( 1.0f, .0f, 0.0f );
    rgbtohsv( a, max_hsv_ ); // blue
    rgbtohsv( b, min_hsv_ ); // red
    setMinMaxValue( .0f, 1.0f );
  };
  ~ColorConv(){};

  void interpColorRGB( float t, float* col ) {

    float p = (t - min_val_) / div_;

    // linear interpolation
    Eigen::Vector3f int_hsv = (1.0f - p) * min_hsv_ + p * max_hsv_;
    Eigen::Vector3f int_rgb;
    hsvtorgb( int_hsv, int_rgb );

    col[0] = int_rgb[0];
    col[1] = int_rgb[1];
    col[2] = int_rgb[2];
  };

  void interpColorRGBUC( float t, unsigned char* col ) {

    float p = (t - min_val_) / div_;

    // linear interpolation
    Eigen::Vector3f int_hsv = (1.0f - p) * min_hsv_ + p * max_hsv_;
    Eigen::Vector3f int_rgb;
    hsvtorgb( int_hsv, int_rgb );

    // clamp
    if ( int_rgb[0] < .0f )  int_rgb[0] = .0f;
    if ( int_rgb[0] > 1.0f ) int_rgb[0] = 1.0f;
    if ( int_rgb[1] < .0f )  int_rgb[1] = .0f;
    if ( int_rgb[1] > 1.0f ) int_rgb[1] = 1.0f;
    if ( int_rgb[2] < .0f )  int_rgb[2] = .0f;
    if ( int_rgb[2] > 1.0f ) int_rgb[2] = 1.0f;

    // convert to unsigned char
    col[0] = (unsigned char) (int_rgb[0] * 255.0f);
    col[1] = (unsigned char) (int_rgb[1] * 255.0f);
    col[2] = (unsigned char) (int_rgb[2] * 255.0f);
  };

  static void hsvtorgb( Eigen::Vector3f& hsv, Eigen::Vector3f& rgb ) {

    if ( hsv[1] >= 1.0f ) hsv[1] = 1.0f;
    else if ( hsv[1] <= 0.0f ) hsv[1] = 0.0f;

    if ( hsv[2] >= 1.0 ) hsv[2] = 1.0f;
    else if ( hsv[2] <= 0.0f ) hsv[2] = 0.0f;

    if ( hsv[1] == 0.0 )
      {
        rgb = Eigen::Vector3f( hsv[2], hsv[2], hsv[2] );
      }
    else
      {
        float hh = hsv[0] * 3.0 / M_PI;
        int i = (int) hh;
        if ( i > 5 )
          {
            hsv[0] -= 6.0;
            i = 0;
        }
        float f = hsv[0] - (double) i;
        float p = hsv[2] * (1.0 - hsv[1]);
        float q = hsv[2] * (1.0 - hsv[1] * f);
        float t = hsv[2] * (1.0 - hsv[1] * (1.0 - f));

        if ( i == 0 )
          {
            rgb = Eigen::Vector3f( hsv[2], t, p );
          }
        else if ( i == 1 )
          {
            rgb = Eigen::Vector3f( q, hsv[2], p );
          }
        else if ( i == 2 )
          {
            rgb = Eigen::Vector3f( p, hsv[2], t );
          }
        else if ( i == 3 )
          {
            rgb = Eigen::Vector3f( p, q, hsv[2] );
          }
        else if ( i == 4 )
          {
            rgb = Eigen::Vector3f( t, p, hsv[2] );
          }
        else
          {
            rgb = Eigen::Vector3f( hsv[2], p, q );
          }
      }
  };

  static void rgbtohsv( Eigen::Vector3f& rgb, Eigen::Vector3f& hsv ) {

    // clamps ( 0.0f, 1.0f )
    if ( rgb[0] > 1.0f ) rgb[0] = 1.0f;
    if ( rgb[0] < .0f ) rgb[0] = .0f;
    if ( rgb[1] > 1.0f ) rgb[1] = 1.0f;
    if ( rgb[1] < .0f ) rgb[1] = .0f;
    if ( rgb[2] > 1.0f ) rgb[2] = 1.0f;
    if ( rgb[2] < .0f ) rgb[2] = .0f;

    float maxrgb = rgb[0];
    if ( maxrgb < rgb[1] ) maxrgb = rgb[1];
    if ( maxrgb < rgb[2] ) maxrgb = rgb[2];
    float minrgb = rgb[0];
    if ( minrgb > rgb[1] ) minrgb = rgb[1];
    if ( minrgb > rgb[2] ) minrgb = rgb[2];

    hsv[2] = maxrgb;

    if (maxrgb != 0.0)
      {
        hsv[1] = (maxrgb - minrgb) / maxrgb;
      }
    else
      hsv[1] = 0.0f;

    if (hsv[1] == 0.0f)
      hsv[0] = 0.0f;
    else
      {
        float delta = maxrgb - minrgb;
        if ( rgb[0] == maxrgb )
          hsv[0] = (rgb[1] - rgb[2]) / delta;
        else if ( rgb[1] == maxrgb )
          hsv[0] = 2.0f + (rgb[2] - rgb[0]) / delta;
        else if ( rgb[2] == maxrgb )
          hsv[0] = 4.0f + (rgb[0] - rgb[1]) / delta;
        hsv[0] *= M_PI / 3.0f;
        if (hsv[0] < 0.0f)
          hsv[0] += 2.0f * M_PI;
      }
  };

  void setMinMaxValue( float min_val, float max_val ) {
    max_val_ = max_val;
    min_val_ = min_val;
    div_ = max_val_ - min_val_;
  };

private:

  Eigen::Vector3f max_hsv_;
  Eigen::Vector3f min_hsv_;
  float max_val_;
  float min_val_;
  float div_;

};

#endif
