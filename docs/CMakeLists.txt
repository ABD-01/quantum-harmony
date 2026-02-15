# ---------------------------------------------------------------------------
# Documentation build: Doxygen → breathe-apidoc → sphinx-build
# ---------------------------------------------------------------------------

find_program(DOXYGEN_EXECUTABLE doxygen REQUIRED)
find_program(BREATHE_APIDOC_EXECUTABLE breathe-apidoc REQUIRED)
find_program(SPHINX_BUILD_EXECUTABLE sphinx-build REQUIRED)
find_package(Python3 COMPONENTS Interpreter REQUIRED)

set(DOCS_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
set(DOCS_XML_DIR    "${DOCS_SOURCE_DIR}/xml")
set(DOCS_API_DIR    "${DOCS_SOURCE_DIR}/api")
set(DOCS_BUILD_DIR  "${DOCS_SOURCE_DIR}/_build")

# Stamp files for dependency tracking.
set(DOXYGEN_STAMP   "${CMAKE_CURRENT_BINARY_DIR}/doxygen.stamp")
set(BREATHE_STAMP   "${CMAKE_CURRENT_BINARY_DIR}/breathe.stamp")

# Step 1 — Run Doxygen (produces xml/).
add_custom_command(
    OUTPUT  ${DOXYGEN_STAMP}
    COMMAND ${DOXYGEN_EXECUTABLE} "${DOCS_SOURCE_DIR}/Doxyfile"
    COMMAND ${CMAKE_COMMAND} -E touch ${DOXYGEN_STAMP}
    WORKING_DIRECTORY "${DOCS_SOURCE_DIR}"
    DEPENDS "${DOCS_SOURCE_DIR}/Doxyfile"
    COMMENT "Running Doxygen..."
)

# Step 2 — Run breathe-apidoc (generates RST stubs from XML).
add_custom_command(
    OUTPUT  ${BREATHE_STAMP}
    COMMAND ${BREATHE_APIDOC_EXECUTABLE} -o "${DOCS_API_DIR}" "${DOCS_XML_DIR}"
    COMMAND ${Python3_EXECUTABLE} "${DOCS_SOURCE_DIR}/post_process.py" "${DOCS_API_DIR}"
    COMMAND ${CMAKE_COMMAND} -E touch ${BREATHE_STAMP}
    WORKING_DIRECTORY "${DOCS_SOURCE_DIR}"
    DEPENDS ${DOXYGEN_STAMP} "${DOCS_SOURCE_DIR}/post_process.py"
    COMMENT "Running breathe-apidoc and post-processing..."
)

# Step 3 — Run sphinx-build (produces final HTML in _build/).
add_custom_command(
    OUTPUT  "${DOCS_BUILD_DIR}/index.html"
    COMMAND ${SPHINX_BUILD_EXECUTABLE} "${DOCS_SOURCE_DIR}" "${DOCS_BUILD_DIR}"
    WORKING_DIRECTORY "${DOCS_SOURCE_DIR}"
    DEPENDS ${BREATHE_STAMP} "${DOCS_SOURCE_DIR}/conf.py" "${DOCS_SOURCE_DIR}/index.md"
    COMMENT "Running sphinx-build..."
)

# Umbrella target.
add_custom_target(docs ALL
    DEPENDS "${DOCS_BUILD_DIR}/index.html"
    COMMENT "Building documentation (Doxygen + Breathe + Sphinx)"
)

# Install the Sphinx HTML output.
install(DIRECTORY "${DOCS_BUILD_DIR}/" DESTINATION ${CMAKE_INSTALL_DOCDIR})
