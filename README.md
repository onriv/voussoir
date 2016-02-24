bookscan
========

A single-camera solution for book scanning.

This program takes images of books (each picture including a two-page spread), detects special glyphs pasted in the corners of the book, and **[de-keystones](https://en.wikipedia.org/wiki/Keystone_effect "Wikipedia: 'Keystone Effect'") and thereby digitally flattens the pages.** It then **automatically separates the pages into separate, cropped image files.**

# Example

The program takes images like this:

<a href="https://raw.githubusercontent.com/publicus/bookscan/master/build/test_input.jpg" >
	<img src="https://raw.githubusercontent.com/publicus/bookscan/master/build/test_input.jpg" alt="[Keystoned image example]" height="256px">
</a>

And turns them into images like this:

<a href="https://raw.githubusercontent.com/publicus/bookscan/master/build/test_input.jpg" style="
	display: inline-block;
	float: left;
	padding-right: 1em;
	">
	<img src="https://raw.githubusercontent.com/publicus/bookscan/master/build/test_output_left_page.jpg" alt="[De-keystoned left page]" height="256px">
</a>

<a href="https://raw.githubusercontent.com/publicus/bookscan/master/build/test_input.jpg" style="
	display: inline-block;
	float: left;
	">
	<img src="https://raw.githubusercontent.com/publicus/bookscan/master/build/test_output_right_page.jpg" alt="[De-keystoned right page]" height="256px">
</a>

# Background

This program was written as part of a one-day build in 2012 by [Yutaka Tsutano](http://yutaka.tsutano.com/projects/others/ "Yutaka Tsutano's website"). Videos of Mr. Tsutano's prototype book scanner build and software tests are available here:

* [Book Scanner: First Prototype](http://www.youtube.com/watch?v=rjzxlA9RWio)
* [Book Scanner: Marker Test](http://www.youtube.com/watch?v=YXANjnry6CU)
* [Book Scanner: Image Processing Test #1](http://www.youtube.com/watch?v=lHHPFBH2EkA)

Mr. Tsutano kindly posted the code after receiving requests from viewers of the above videos, and kindly released the code under the permissive [ISC license](https://en.wikipedia.org/wiki/ISC_license "ISC License") (which is [functionally equivalent to](http://choosealicense.com/licenses/#isc "ChooseALicense.com: ISC License") the [BSD 2-Clause](http://choosealicense.com/licenses/bsd-2-clause/ "BSD Two-Clause License") and [MIT](http://choosealicense.com/licenses/mit/ "MIT License") licenses) in 2016.

Mr. Tsutano noted in the code's original readme that "this program was written in ad-hoc manner without thinking about publishing the source code. It was just a test of concept. So I published it. But it was a project that was done in a day years ago, and I have no time to verify it still works today." Jacob Levernier, a member of the [DIY Book Scanning](http://DIYBookscanner.org "DIYBookScanner.org") community, forked the codebase in 2016 in order to better document and clean it for a wider audience.

## Compilation

Install OpenCV and SCons:

* **MacPorts:** `sudo port install opencv scons`
* Linux:
	* **openSUSE**: `sudo zypper install opencv scons libopencv2_4 opencv-devel`

*\[If you successfully build the program on another system, please let me know, and I'll add the required packages here.]*

Compile using SCons:

```
cd [directory of the source code]
scons
```

The binary executable will be saved under `./build/extpage`.

# Usage

## Getting Ready to Use the Program / Taking Pictures

Within the `markers` directory, you'll find PDF and Adobe Illustrator / Inkscape versions of a series of 15 "glyphs," small images that each comprises a unique pattern of pixels in a 6x6 grid. You'll need to print and cut out the glyphs; at the moment, only glyphs 0-3 (left page) and 4-7 (right page) are needed.

Tape or otherwise affix the glyphs in **clockwise order** around the perimeter of each book page (for example, if you're using a glass or acrylic [platen](http://diybookscanner.org/ "DIYBookScanner.org -- Scroll down for a description of each part of a book scanner") to flatten the pages of a book, affix the glyphs in each corner of the platen: **starting at the top left and moving clockwise to the center/spine of the book,** glyphs 0, 1, 2, and 3 on the left page, and (again from top left and moving clockwise) glyphs 4, 5, 6, and 7 on the right page.

When you take photos, it should be under **bright, even lighting,** in order to make both the book pages and the glyphs visible and crisp. One of the advantages of using this program is that you can take photos from off center -- as long as the glyphs can all be clearly seen and are not blurry in the image, the each page will be de-keystoned as if it had been photographed perpendicularly.

Do note that the program only transforms the pages as two-dimensional objects; that is, it **assumes that the pages are each (individually) flat** (e.g., under a glass or acrylic platen). If the pages are **warped/curled** in addition to being keystoned, this program will not correct the curl (correcting curl is difficult to do; ScanTailor, linked below, makes a good effort by looking for curves in lines of text on each page, as do commercial options like [Booksorber](http://booksorber.com/ "Booksorber"), which tries to find the outline of the book's pages; in general, though, dewarping a page consistently likely requires a more complicated setup using [lasers](https://github.com/duerig/laser-dewarp "Laser Dewarper DIY book scanning software") or [infrared light]("https://www.youtube.com/watch?v=03ccxwNssmo" "YouTube: 'BFS-Auto: High Speed Book Scanner at over 250 pages/min'").

## Using the Program

The program takes three arguments:

`./extpage test_input.jpg output_left.jpg output_right.jpg`

where test_input.jpg is the input file name, and the following two are the
output file names.

### Debugging using a Webcam

To debug using a webcam, execute the program without an argument:

	./extpage

If a webcam is found, this will cause a window to open, showing output from the webcam. When the four "left page" glyphs (i.e., glyphs 0, 1, 2, and 3) are detected by the webcam, a new window will open showing the de-keystoned image that the four glyphs surround. Similarly, when the four "right page" glyphs (i.e., glyphs 4, 5, 6, and 7) are detected by the webcam, an additional new window will open, showing the de-keystoned image for those four glyphs. Throughout this process, debugging text will be given in the terminal window, including which glyphs are detected.

### Using Automator.app

Per Mr. Tsutano's original proof-of-concept, one way to automate the use of the program in OSX is to use an Automator script (the script below is what was used in the original demonstration videos above):

```
if [ x$1 = x ]; then
	exit;
fi

FILENAME=${1##*/}
EXT=${FILENAME##*.}
BASE=${FILENAME%.*}
SRC_PATH=${1%/*}
RESULT_PATH=${SRC_PATH%/*}/out/

LEFT_IMG=${RESULT_PATH}${BASE}_left.${EXT}
RIGHT_IMG=${RESULT_PATH}${BASE}_right.${EXT}

say "Extracting pages..." 

if extpage $1 $LEFT_IMG $RIGHT_IMG > /dev/null; then
	say "Done." 
else
	say "Failed."
fi

echo $1
echo $LEFT_IMG
echo $RIGHT_IMG
```

Save it as an application and invoke it using Folder Actions. For a screenshot of this setup, see [automator.png](docs/automator.png).

### More on Book Scanning

* If you're interested in building some **hardware** to flatten the pages of the book, I recommend exploring the [DIYBookScanner.org gallery and forums](http://diybookscanner.org/intro.html "DIYBookScanner.org: Gallery"). An especially popular and easy-to-build model was created by a gentleman named David Landin in 2013 out of **PVC plumbing pipe,** and can be viewed [here](http://diybookscanner.org/forum/viewtopic.php?f=14&t=2914 "DIYBookScanner.org: David Landin model").
* Additionally, the founder of the DIYBookScanning community, [Daniel Reetz](http://www.danreetz.com/ "Daniel Reetz"), wrote an extensive [monograph](http://diybookscanner.org/archivist "DIYBookScanner.org: The Archivist") on DIY scanning hardware in 2015, when he retired from the project (see also [Mr. Reetz' blog post](http://www.danreetz.com/blog/2015/12/31/internet-dan-seems-to-be-dead/ "Daniel Reetz: 'Internet Dan Seems to be Dead'") about the site. Mr. Reetz' expertise on the topic is unmatched; his monograph is, in my opinion, the single best place to begin learning about DIY scanners (after the [DIYBookScanner.org gallery](http://diybookscanner.org/intro.html "DIYBookScanner.org: Gallery") linked above).
* If you're interested in post-processing for your book photos (e.g., white-balance correcting, further automatic cropping, etc., the output from this program can be used as input for **[ScanTailor](http://scantailor.org/ "ScanTailor"),** which is also free and open-source.

# Todo

* Add help documentation to the program itself (allowing a command-line `--help / -h` flag)
* Add command-line flags for the following:
	* Show detected glyph pattern in debugging mode
	* Show page width and height (perhaps with a `--verbose / -v` flag)
* Establish a build process to allow distributing ready-to-use binaries for different platforms (including the major Linux distros 32- and 64-bit, OSX 64-bit, and Windows)
* **Get more people on board!** I (Jacob) am not a C++ developer, so I see my role as primarily custodial right now (i.e., working on documentation and community-building).
* Craft a more accurate, clever, and less generic name for the program.

# Contributors

* [Yutaka Tsutano](http://yutaka.tsutano.com "Yutaka Tsutano's website")
* [Jacob Levernier](http://adunumdatum.org "Jacob Levernier's website")
