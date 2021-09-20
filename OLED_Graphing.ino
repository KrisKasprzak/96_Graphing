/*

  This program provides functions to draw:
  1. horizontal bar graph
  2. vertical bar graph
  3. dial
  4. cartesian graph

  It requires an microcrontrioller Arduino, teensy and a 0.96 dispaly (1306 driver chip)

  display. sources
  https://www.amazon.com/Diymall-Yellow-Arduino-display.-Raspberry/dp/B00O2LLT30/ref=pd_lpo_vtph_147_bs_t_1?_encoding=UTF8&psc=1&refRID=BQAPE07SVJQNDKEVMTQZ
  https://www.amazon.com/dp/B06XRCQZRX/ref=sspa_dk_detail_0?psc=1&pd_rd_i=B06XRCQZRX&pd_rd_wg=aGuhK&pd_rd_r=1WJPXAG68XFSADGDPNZR&pd_rd_w=DAAsB

  Adafruit libraries
  https://github.com/adafruit/Adafruit-GFX-Library/archive/master.zip
  https://github.com/adafruit/Adafruit_SSD1306

  Revisions
  rev     date        author      description
  1       12-5-2017   kasprzak    initial creation


  Pin settings

  Arduino   device
  5V        Vin
  GND       GND
  A0        read an input voltage from a pot across Vcc and Gnd (just for demo purposes)
  A1
  A2
  A3
  A4        SDA on the display. or use dedicated pin on board if provided
  A5        SCL on the display. use dedicated pin on board if provided
  1
  2
  3
  4
  5
  6
  7
  8
  9
  10
  11
  12
  13


*/

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define READ_PIN            A0
#define OLED_RESET          A4
#define LOGO16_GLCD_HEIGHT  16
#define LOGO16_GLCD_WIDTH   16

// create what ever variables you need
double volts;
double bvolts;
double x, y;

// these are a required variables for the graphing functions
bool Redraw1 = true;
bool Redraw2 = true;
bool Redraw3 = true;
bool Redraw4 = true;
double ox , oy ;

// create the display object
Adafruit_SSD1306 display(OLED_RESET);

void setup() {

  Serial.begin(9600);

  // initialize the display
  // note you may have to change the address
  // the most common are 0X3C and 0X3D

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  // if you distribute your code, the adafruit license requires you show their splash screen
  // otherwise clear the video buffer memory then display
  display.display();
  //delay(2000);
  display.clearDisplay();
  display.display();

  // establish whatever pin reads you need
  pinMode(A0, INPUT);

}

unsigned long OldTime;
unsigned long counter;


  void loop(void) {

  // read some values and convert to volts
  bvolts = analogRead(A0);
  volts = (bvolts  / 204.6 ) ;


  // draw any of the following, sorry for the crazy long arguement lists
  // note 0.96 isn't very big, so you can't show many graphs at the same time
  // also colors are hard coded
  // the display i use draws text at the top in yellow--we'll use that for drawing the title


  // call one of these functions
  DrawBarChartV(display, bvolts, 25,  60, 40, 40, 0, 1024 , 256, 0, "A0 (bits)", Redraw1);
  //  DrawBarChartH(display, volts, 10, 45, 100, 20, 0, 5, 1, 0, "A0 (volts)", Redraw2);
  //  DrawDial(display, volts, 65, 50, 25, 0, 5 , 1, 0, 200, "A0 (volts)", Redraw3);

  // for multiple graphs uncomment the next 2 lines
  //  DrawBarChartV(display, bvolts, 5,  60, 10, 40, 0, 1024 , 256, 0, "A0 (bits/volts)", Redraw1);
  //  DrawDial(display, volts, 90, 50, 25, 0, 5 , 1, 0, 200, "A0 (bits/volts)", Redraw3);

  // or show a cartesian style graph to plot values over time (or whatever)

    DrawCGraph(display, x++, bvolts, 30, 50, 75, 30, 0, 100, 25, 0, 1024, 512, 0, "Bits vs Seconds", Redraw4);
    if (x > 100) {
    while (1) {}
    }


  delay(1000);


  }



  /*
  This method will draw a vertical bar graph for single input
  it has a rather large arguement list and is as follows

  &d to pass the display object, mainly used if multiple displays are connected to the MCU
  curval = date to graph (must be between loval and hival)
  x = position of bar graph (lower left of bar)
  y = position of bar (lower left of bar
  w = width of bar graph
  h =  height of bar graph (does not need to be the same as the max scale)
  loval = lower value of the scale (can be negative)
  hival = upper value of the scale
  inc = scale division between loval and hival
  dig = format control to set number of digits to display. (not includeing the decimal)
  label = bottom lable text for the graph
  redraw = flag to redraw display. only on first pass (to reduce flickering)

*/

