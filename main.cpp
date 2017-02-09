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

    Usage:
      bookscan [--verbose] [-w page_width_argument] [-t page_height_argument] -i input_image -o <output_image_one> [<output_image_two>] [-x example]
      bookscan (-h | --help)
      bookscan (-v | --version)

    Options:
      -h --help     Show this screen.
      -v --version  Show version.
      
      --verbose     Show additional output, including the values of every option the program accepts.
      
      -t --page-height=<page_width_argument>  Height of each page (in any metric) ('t' is for 'tall') [default: 10.0].
      -w --page-width=<page_height_argument>  Width of each page (in any metric) [default: 6.0].
      -x Example [default: 1.5]
      --no-left-page Only process right-side pages (Markers 0-3).
      --no-right-page Only process left-side pages (Markers 4-7).
      
      -i --input-image The input image.
      -o --output The output image(s).
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

int main(int argc, const char** argv)
{

    
    //////////////////////////
    
    if (argc > 3) {
        page_width = atof(argv[2]); // Use the second argument. In the example image, this should be 6
        page_height = atof(argv[3]); // Use the third argument. In the example image, this should be 9.5
    }
    
    // Configure left page. Glyph '0' is expected to be in the top left corner of the page. Glyph '1' is expected to be in the top right corner, etc. going clockwise.
    std::map<int, CvPoint2D32f> left_dst_markers;
    left_dst_markers[0] = cvPoint2D32f(0.00, 0.00);
    left_dst_markers[1] = cvPoint2D32f(page_width, 0.00);
    left_dst_markers[2] = cvPoint2D32f(page_width, page_height);
    left_dst_markers[3] = cvPoint2D32f(0.00, page_height);
    
    // Configure right page. Similar to the left page, Glyph '4' is expected to be in the top left corner of the page, with each higher-numbered glyph in the next corner, going around the page clockwise.
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
    
    left_layout.page_left = 0;
    left_layout.page_top = 0;
    left_layout.page_right = page_width + 0.4;
    left_layout.page_bottom = page_height - 0.05;
    
    right_layout.page_left = 0;
    right_layout.page_top = 0;
    right_layout.page_right = page_width + 0.4;
    right_layout.page_bottom = page_height - 0.05;
    

    // Process if an input image is supplied; otherwise, open a webcam for
    // debugging.
    if (argc > 3) {
        IplImage *src_img = cvLoadImage(argv[1]);
        if (src_img == NULL) {
            std::cerr << "Failed to load the source image specified.\n";
            return 1;
        }

        BookImage book_img(src_img);

        IplImage *left_img
                = book_img.create_page_image(left_dst_markers, left_layout);
        
        if (left_img != NULL) {
            cvSaveImage(argv[4], left_img);
            cvReleaseImage(&left_img);
        }

        IplImage *right_img
                = book_img.create_page_image(right_dst_markers, right_layout);
        if (right_img != NULL) {
            cvSaveImage(argv[5], right_img);
            cvReleaseImage(&right_img);
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
            std::cerr << "Failed to load the camera device.\n";
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
