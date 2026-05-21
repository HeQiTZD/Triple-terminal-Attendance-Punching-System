# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\appAttendanceAdmin_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appAttendanceAdmin_autogen.dir\\ParseCache.txt"
  "appAttendanceAdmin_autogen"
  )
endif()
