set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(CFE_UDP_TLM_APP C)

list (FIND TGTSYS_${SYSVAR}_APPS sns_rd_app HAVE_APP)
if (HAVE_APP GREATER_EQUAL 0)
  message("FOUND")
  include_directories($<TARGET_PROPERTY:sns_rd_app,INTERFACE_INCLUDE_DIRECTORIES>)
else()
  message("NOT FOUND")
endif()

# Create the app module
add_cfe_app(udp_tlm_app fsw/src/udp_tlm_app.c)

# Add table
add_cfe_tables(udp_tlm_app fsw/tables/udp_tlm_tbl.c)

target_include_directories(udp_tlm_app PUBLIC
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
