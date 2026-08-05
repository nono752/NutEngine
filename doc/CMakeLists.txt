find_package(Doxygen OPTIONAL_COMPONENTS dot)

if(DOXYGEN_FOUND)
    # if Graphviz installed, generate inheritance/inclusion graphs
    if(Doxygen_dot_FOUND)
        set(DOXYGEN_HAVE_DOT "YES")

        set(DOXYGEN_CLASS_GRAPH YES) # inheritance graphs
        set(DOXYGEN_COLLABORATION_GRAPH YES) # classes dependencies
        set(DOXYGEN_INCLUDE_GRAPH YES) # tree of inclusions

        set(DOXYGEN_UML_LOOK YES)

        message(STATUS "Graphviz found.")
    else()
        message(STATUS "Graphviz not found, continue configuration.")
    endif()
    
    # global setup
    set(DOXYGEN_GENERATE_HTML "YES")
    set(DOXYGEN_GENERATE_LATEX "NO")
    set(DOXYGEN_RECURSIVE "YES") # find recursivly subdirectories
    set(DOXYGEN_SHOW_FILES "NO")
    set(DOXYGEN_WARNINGS "YES")
    set(DOXYGEN_WARN_IF_UNDOCUMENTED "YES")
    set(DOXYGEN_EXTRACT_ALL "NO") # document only doxygen commented
    
    # DOC USER (public API) 
    set(DOXYGEN_PROJECT_NAME "NutEngine")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs_public")
    set(DOXYGEN_EXTRACT_PRIVATE "NO") # don't show private methods
    set(DOXYGEN_INTERNAL_DOCS "NO") # don't show \internal balised
    set(DOXYGEN_EXCLUDE 
        "${CMAKE_SOURCE_DIR}/README.md" # avoid using it for mainpage
        "${CMAKE_CURRENT_SOURCE_DIR}/dev" # don't need it here
    )
    doxygen_add_docs(
        doc_public
        ${CMAKE_SOURCE_DIR}/src
        COMMENT "Generate doxygen public documentation..."
    )
 
    # DOC DEV
    set(DOXYGEN_PROJECT_NAME "NutEngine (Dev)")
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/docs_dev")
    set(DOXYGEN_EXTRACT_PRIVATE "YES")
    set(DOXYGEN_INTERNAL_DOCS "YES")
    set(DOXYGEN_EXCLUDE 
        "${CMAKE_SOURCE_DIR}/README.md"
    )
    doxygen_add_docs(
        doc_dev 
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Generate doxygen dev documentation..."
    )
    
    message(STATUS "Doxygen found, targets 'doc_public' and 'doc_dev' configured.")
else()
    message(STATUS "Doxygen not found, documentation targets disabled.")
endif()