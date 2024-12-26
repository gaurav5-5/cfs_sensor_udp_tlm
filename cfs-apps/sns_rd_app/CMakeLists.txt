set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(CFE_SNS_RD_APP C)

# Create the app module
add_cfe_app(sns_rd_app fsw/src/sns_rd_app.c fsw/src/sns_rd_app_sensorlib.c)

# Add table
add_cfe_tables(sns_rd_app fsw/tables/sns_rd_app_tbl.c)

target_include_directories(sns_rd_app PUBLIC
  fsw/inc
  fsw/mission_inc
  fsw/platform_inc
)

# If UT is enabled, then add the tests from the subdirectory
# Note that this is an app, and therefore does not provide
# stub functions, as other entities would not typically make 
# direct function calls into this application.
if (ENABLE_UNIT_TESTS)
  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)