void DrawBarChartV(Adafruit_SSD1306 &d, double curval, double x , double y , double w, double h , double loval , double hival , double inc , double dig, String label, bool &Redraw)
{
  double stepval, my, level, i, data;

  if (Redraw) {
    Redraw = false;
    d.fillRect(0, 0,  127 , 14, SSD1306_WHITE);
    d.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    d.setTextSize(1);
    d.setCursor(2, 4);
    d.println(label);
    // step val basically scales the hival and low val to the height
    // deducting a small value to eliminate round off errors
    // this val may need to be adjusted
    stepval = ( inc) * (double (h) / (double (hival - loval))) - .001;
    for (i = 0; i <= h; i += stepval) {
      my =  y - h + i;
      d.drawFastHLine(x + w + 1, my,  5, SSD1306_WHITE);
      // draw lables
      d.setTextSize(1);
      d.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
      d.setCursor(x + w + 12, my - 3 );
      data = hival - ( i * (inc / stepval));
      d.print(data, dig);
    }
  }
  // compute level of bar graph that is scaled to the  height and the hi and low vals
  // this is needed to accompdate for +/- range
  level = (h * (((curval - loval) / (hival - loval))));
  // draw the bar graph
  // write a upper and lower bar to minimize flicker cause by blanking out bar and redraw on update
  d.drawRect(x, y - h, w, h, SSD1306_WHITE);
  d.fillRect(x, y - h, w, h - level,  SSD1306_BLACK);
  d.drawRect(x, y - h, w, h, SSD1306_WHITE);
  d.fillRect(x, y - level, w,  level, SSD1306_WHITE);
  // up until now print sends data to a video buffer NOT the screen
  // this call sends the data to the screen
  d.display();

}

/*
  This method will draw a horizontal bar graph for single input
  it has a rather large arguement list and is as follows

  &display to pass the display object, mainly used if multiple displays are connected to the MCU
  x = position of bar graph (upper left of bar)
  curval = date to graph (must be between loval and hival)
  y = position of bar (upper left of bar (add some vale to leave room for label)
  w = width of bar graph (does not need to be the same as the max scale)
  h =  height of bar graph
  loval = lower value of the scale (can be negative)
  hival = upper value of the scale
  inc = scale division between loval and hival
  dig = format control to set number of digits to display. (not includeing the decimal)
  label = bottom lable text for the graph
  redraw = flag to redraw display. only on first pass (to reduce flickering)
*/

