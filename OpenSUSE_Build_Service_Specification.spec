# Note: This specfile was created with the help of the tutorial at http://www.tldp.org/HOWTO/RPM-HOWTO/build.html

Name:           bookscan
Version:        0.1
Release:        1%{?dist}
Summary:        A single-camera solution for book scanning.

#Group:          
License:        ISC
URL:            https://github.com/publicus/bookscan
Source0:        bookscan-Adding_Support_for_OpenSUSE_Build_Service.tar.gz
BuildRoot:      /var/tmp/%{name}-buildroot

BuildRequires:  cmake
BuildRequires:  opencv
BuildRequires:  opencv-devel
BuildRequires:  libopencv2_4
BuildRequires:  cpp5
BuildRequires:  gcc5
BuildRequires:  gcc5-c++

Requires:       opencv

%description
This program takes images of books (each picture including a two-page spread), detects special glyphs pasted in the corners of the book, and de-keystones and thereby digitally flattens the pages. It then automatically separates the pages into separate, cropped image files.

%prep
%setup -q -n bookscan-Adding_Support_for_OpenSUSE_Build_Service


%build
cmake -DCMAKE_CXX_COMPILER=g++-5 .
make

#%check

%install
#rm -rf $RPM_BUILD_ROOT
mkdir --parents $RPM_BUILD_ROOT/usr/bin

install --strip --mode=755 $RPM_BUILD_DIR/bookscan-Adding_Support_for_OpenSUSE_Build_Service/bin/bookscan $RPM_BUILD_ROOT/usr/bin/bookscan

#%clean
#rm -rf %{BuildRoot}


%files -f bin/bookscan
#%defattr(-,root,root)
#%doc bin/docs/markers_for_book_scanner.pdf bin/docs/markers_for_book_scanner.ai bin/docs/Example_Images/test_input.jpg

#/usr/bin/bookscan

%changelog
* Fri Feb 10 2017 Jacob Levernier <j@adunumdatum.org> 0.1-1
- Added Specfile

