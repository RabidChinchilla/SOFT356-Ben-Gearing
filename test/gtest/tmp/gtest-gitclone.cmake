
if(NOT "C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest-stamp/gtest-gitinfo.txt" IS_NEWER_THAN "C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest-stamp/gtest-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: 'C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest-stamp/gtest-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E remove_directory "C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: 'C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "C:/Program Files/Git/cmd/git.exe"  clone  "https://github.com/google/googletest.git" "gtest"
    WORKING_DIRECTORY "C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/google/googletest.git'")
endif()

execute_process(
  COMMAND "C:/Program Files/Git/cmd/git.exe"  checkout master --
  WORKING_DIRECTORY "C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'master'")
endif()

execute_process(
  COMMAND "C:/Program Files/Git/cmd/git.exe"  submodule update --recursive --init 
  WORKING_DIRECTORY "C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: 'C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest-stamp/gtest-gitinfo.txt"
    "C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest-stamp/gtest-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: 'C:/Users/Gearing/Documents/GitHub/SOFT356-Ben-Gearing/test/gtest/src/gtest-stamp/gtest-gitclone-lastrun.txt'")
endif()

