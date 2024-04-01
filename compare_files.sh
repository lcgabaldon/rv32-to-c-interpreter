#!/bin/bash

# Path to the directories
instructor_dir="Instructor"
student_dir="Student"

# Check if directories exist
if [ ! -d "$instructor_dir" ] || [ ! -d "$student_dir" ]; then
  echo "Error: One or both directories do not exist."
  exit 1
fi

# Get the list of filenames in each directory
instructor_files=$(find "$instructor_dir" -type f -exec basename {} \;)
student_files=$(find "$student_dir" -type f -exec basename {} \;)

# Iterate over the instructor files
for instructor_file in $instructor_files; do
  # Check if the file exists in the student directory
  if [ -e "$student_dir/$instructor_file" ]; then
    echo "Comparing $instructor_file..."
    # Compare line by line
    diff_result=$(diff "$instructor_dir/$instructor_file" "$student_dir/$instructor_file")
    if [ $? -eq 0 ]; then
      echo "Files are identical."
    else
      echo "Files differ:"
      echo "$diff_result"
    fi
  else
    echo "File $instructor_file not found in $student_dir."
  fi
done

