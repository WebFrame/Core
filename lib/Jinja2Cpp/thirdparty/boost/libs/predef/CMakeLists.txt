cmake_minimum_required(VERSION 3.5)
project(boost_predef)

find_package(BCM)
include(BCMDeploy)
include(BCMSetupVersion)


bcm_setup_version(VERSION 1.64.0)

add_library(boost_predef INTERFACE
)
set_property(TARGET boost_predef PROPERTY EXPORT_NAME predef)





bcm_deploy(TARGETS boost_predef INCLUDE include NAMESPACE boost::)

if(BOOST_CMAKE_BUILD_TESTS)
    add_subdirectory(test)
endif()
