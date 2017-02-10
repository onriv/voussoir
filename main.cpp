/*
 * Copyright (c) 2012, 2016, Yutaka Tsutano
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/////////////////////////////////////////////
// Specify dependencies
/////////////////////////////////////////////

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include <iostream>
#include <vector>
#include <string>

#include <docopt-0.6.2/docopt.h> // For parsing command line arguments.

#include <typeinfo>

#include "marker.h"
#include "page.h"

/////////////////////////////////////////////

/////////////////////////////////////////////
// Set up docopt (for automatic command-line argument parsing from help documentation
/////////////////////////////////////////////
 
static const char USAGE[] =
R"(Bookscan.
    Description:
      This program takes images of books (each picture including a two-page spread), detects special glyphs pasted in the corners of the book, and de-keystones and thereby digitally flattens the pages. It then automatically separates the pages into separate, cropped image files.
    
    Usage:
      bookscan
      
      bookscan (-h | --help)
      
      bookscan (-v | --version)
      
      bookscan [--verbose] [--no-left-page] [--no-right-page] [-w <page_width_argument>] [-t <page_height_argument>] [-i <input_image>] [<output_image_one>] [<output_image_two>]
      
      bookscan [--verbose] [--no-left-page] [--no-right-page] [-w <page_width_argument>] [-t <page_height_argument>] [--offset-left-page-left-side <offset_left_page_left_side>] [--offset-left-page-right-side <offset_left_page_right_side>] [--offset-left-page-top-side <offset_left_page_top_side>] [--offset-left-page-bottom-side <offset_left_page_bottom_side>] [--offset-right-page-left-side <offset_right_page_left_side>] [--offset-right-page-right-side <offset_right_page_right_side>] [--offset-right-page-top-side <offset_right_page_top_side>] [--offset-right-page-bottom-side <offset_right_page_bottom_side>] [-i <input_image>] [<output_image_one>] [<output_image_two>]

    Options:
      -h --help     Show this screen.
      -v --version  Show version.
      
      --verbose     Show additional output, including the values of every option the program accepts.
      
      -t --page-height=<page_height_argument>  Height of each page (in any metric) ('t' is for 'tall'). [default: 9.5]
      -w --page-width=<page_width_argument>  Width of each page (in any metric). [default: 6.0]
      --no-left-page  Only process right-side pages (Markers 0-3).
      --no-right-page  Only process left-side pages (Markers 4-7).
      
      -i --input-image=<input_image>  The input image.
      
      <output_image_one>  The output image. Needs to have an image-like file extension (e.g., ".jpg", ".JPG", ".png", ".tif", ".tiff").
      <output_image_two>  If relevant, the second output image (see <output_image_one> above).
      
      --offset-left-page-left-side=<offset_left_page_left_side>  Page offset, in the same units as page height and width. [default: 0.00]
      --offset-left-page-right-side=<offset_left_page_right_side>  Page offset, in the same units as page height and width. [default: 0.00]
      --offset-left-page-top-side=<offset_left_page_top_side>  Page offset, in the same units as page height and width. [default: 0.00]
      --offset-left-page-bottom-side=<offset_left_page_bottom_side>  Page offset, in the same units as page height and width. [default: 0.00]
      
      --offset-right-page-left-side=<offset_right_page_left_side>  Page offset, in the same units as page height and width. [default: 0.00]
      --offset-right-page-right-side=<offset_right_page_right_side>  Page offset, in the same units as page height and width. [default: 0.00]
      --offset-right-page-top-side=<offset_right_page_top_side>  Page offset, in the same units as page height and width. [default: 0.00]
      --offset-right-page-bottom-side=<offset_right_page_bottom_side>  Page offset, in the same units as page height and width. [default: 0.00]
      
    Debugging mode:
      Running the program without any arguments will open a webcam window for real-time glyph detection (for calibration). If a webcam is found, this will cause a window to open, showing output from the webcam. When the four "left page" glyphs (i.e., glyphs 0, 1, 2, and 3) are detected by the webcam, a new window will open showing the de-keystoned image that the four glyphs surround. Similarly, when the four "right page" glyphs (i.e., glyphs 4, 5, 6, and 7) are detected by the webcam, an additional new window will open, showing the de-keystoned image for those four glyphs. Throughout this process, debugging text will be given in the terminal window, including which glyphs are detected.
      
    Placing markers:
      Within the docs directory, you'll find PDF and Adobe Illustrator / Inkscape versions of a series of 15 "glyphs," small images that each comprises a unique pattern of pixels in a 6x6 grid. You'll need to print and cut out the glyphs; at the moment, only glyphs 0-3 (left page) and 4-7 (right page) are needed. Tape or otherwise affix the glyphs in clockwise order around the perimeter of each book page (for example, if you're using a glass or acrylic platen to flatten the pages of a book, affix the glyphs in each corner of the platen: starting at the top left and moving clockwise to the center/spine of the book, place glyphs 0, 1, 2, and 3 around the left page, and (again from top left and moving clockwise) glyphs 4, 5, 6, and 7 on the right page. The program will, by default, crop to the inside vertical, outside horizontal edge of the glyphs it detects. This can be adjusted using the offset arguments defined above. The offset arguments can be positive or negative (e.g., setting --offset-left-page-left-side to -0.5 will move the crop line to the left 0.5 units).
)";

/////////////////////////////////////////////

/////////////////////////////////////////////
// Define the program
/////////////////////////////////////////////

void process_image(IplImage *src_img,
        std::map<int, CvPoint2D32f> &left_dst_markers,
        LayoutInfo left_layout,
        std::map<int, CvPoint2D32f> &right_dst_markers,
        LayoutInfo right_layout)
{
    BookImage book_image(src_img);

    {
        IplImage *dst_img
                = book_image.create_page_image(left_dst_markers, left_layout);
        if (dst_img != NULL) {
            cvShowImage("Left", dst_img);
            cvReleaseImage(&dst_img);
        }
    }

    {
        IplImage *dst_img
                = book_image.create_page_image(right_dst_markers, right_layout);
        if (dst_img != NULL) {
            cvShowImage("Right", dst_img);
            cvReleaseImage(&dst_img);
        }
    }
}

float page_width;
float page_height;

bool verbose;

bool is_input_image_given;
const char* input_image;

bool is_first_output_image_given;
const char* first_output_image;

bool is_second_output_image_given;
const char* second_output_image;

bool process_left_page;
bool process_right_page;

float offset_left_page_left_side;
float offset_left_page_right_side;
float offset_left_page_top_side;
float offset_left_page_bottom_side;

float offset_right_page_left_side;
float offset_right_page_right_side;
float offset_right_page_top_side;
float offset_right_page_bottom_side;

int main(int argc, const char** argv)
{
    //////////////////////////
    // Parse command-line options with docopt
    //////////////////////////
    std::map<std::string, docopt::value> args
        = docopt::docopt(
             USAGE,
             { argv + 1, argv + argc },
             true, // show help if requested
             "Bookscan 0.1" // version string
          );
    // To extract arguments, you can use the form 'args["--page-height"].asLong()'. See https://github.com/docopt/docopt.cpp/issues/8 for an offhand example. The values are normally strings. asBool() and asLong() can be used to transform them into boolean and numbers, respectively.
    
    /////////////
    // Extract argument values
    /////////////
    
    // Examples for argument parsing:
        // std::string example = args["--example_argument"].asString(); // String
        // float example = stof(args["--example_argument"].asString()); // Float
        // bool example = args["--example_argument"].asBool(); // Boolean

    verbose = args["--verbose"].asBool();
    
    if(verbose == true){ // If we've been asked to be verbose, print info. about each option that the program accepts (in the form "Name: 'Value'"):
        std::cout << "Verbose mode is turned on." << std::endl;
        
        std::cout << "Program options and their current settings:" << std::endl;
        
        for(auto const& arg : args) {
            std::cout << "    " << arg.first << ": " << arg.second << std::endl; // We'll indent the lines with several leading spaces, just for aesthetics.
        }
    }
    
    
    page_height = stof(args["--page-height"].asString());
    page_width = stof(args["--page-width"].asString());
    
    offset_left_page_left_side = stof(args["--offset-left-page-left-side"].asString());
    offset_left_page_right_side = stof(args["--offset-left-page-right-side"].asString());
    offset_left_page_top_side = stof(args["--offset-left-page-top-side"].asString());
    offset_left_page_bottom_side = stof(args["--offset-left-page-bottom-side"].asString());
    offset_right_page_left_side = stof(args["--offset-right-page-left-side"].asString());
    offset_right_page_right_side = stof(args["--offset-right-page-right-side"].asString());
    offset_right_page_top_side = stof(args["--offset-right-page-top-side"].asString());
    offset_right_page_bottom_side = stof(args["--offset-right-page-bottom-side"].asString());
    
    
    if(args["--input-image"]){ // If a value has been set (i.e., is not null) is its default (just a space), treat it as not having been set.
        std::cout << "Input image was given. Processing image..." << std::endl;
        is_input_image_given = true;
        input_image = args["--input-image"].asString().c_str();
    } else {
        std::cout << "Input image was *not* given. Thus, attempting to open webcam..." << std::endl;
        is_input_image_given = false;
    }
    
    if(args["<output_image_one>"]){ // If a value has been set (i.e., is not null) is its default (just a space), treat it as not having been set.
        //std::cout << "YES" << std::endl;
        is_first_output_image_given = true;
        first_output_image = args["<output_image_one>"].asString().c_str();
    } else {
        //std::cout << "NO" << std::endl;
        is_first_output_image_given = false;
    }
    
    if(args["<output_image_two>"]){ // If a value has been set (i.e., is not null) is its default (just a space), treat it as not having been set.
        //std::cout << "YES" << std::endl;
        is_second_output_image_given = true;
        second_output_image = args["<output_image_two>"].asString().c_str();
    } else {
        //std::cout << "NO" << std::endl;
        is_second_output_image_given = false;
    }
    
    process_left_page = ! args["--no-left-page"].asBool(); // Make this a positive question ("Do we process the left page?") by flipping it with '~' from the assertion "Do not process the left page."
    process_right_page = ! args["--no-right-page"].asBool(); // Make this a positive question ("Do we process the left page?") by flipping it with '~' from the assertion "Do not process the left page."
    
    /////////////
    // End Extract argument values
    /////////////
    
    //////////////////////////
    
    // Configure left page. Glyph '0' is expected to be in the top left corner of the page. Glyph '1' is expected to be in the top right corner, etc. going clockwise.
    // These points are in the form cvPoint2D32f(horizontal_location, vertical_location) in whatever units page_width and page_height are (what matters is their relation to each other, and not as much the units themselves, although larger units will mean larger output images).
    std::map<int, CvPoint2D32f> left_dst_markers;
    left_dst_markers[0] = cvPoint2D32f(0.00, 0.00);
    left_dst_markers[1] = cvPoint2D32f(page_width, 0.00);
    left_dst_markers[2] = cvPoint2D32f(page_width, page_height);
    left_dst_markers[3] = cvPoint2D32f(0.00, page_height);
    
    // Configure right page. Similar to the left page, Glyph '4' is expected to be in the top left corner of the page, with each higher-numbered glyph in the next corner, going around the page clockwise.
    // See above in the "Configure left page" section re: the format of these points.
    std::map<int, CvPoint2D32f> right_dst_markers;
    right_dst_markers[4] = cvPoint2D32f(0.00, 0.00);
    right_dst_markers[5] = cvPoint2D32f(page_width, 0.00);
    right_dst_markers[6] = cvPoint2D32f(page_width, page_height);
    right_dst_markers[7] = cvPoint2D32f(0.00, page_height);
    
    // Define additional information about the pages:
    LayoutInfo left_layout;
    LayoutInfo right_layout;
    left_layout.dpi = 600.0;
    right_layout.dpi = 600.0;
    
    // The image will automatically be cropped to the outside width of the glyphs, and the inside height of the glyphs (i.e., the longest width between glyphs, and the shortest height between glyphs.
    // Here, then, you can define offsets (+ or -) if, for example, you want to bring in the image more (if the glyphs are printed on a piece of paper, e.g.).
    // To define an offset, just add or subtract from the numbers below (e.g., "page_width - 0.30").
    // These offsets should be on the same scale as the page width and height you pass to the program through the command line (e.g., inches) -- it doesn't matter which scale you use, as long as it's consistent (the important thing is the relative sizes of the numbers given to the program).
    
    left_layout.page_left = 0 + offset_left_page_left_side;
    left_layout.page_top = 0 + offset_left_page_top_side;
    left_layout.page_right = page_width + offset_left_page_right_side;
    left_layout.page_bottom = page_height + offset_left_page_bottom_side;
    
    right_layout.page_left = 0 + offset_right_page_left_side;
    right_layout.page_top = 0 + offset_right_page_top_side;
    right_layout.page_right = page_width + offset_right_page_right_side;
    right_layout.page_bottom = page_height + offset_right_page_bottom_side;
    
    // Process if an input image is supplied; otherwise, open a webcam for
    // debugging.
    if (is_input_image_given == true) {
        
        IplImage *src_img = cvLoadImage(input_image);
        
        if (src_img == NULL) {
            std::cerr << "Error: Failed to load the source image specified." << std::endl;
            return 1;
        }
        
        BookImage book_img(src_img);
        
        if (process_left_page == true) {
	        IplImage *left_img
	                = book_img.create_page_image(left_dst_markers, left_layout);
	        
	        if (left_img != NULL) {
	            cvSaveImage(first_output_image, left_img);
	            cvReleaseImage(&left_img);
	        }
        }
        
        if (process_right_page == true) {
	        IplImage *right_img
	                = book_img.create_page_image(right_dst_markers, right_layout);
	        if (right_img != NULL) {
	            cvSaveImage(second_output_image, right_img);
	            cvReleaseImage(&right_img);
	        }
	    }
	    
        cvReleaseImage(&src_img);
    } else { // Open debugging windows
        // Create windows.
        cvNamedWindow("Source", 0);
        cvResizeWindow("Source", 480, 640);

        left_layout.dpi = 100;
        right_layout.dpi = 100;

        // Open webcam.
        CvCapture* capture = cvCreateCameraCapture(0);
        if (!capture) {
            std::cerr << "Failed to load the camera device." << std::endl;
            return 1;
        }
        const double scale = 1.0;
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 1600 * scale);
        cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 1200 * scale);
        
        // Process the pages in real-time, displaying the output.
        while (cvWaitKey(10) < 0) {
            IplImage *src_img = cvQueryFrame(capture);
            cvShowImage("Source", src_img);
            process_image(src_img,
                    left_dst_markers, left_layout,
                    right_dst_markers, right_layout);
        }
    }

    return 0;
}

/////////////////////////////////////////////
