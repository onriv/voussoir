#!/bin/bash

################################
# Notes
################################

# This script requires that inotifywait be installed. Sometimes inotifywait comes in a larger package called inotify-tools.

# You can stop the script with Ctrl+C

################################
# End Notes
################################

################################
# Settings
################################

# Neither of these path names should have a trailing slash (i.e., a '/' at the end):
directory_to_watch="/home/jacoblevernier/Downloads/test"
output_directory="/home/jacoblevernier/Downloads/test/output"

program_location="/home/jacoblevernier/Downloads/bookscan/bin/voussoir"

program_arguments="--verbose"

################################
# End Settings
################################

# Create the output directory if necessary:
mkdir --parents "$output_directory"
cd "$directory_to_watch"

# Watch for files to be placed into $directory_to_watch. When they are, run the program on them.
inotifywait --monitor "$directory_to_watch" --format '%f' -e close_write | \
    while read file; do
        "$program_location" \
        $program_arguments \
        --input-image "$file" \
        "$output_directory/$file-left_page" \
        "$output_directory/$file-right_page"
    done
