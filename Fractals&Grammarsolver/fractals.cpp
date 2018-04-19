/* This is a collaborative work between Wenyan Hua (wenyan28@stanford.edu)
 * and Shaw Lu (xiaolu@stanford.edu)
 * as part2 of assignment3
 * of CS 106B, Programming Abstraction.
 * The program has two functions.
 * For sierpinski triangle, the user interface is summarized as
 * (1) User sets x/y as the coordinates of the top/left of the triangle,
 * (2) User sets the length of each side of the triangle
 * (3) User the order of the figure to draw
 * (4) User clicks the first button to draw a order-K sierpinski triangle
 * For Flood fill function, the user interface is summarized as
 * (1) User select a color
 * (2) User clicks the mouse and changes the color of contiguous part to selected color
 */

#include "fractals.h"
using namespace std;

const float h = 0.8660;

void drawSierpinskiTriangle(GWindow& window, double x, double y, double size, int order) ;
bool isinBound(const int width, const int height, int x, int y);
void floodFillHelper(GWindow& window, int x, int y, int color, int clickedRgb, int &count, int const width, int const height);
int floodFill(GWindow& window, int x, int y, int color);


/* In each call, draws three inverted equilateral triangles
 * at the top left, top right and bottom of the original
 * equilateral triangle, resulting in four equilateral
 * triangles of equal size. */
void drawSierpinskiTriangle(GWindow& window, double x, double y, double size, int order) {
    if (order == 0) return; // base case
    if (x >= 0 && y >= 0 && size >= 0) {
        window.drawLine(x, y, x + size, y);
        window.drawLine(x, y, x + size/2.0, y + size*h);
        window.drawLine(x + size, y, x + size/2.0, y + size*h);
        drawSierpinskiTriangle(window, x, y, size/2, order-1);
        drawSierpinskiTriangle(window, x+size/2, y, size/2, order-1);
        drawSierpinskiTriangle(window, x+size/4, y+size*h/2, size/2, order-1);
    } else {
        // If the x, y, order, or size passed is negative, throw a string exception
        throw "";
    }
}

// Check if the pixel is in the bound of the canvas
bool isinBound(const int width, const int height, int x, int y) {
    if ( x < width && y < height && x >= 0 && y >= 0 ){
        return true;
    }else return false;
}

// Helper function for recursion
void floodFillHelper(GWindow& window, int x, int y, int color, int clickedRgb, int &count, int const width, int const height) {
    // color of a neighboring pixel
    int new_rgb = window.getPixel(x, y);
    // base case
    if ( new_rgb != clickedRgb ) {
        return;
    }

    if ( new_rgb != color && isinBound (width, height, x, y)) {
        window.setPixel(x, y, color);
        count++;
        floodFillHelper(window, x-1, y, color, clickedRgb, count, width, height);
        floodFillHelper(window, x+1, y, color, clickedRgb, count, width, height);
        floodFillHelper(window, x, y-1, color, clickedRgb, count, width, height);
        floodFillHelper(window, x, y+1, color, clickedRgb, count, width, height);
    }
    return;
}

/* Change the contiguous part of the clicked pixel
 * to the selected color.
 * Return the number of pixels changed.
 */
int floodFill(GWindow& window, int x, int y, int color) {
    // color of pixel clicked
    int clickedRgb = window.getPixel(x, y);
    int width = window.getCanvasWidth();
    int height = window.getCanvasHeight();

    if ( clickedRgb == color ) return 0;
    int count = 0;
    floodFillHelper(window, x, y, color, clickedRgb, count, width, height);
    return count;
}
