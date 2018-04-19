// This is the CPP file you will edit and turn in.
// Also remove these comments here and add your own.
// TODO: rewrite this comment

#include <iostream>
#include "console.h"
#include "gbufferedimage.h"
#include "grid.h"
#include "gwindow.h"
#include "fauxtoshop-provided.h"   // instructor-provided code
#include "simpio.h"
#include "random.h"


void scatterImage(GBufferedImage& img);
void edgeDetection(GBufferedImage& img);
void greenScreen(GBufferedImage& img);
void compareImages(GBufferedImage& img, GWindow& gw);
int getThreshold();
void getLocation(int& row, int& col);
bool saveImage(GBufferedImage& img);
void filterFunc(GBufferedImage& img, GWindow gw);

using namespace std;

// this function is used as scatter filter
void scatterImage(GBufferedImage& img){
    fakeRandomNumberGenerator();
    // get radiu from user input
    int rad = getInteger("Enter degree of scatter [1-100]:", "Enter degree of scatter [1-100]:");
    while (rad < 1 || rad > 100){
        rad = getInteger("Enter degree of scatter [1-100]:", "Enter degree of scatter [1-100]:");
    }

    Grid<int> original = img.toGrid();
    int row = original.height();
    int col = original.width();
    Grid<int> newgrid(row, col);
    //randomly choose a nearby pixel in original grid to provide color for the new grid.
    for (int r = 0; r < row; r++){
        for (int c = 0; c < col; c++){
            int row_low, col_low, row_high, col_high;
            row_low = r - rad;
            col_low = c - rad;
            row_high = r + rad;
            col_high = c + rad;
            int ran_row = randomInteger(row_low, row_high);
            int ran_col = randomInteger(col_low, col_high);
            while (!original.inBounds(ran_row,ran_col)){
                ran_row = randomInteger(row_low, row_high);
                ran_col = randomInteger(col_low, col_high);
        }
            newgrid[r][c] = original[ran_row][ran_col];
        }
    }
    img.fromGrid(newgrid);
    return;
}

// this function is used to detect edge of an image
void edgeDetection(GBufferedImage& img){
    const int threshold = getThreshold();
    Grid<int> original = img.toGrid();
    Grid<int> new_grids = img.toGrid();
    int imgcol = original.width();
    int imgrow = original.height();
    // loop over each pixel
    for (int r = 0; r < imgrow; r++){
        for (int c = 0; c < imgcol; c++){
            int red1, green1, blue1;
            int pixel = original[r][c];
            GBufferedImage::getRedGreenBlue(pixel, red1, green1, blue1);
            int maxdiff = 0;
            // loop over neighbors of each selected pixel
            for (int i = r-1; i <= r+1; i++){
                for (int j = c-1; j <= c+1; j++){
                    if (original.inBounds(i,j)){
                        int red2, green2, blue2;
                        int pixel2 = original[i][j];
                        GBufferedImage::getRedGreenBlue(pixel2, red2, green2, blue2);
                        // calculate RGB color difference
                        int rgbmax;
                        rgbmax = max(abs(red1-red2), abs(green1-green2));
                        rgbmax = max(rgbmax, abs(blue1-blue2));
                        //get the biggest diffenrence from neighbours
                        if (rgbmax > maxdiff){
                            maxdiff = rgbmax;
                        }
                    }
                }
            }
            // check if the selected pixel is an edge and mark in new grids
            if (maxdiff > threshold){
                new_grids[r][c] = BLACK;
            }else{
                new_grids[r][c] = WHITE;
            }
        }
    }
    img.fromGrid(new_grids);
    return;
}

// this fuction is to make green screen effect
void greenScreen(GBufferedImage& img){
    cout << "Now choose another file to add to your background image." << endl;
    // the file name of sticker image and open the image
    string stickername = getLine("Enter name of image file to open:");
    while (stickername == ""){
        stickername = getLine("Enter name of image file to open:");
    }
    GBufferedImage stickerimg;
    if (openImageFromFilename(stickerimg, stickername)){
        cout << "Opening image file, may take a minute..." << endl;
    }
    Grid<int> stickergrid = stickerimg.toGrid();
    Grid<int> originalgrid = img.toGrid();

    //ask user to define tolence and the location of the sticker image
    int row, col;
    int tolerance = getInteger("Now choose a tolerance threshold:", "Now choose a tolerance threshold:");
    while (tolerance < 0 || tolerance > 100){
        tolerance = getInteger("Now choose a tolerance threshold:", "Now choose a tolerance threshold:");
    }
    getLocation(row, col);

    // loop over pixels of sticker image to get difference from GREEN
    for (int r = 0; r < stickergrid.height(); r++){
        for (int c = 0; c < stickergrid.width(); c++){
            int red, green, blue;
            int pixel = stickergrid[r][c];
            GBufferedImage::getRedGreenBlue(pixel, red, green, blue);
            int rgbmax = max(abs(red), abs(blue));
            rgbmax = max(rgbmax, abs(green - 255));
            if (rgbmax >= tolerance  && originalgrid.inBounds(row+r, col+c)){
                originalgrid[row+r][col+c] = stickergrid[r][c];
            }
        }
    }
    img.fromGrid(originalgrid);
    return;
}