void DrawBarChartH(Adafruit_SSD1306 &d, double curval, double x , double y , double w, double h , double loval , double hival , double inc , double dig, String label, bool &Redraw)
{
  double stepval, mx, level, i, data;

  if (Redraw) {
    Redraw = false;
    d.fillRect(0, 0,  127 , 16, SSD1306_WHITE);
    d.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    d.setTextSize(1);
    d.setCursor(2, 4);
    d.println(label);
    // step val basically scales the hival and low val to the width
    stepval =  inc * (double (w) / (double (hival - loval))) - .00001;
    // draw the text
    for (i = 0; i <= w; i += stepval) {
      d.drawFastVLine(i + x , y ,  5, SSD1306_WHITE);
      // draw lables
      d.setTextSize(1);
      d.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
      d.setCursor(i + x , y + 10);
      // addling a small value to eliminate round off errors
      // this val may need to be adjusted
      data =  ( i * (inc / stepval)) + loval + 0.00001;
      d.print(data, dig);
    }
  }
  // compute level of bar graph that is scaled to the width and the hi and low vals
  // this is needed to accompdate for +/- range capability
  // draw the bar graph
  // write a upper and lower bar to minimize flicker cause by blanking out bar and redraw on update
  level = (w * (((curval - loval) / (hival - loval))));
  d.fillRect(x + level, y - h, w - level, h,  SSD1306_BLACK);
  d.drawRect(x, y - h, w,  h, SSD1306_WHITE);
  d.fillRect(x, y - h, level,  h, SSD1306_WHITE);
  // up until now print sends data to a video buffer NOT the screen
  // this call sends the data to the screen
  d.display();

}


/*
  This method will draw a dial-type graph for single input
  it has a rather large arguement list and is as follows

  &display to pass the display object, mainly used if multiple displays are connected to the MCU
  curval = date to graph (must be between loval and hival)
  cx = center position of dial
  cy = center position of dial
  r = radius of the dial
  loval = lower value of the scale (can be negative)
  hival = upper value of the scale
  inc = scale division between loval and hival
  sa = sweep angle for the dials scale
  dig = format control to set number of digits to display. (not includeing the decimal)
  label = bottom lable text for the graph
  redraw = flag to redraw display. only on first pass (to reduce flickering)
*/

void DrawDial(Adafruit_SSD1306 &d, double curval, int cx, int cy, int r, double loval , double hival , double inc, double dig, double sa, String label, bool &Redraw) {

  double ix, iy, ox, oy, tx, ty, lx, rx, ly, ry, i, Offset, stepval, data, angle;
  double degtorad = .0174532778;
  static double px = cx, py = cy, pix = cx, piy = cy, plx = cx, ply = cy, prx = cx, pry = cy;

  if (Redraw) {
    Redraw = false;
    // draw the dial only one time--this will minimize flicker
    d.fillRect(0, 0,  127 , 16, SSD1306_WHITE);
    d.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    d.setTextSize(1);
    d.setCursor(2, 4);
    d.println(label);
    // center the scale about the vertical axis--and use this to offset the needle, and scale text
    Offset = (270 +  sa / 2) * degtorad;
    // find hte scale step value based on the hival low val and the scale sweep angle
    // deducting a small value to eliminate round off errors
    // this val may need to be adjusted
    stepval = ( inc) * (double (sa) / (double (hival - loval))) + .00;
    // draw the scale and numbers
    // note draw this each time to repaint where the needle was
    for (i = 0; i <= sa; i += stepval) {
      angle = ( i  * degtorad);
      angle = Offset - angle ;
      ox =  (r - 2) * cos(angle) + cx;
      oy =  (r - 2) * sin(angle) + cy;
      ix =  (r - 10) * cos(angle) + cx;
      iy =  (r - 10) * sin(angle) + cy;
      tx =  (r + 10) * cos(angle) + cx + 8;
      ty =  (r + 10) * sin(angle) + cy;
      d.drawLine(ox, oy, ix, iy, SSD1306_WHITE);
      d.setTextSize(1);
      d.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
      d.setCursor(tx - 10, ty );
      data = hival - ( i * (inc / stepval)) ;
      d.println(data, dig);
    }
    for (i = 0; i <= sa; i ++) {
      angle = ( i  * degtorad);
      angle = Offset - angle ;
      ox =  (r - 2) * cos(angle) + cx;
      oy =  (r - 2) * sin(angle) + cy;
      d.drawPixel(ox, oy, SSD1306_WHITE);
    }
  }
  // compute and draw the needle
  angle = (sa * (1 - (((curval - loval) / (hival - loval)))));
  angle = angle * degtorad;
  angle = Offset - angle  ;
  ix =  (r - 10) * cos(angle) + cx;
  iy =  (r - 10) * sin(angle) + cy;
  // draw a triangle for the needle (compute and store 3 vertiticies)
  lx =  2 * cos(angle - 90 * degtorad) + cx;
  ly =  2 * sin(angle - 90 * degtorad) + cy;
  rx =  2 * cos(angle + 90 * degtorad) + cx;
  ry =  2 * sin(angle + 90 * degtorad) + cy;

  // blank out the old needle
  d.fillTriangle (pix, piy, plx, ply, prx, pry, SSD1306_BLACK);

  // then draw the new needle
  d.fillTriangle (ix, iy, lx, ly, rx, ry, SSD1306_WHITE);

  // draw a cute little dial center
  d.fillCircle(cx, cy, 3, SSD1306_WHITE);

  //save all current to old so the previous dial can be hidden
  pix = ix;
  piy = iy;
  plx = lx;
  ply = ly;
  prx = rx;
  pry = ry;
  // up until now print sends data to a video buffer NOT the screen
  // this call sends the data to the screen
  d.display();

}


