# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels

# Utility rule file for copy-test-makefile.

# Include any custom commands dependencies for this target.
include CMakeFiles/copy-test-makefile.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/copy-test-makefile.dir/progress.make

CMakeFiles/copy-test-makefile: shared

shared: /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/utils/shared
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating shared"
	/usr/bin/cmake -E copy /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels/../utils/shared /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels/bin/

copy-test-makefile: CMakeFiles/copy-test-makefile
copy-test-makefile: shared
copy-test-makefile: CMakeFiles/copy-test-makefile.dir/build.make
.PHONY : copy-test-makefile

# Rule to build all files generated by this target.
CMakeFiles/copy-test-makefile.dir/build: copy-test-makefile
.PHONY : CMakeFiles/copy-test-makefile.dir/build

CMakeFiles/copy-test-makefile.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/copy-test-makefile.dir/cmake_clean.cmake
.PHONY : CMakeFiles/copy-test-makefile.dir/clean

CMakeFiles/copy-test-makefile.dir/depend:
	cd /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels /home/sarpant/Documents/m1/s2/research_projects/prefetch_reload/AdversarialPrefetch/covert_channels/CMakeFiles/copy-test-makefile.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/copy-test-makefile.dir/depend

