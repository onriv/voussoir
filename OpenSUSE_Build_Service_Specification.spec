# Note: This specfile was created with the help of the tutorial at https://docs.fedoraproject.org/en-US/Fedora_Draft_Documentation/0.1/html/Packagers_Guide/sect-Packagers_Guide-Creating_a_Basic_Spec_File.html

Name:           bookscan
Version:        0.1
Release:        1%{?dist}
Summary:        A single-camera solution for book scanning.

#Group:          
License:        ISC
URL:            https://github.com/publicus/bookscan
Source0:        %{BuildRoot}/bookscan-Adding_Support_for_OpenSUSE_Build_Service.tar.gz
BuildRoot:      %{_tmppath}/bookscan-Adding_Support_for_OpenSUSE_Build_Service

BuildRequires:  cmake
BuildRequires:  opencv
BuildRequires:  opencv-devel
BuildRequires:  libopencv2_4
BuildRequires:  cpp5
BuildRequires:  gcc5

Requires:       opencv

%description
This program takes images of books (each picture including a two-page spread), detects special glyphs pasted in the corners of the book, and de-keystones and thereby digitally flattens the pages. It then automatically separates the pages into separate, cropped image files.

%prep
%setup -q


%build
#%configure
#make %{?_smp_mflags}
cmake -DCMAKE_CXX_COMPILER=g++-5 .
make

#%check
#make check

#%install


#%clean
rm -rf %{BuildRoot}


%files
#%defattr(-,root,root,-)

#%doc markers_for_book_scanner.pdf markers_for_book_scanner.ai test_input.jpg

%changelog
* Fri Feb 10 2017 Jacob Levernier <j@adunumdatum.org> 0.1-1
- Added Specfile

