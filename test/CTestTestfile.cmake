# CMake generated Testfile for 
# Source directory: /home/luisvm/Documents/lqg/test
# Build directory: /home/luisvm/Documents/lqg/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(cloud "./run" "/home/luisvm/Documents/lqg/bin/cloud")
set_tests_properties(cloud PROPERTIES  WORKING_DIRECTORY "/home/luisvm/Documents/lqg/" _BACKTRACE_TRIPLES "/home/luisvm/Documents/lqg/test/CMakeLists.txt;14;add_test;/home/luisvm/Documents/lqg/test/CMakeLists.txt;19;add_test_case_with_run;/home/luisvm/Documents/lqg/test/CMakeLists.txt;0;")
add_test(system "./run" "/home/luisvm/Documents/lqg/bin/system")
set_tests_properties(system PROPERTIES  WORKING_DIRECTORY "/home/luisvm/Documents/lqg/" _BACKTRACE_TRIPLES "/home/luisvm/Documents/lqg/test/CMakeLists.txt;14;add_test;/home/luisvm/Documents/lqg/test/CMakeLists.txt;20;add_test_case_with_run;/home/luisvm/Documents/lqg/test/CMakeLists.txt;0;")
