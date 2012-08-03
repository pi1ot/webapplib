# project
PROJECT( WebAppLib )
CMAKE_MINIMUM_REQUIRED( VERSION 2.8 )
SET( CMAKE_CXX_FLAGS_RELEASE "-Wall -O2 -fPIC" )
SET( CMAKE_INSTALL_PREFIX /usr/local )

# source files
SET( WEBAPPLIB_SRCS waString.cpp waFileSystem.cpp waTemplate.cpp 
    waHttpClient.cpp waEncode.cpp waDateTime.cpp waTextFile.cpp 
    waConfigFile.cpp waUtility.cpp )
# header files    
SET( WEBAPPLIB_INCS waString.h waFileSystem.h waTemplate.h 
    waHttpClient.h waEncode.h waDateTime.h waTextFile.h 
    waConfigFile.h waUtility.h webapplib.h )

# find mysql
FIND_PATH( MYSQL_INCLUDE mysql.h 
    /usr/include/mysql /usr/local/mysql/include/mysql ) 
FIND_LIBRARY( MYSQL_LIBRARY mysqlclient  
    /usr/lib/mysql /usr/local/mysql/lib/mysql )

# waMysqlClient
IF( MYSQL_INCLUDE AND MYSQL_LIBRARY )
    MESSAGE( STATUS "MySQL found: " ${MYSQL_INCLUDE} )
    # include waMysqlClient
    INCLUDE_DIRECTORIES( ${MYSQL_INCLUDE} )
    LIST( APPEND WEBAPPLIB_SRCS waMysqlClient.cpp )
    LIST( APPEND WEBAPPLIB_INCS waMysqlClient.h )    
ELSE( MYSQL_INCLUDE AND MYSQL_LIBRARY )
    MESSAGE( STATUS "MySQL not found" )
    # do not include waMysqlClient
    ADD_DEFINITIONS( -D_WEBAPPLIB_NOMYSQL ) 
ENDIF( MYSQL_INCLUDE AND MYSQL_LIBRARY )

# build library
ADD_LIBRARY( webapp SHARED ${WEBAPPLIB_SRCS} )
ADD_LIBRARY( webapp_static STATIC ${WEBAPPLIB_SRCS} )
TARGET_LINK_LIBRARIES( webapp ${MYSQL_LIBRARY} )

# rename libwebapp_static.a to libwebapp.a
SET_TARGET_PROPERTIES( webapp_static PROPERTIES OUTPUT_NAME "webapp" )
# keep libwebapp.so
SET_TARGET_PROPERTIES( webapp PROPERTIES CLEAN_DIRECT_OUTPUT 1 ) 
# keep libwebapp.a
SET_TARGET_PROPERTIES( webapp_static PROPERTIES CLEAN_DIRECT_OUTPUT 1 )
# libwebapp.so version and so-name
SET_TARGET_PROPERTIES( webapp PROPERTIES VERSION 1.1 SOVERSION 1 ) 
	
# install
INSTALL( TARGETS webapp webapp_static 
    LIBRARY DESTINATION lib  
    ARCHIVE DESTINATION lib )
INSTALL( FILES ${WEBAPPLIB_INCS} DESTINATION include/webapplib )

