###########################################################
#
# HUFF_APP mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the HUFF_APP configuration
set(HUFF_APP_MISSION_CONFIG_FILE_LIST
  huff_app_fcncodes.h
  huff_app_interface_cfg.h
  huff_app_mission_cfg.h
  huff_app_perfids.h
  huff_app_msg.h
  huff_app_msgdefs.h
  huff_app_msgstruct.h
  huff_app_tbl.h
  huff_app_tbldefs.h
  huff_app_tblstruct.h
  huff_app_topicids.h
)

if (CFE_EDS_ENABLED_BUILD)

  # In an EDS-based build, these files come generated from the EDS tool
  set(HUFF_APP_CFGFILE_SRC_HUFF_APP_interface_cfg "huff_app_eds_designparameters.h")
  set(HUFF_APP_CFGFILE_SRC_HUFF_APP_tbldefs       "huff_app_eds_typedefs.h")
  set(HUFF_APP_CFGFILE_SRC_HUFF_APP_tblstruct     "huff_app_eds_typedefs.h")
  set(HUFF_APP_CFGFILE_SRC_HUFF_APP_msgdefs       "huff_app_eds_typedefs.h")
  set(HUFF_APP_CFGFILE_SRC_HUFF_APP_msgstruct     "huff_app_eds_typedefs.h")
  set(HUFF_APP_CFGFILE_SRC_HUFF_APP_fcncodes      "huff_app_eds_cc.h")

endif(CFE_EDS_ENABLED_BUILD)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(HUFF_APP_CFGFILE ${HUFF_APP_MISSION_CONFIG_FILE_LIST})
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