// this function is used to count numbers of different pixels between two images
void compareImages(GBufferedImage& img, GWindow& gw){
    GBufferedImage compimg;
    // open another image
    bool valid = true;
    while (valid){
        cout << "Now choose another file to compare to." << endl;
        string compfile = getLine("Enter name of image file to open:");
        if (compfile != ""){
            cout << "Opening image file, may take a minute..." << endl;
            openImageFromFilename(compimg, compfile);
            valid = false;
        }
    }
    int diff = img.countDiffPixels(compimg);
    // print results
    if (diff == 0){
        cout << "These images are the same!" << endl;
    }else{
        cout << "These images differ in " << diff << " pixel locations!" << endl;
        string otherImg = compimg.getFilename();
        showDiffWindow(gw, otherImg);
    }
    return;
}

// this fuction is used to get input threshold
int getThreshold(){
    while (true){
        int threshold = getInteger("Enter threshold for edge detection:", "Enter threshold for edge detection:");
        if (threshold > 0){
            return threshold;
        }
    }
}

// this function is used to get input location
void getLocation(int& row, int& col){
    while (true) {
        string loc = getLine("Enter location to place image as \"(row, colom)\" (or blank to use mouse):");
        if (loc=="") {
            // get user click location
            getMouseClickLocation(row, col);
            return;
        }
        size_t found = loc.find(",");
        if (found!=string::npos) {
            string x = loc.substr(0, found);
            string y = loc.substr(found+1, loc.size());
            size_t left_par = x.find("(");
            size_t right_par = y.find(")");
            if (left_par == 0 && right_par == y.size()-1) {
                // parse x, y coordinates
                string row_candidate = x.substr(1, x.size());
                string col_candidate = y.substr(0, y.size()-1);
                if (all_of(row_candidate.begin(), row_candidate.end(), ::isdigit) &&
                        all_of(col_candidate.begin(), col_candidate.end(), ::isdigit) &&
                        stoi(row_candidate) >= 0 && stoi(col_candidate) >= 0) {
                    row = stoi(row_candidate);
                    col = stoi(col_candidate);
                    return;
                }
            }
        }
    }
}

// this fuction is used to save image with input file name
bool saveImage(GBufferedImage& img){
    string savedfilename = getLine("Enter filename to save image (or blank to skip saving):");
    if (savedfilename == ""){
        return false;
    }else{
        saveImageToFilename(img, savedfilename);
        return true;
    }
}

// this function is used to choose filter fuction
void filterFunc(GBufferedImage& img, GWindow gw){
    while (true){
        cout << "Which image filter would you like to apply?" << endl;
        cout << "       1 - Scatter" << endl;
        cout << "       2 - Edge detection" << endl;
        cout << "       3 - \"Green screen\" with another image" << endl;
        cout << "       4 - Compare image with another image" << endl;
        int filternum = getInteger("Your choice:", "Illegal integer format. Try again.");
        if (filternum == 1){
              scatterImage(img);
              saveImage(img);
              return;
        }else if (filternum == 2){
              edgeDetection(img);
              saveImage(img);
              return;
        }else if(filternum == 3){
              greenScreen(img);
              saveImage(img);
              return;
        }else if(filternum == 4){
              compareImages(img, gw);
              return;
        }
    }
}

int main() {
    cout << "Welcome to Fauxtoshop!" << endl;

    // basic setup of Graphics Window
    GWindow gw;
    gw.setTitle("Fauxtoshop");
    gw.setExitOnClose(true);
    gw.setVisible(true);
    GBufferedImage img;

    bool valid = true;
    while (valid){
        string filename = getLine("Enter name of image file to open (or blank to quit):");
        if (filename == ""){
            valid = false;
        } else if(openImageFromFilename(img, filename)){
            cout << "Opening image file, may take a minute..." << endl;
            gw.setCanvasSize(img.getWidth(), img.getHeight());
            gw.add(&img, 0, 0);
            filterFunc(img, gw);
            gw.clear();
            cout << endl;
        }
    }
    cout << "Exiting." << endl;
    return 0;
}