/*

  function to draw a cartesian coordinate system and plot whatever data you want
  just pass x and y and the graph will be drawn huge arguement list

  &display to pass the display object, mainly used if multiple displays are connected to the MCU
  x = x data point
  y = y datapont
  gx = x graph location (lower left)
  gy = y graph location (lower left)
  w = width of graph
  h = height of graph
  xlo = lower bound of x axis
  xhi = upper bound of x asis
  xinc = division of x axis (distance not count)
  ylo = lower bound of y axis
  yhi = upper bound of y asis
  yinc = division of y axis (distance not count)
  title = title of graph
  &redraw = flag to redraw graph on fist call only

*/


void DrawCGraph(Adafruit_SSD1306 &d, double x, double y, double gx, double gy, double w, double h, double xlo, double xhi, double xinc, double ylo, double yhi, double yinc, double dig, String title, boolean &Redraw) {

  double i;
  double temp;
  int rot, newrot;

  if (Redraw == true) {
    Redraw = false;
    d.fillRect(0, 0,  127 , 16, SSD1306_WHITE);
    d.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
    d.setTextSize(1);
    d.setCursor(2, 4);
    d.println(title);
    ox = (x - xlo) * ( w) / (xhi - xlo) + gx;
    oy = (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
    // draw y scale
    d.setTextSize(1);
    d.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
    for ( i = ylo; i <= yhi; i += yinc) {
      // compute the transform
      // note my transform funcition is the same as the map function, except the map uses long and we need doubles
      temp =  (i - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
      if (i == 0) {
        d.drawFastHLine(gx - 3, temp, w + 3, SSD1306_WHITE);
      }
      else {
        d.drawFastHLine(gx - 3, temp, 3, SSD1306_WHITE);
      }
      d.setCursor(gx - 27, temp - 3);
      d.println(i, dig);
    }
    // draw x scale
    for (i = xlo; i <= xhi; i += xinc) {
      // compute the transform
      d.setTextSize(1);
      d.setTextColor(SSD1306_WHITE, SSD1306_BLACK);
      temp =  (i - xlo) * ( w) / (xhi - xlo) + gx;
      if (i == 0) {
        d.drawFastVLine(temp, gy - h, h + 3, SSD1306_WHITE);
      }
      else {
        d.drawFastVLine(temp, gy, 3, SSD1306_WHITE);
      }
      d.setCursor(temp, gy + 6);
      d.println(i, dig);
    }
  }

  // graph drawn now plot the data
  // the entire plotting code are these few lines...

  x =  (x - xlo) * ( w) / (xhi - xlo) + gx;
  y =  (y - ylo) * (gy - h - gy) / (yhi - ylo) + gy;
  d.drawLine(ox, oy, x, y, SSD1306_WHITE);
  d.drawLine(ox, oy - 1, x, y - 1, SSD1306_WHITE);
  ox = x;
  oy = y;

  // up until now print sends data to a video buffer NOT the screen
  // this call sends the data to the screen
  d.display();

}


/////////////////////////////////////////////////////////

