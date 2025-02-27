project(CFE_HUFF_APP C)

set(APP_SRC_FILES
  fsw/src/huff_app.c
  fsw/src/huff_app_cmds.c
  fsw/src/huff_app_utils.c
  #fsw/tables/huff_app_tbl.c
)

if (CFE_EDS_ENABLED_BUILD)
  list(APPEND APP_SRC_FILES
    fsw/src/huff_app_eds_dispatch.c
  )
else()
  list(APPEND APP_SRC_FILES
    fsw/src/huff_app_dispatch.c
  )
endif()

# Create the app module
add_cfe_app(huff_app ${APP_SRC_FILES})

target_include_directories(huff_app PUBLIC fsw/inc)
# Include the public API from sample_lib to demonstrate how
# to call library-provided functions
add_cfe_app_dependency(huff_app bench_lib)

# Add table
#add_cfe_tables(huff_app fsw/tables/huff_app_tbl.c)
#target_link_libraries(huff_app tbl)

# If UT is enabled, then add the tests from the subdirectory
# Note that this is an app, and therefore does not provide
# stub functions, as other entities would not typically make
# direct function calls into this application.
if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)
