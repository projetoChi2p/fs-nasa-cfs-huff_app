###########################################################
#
# HUFF_APP platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the HUFF_APP configuration
set(HUFF_APP_PLATFORM_CONFIG_FILE_LIST
  huff_app_internal_cfg.h
  huff_app_platform_cfg.h
  huff_app_perfids.h
  huff_app_msgids.h
  #huff_app_msgstruct.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(HUFF_APP_CFGFILE ${HUFF_APP_PLATFORM_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${HUFF_APP_CFGFILE}" NAME_WE)
  if (DEFINED HUFF_APP_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${HUFF_APP_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${HUFF_APP_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${HUFF_APP_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
