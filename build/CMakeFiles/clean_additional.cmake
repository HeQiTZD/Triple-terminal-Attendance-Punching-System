# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\appAttendanceServer_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\appAttendanceServer_autogen.dir\\ParseCache.txt"
  "appAttendanceServer_autogen"
  )
endif()
