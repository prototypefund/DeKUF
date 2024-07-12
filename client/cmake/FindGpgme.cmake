include(FindPackageHandleStandardArgs)

find_library(Gpgme_LIBRARY NAMES gpgme)
find_path(Gpgme_INCLUDE_DIR NAMES gpgme.h)

find_package_handle_standard_args(Gpgme
    REQUIRED_VARS Gpgme_LIBRARY Gpgme_INCLUDE_DIR)

if (Gpgme_FOUND)
    mark_as_advanced(Gpgme_INCLUDE_DIR)
    mark_as_advanced(Gpgme_LIBRARY)
endif()

# TODO: Would be nicer to define a target like Gpgme::Gpgme.
# See: https://izzys.casa/2020/12/how-to-find-packages-with-cmake-the-basics/
