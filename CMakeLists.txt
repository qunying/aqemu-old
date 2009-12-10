# AQEMU CMake Project
PROJECT( AQEMU )
CMAKE_MINIMUM_REQUIRED( VERSION 2.4.0 )

# Verbose compiler output
SET( CMAKE_VERBOSE_MAKEFILE ON )

# For find VNC libs
SET( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules )

# Files
SET( aqemu_headers
	About_Window.h
	Add_New_Device_Window.h
	Add_Port_Window.h
	Add_USB_Port_Window.h
	Advanced_Settings_Window.h
	Convert_HDD_Image_Window.h
	Copy_VM_Window.h
	Create_HDD_Image_Window.h
	Create_Template_Window.h
	Device_Manager_Widget.h
	Edit_Emulator_Version_Window.h
	Emulator_Control_Window.h
	Error_Log_Window.h
	First_Start_Wizard.h
	HDD_Image_Info.h
	Main_Window.h
	Monitor_Window.h
	Network_Widget.h
	Old_Network_Widget.h
	Ports_Tab_Widget.h
	Properties_Window.h
	QEMU_Binary_Window.h
	Select_Icon_Window.h
	Settings_Window.h
	Snapshot_Properties_Window.h
	Snapshots_Window.h
	System_Info.h
	Utils.h
	VM.h
	VM_Devices.h
	VM_Wizard_Window.h
	VNC_Password_Window.h
)

SET( aqemu_sources
	About_Window.cpp
	Add_New_Device_Window.cpp
	Add_Port_Window.cpp
	Add_USB_Port_Window.cpp
	Advanced_Settings_Window.cpp
	Convert_HDD_Image_Window.cpp
	Copy_VM_Window.cpp
	Create_HDD_Image_Window.cpp
	Create_Template_Window.cpp
	Device_Manager_Widget.cpp
	Edit_Emulator_Version_Window.cpp
	Emulator_Control_Window.cpp
	Error_Log_Window.cpp
	First_Start_Wizard.cpp
	HDD_Image_Info.cpp
	main.cpp
	Main_Window.cpp
	Monitor_Window.cpp
	Network_Widget.cpp
	Old_Network_Widget.cpp
	Ports_Tab_Widget.cpp
	Properties_Window.cpp
	QEMU_Binary_Window.cpp
	Select_Icon_Window.cpp
	Settings_Window.cpp
	Snapshot_Properties_Window.cpp
	Snapshots_Window.cpp
	System_Info.cpp
	Utils.cpp
	VM.cpp
	VM_Devices.cpp
	VM_Wizard_Window.cpp
	VNC_Password_Window.cpp
)

SET( aqemu_ui
	About_Window.ui
	Add_New_Device_Window.ui
	Add_Port_Window.ui
	Add_USB_Port_Window.ui
	Advanced_Settings_Window.ui
	Convert_HDD_Image_Window.ui
	Copy_VM_Window.ui
	Create_HDD_Image_Window.ui
	Create_Template_Window.ui
	Device_Manager_Widget.ui
	Edit_Emulator_Version_Window.ui
	Emulator_Control_Window.ui
	Error_Log_Window.ui
	First_Start_Wizard.ui
	Main_Window.ui
	Monitor_Window.ui
	Network_Widget.ui
	Old_Network_Widget.ui
	Ports_Tab_Widget.ui
	Properties_Window.ui
	QEMU_Binary_Window.ui
	Select_Icon_Window.ui
	Settings_Window.ui
	Snapshot_Properties_Window.ui
	Snapshots_Window.ui
	VM_Wizard_Window.ui
	VNC_Password_Window.ui
)

SET( aqemu_resouces shared_images.qrc )

SET( translations Russian.ts )

SET( files_to_translate ${files_to_translate} ${aqemu_headers} ${aqemu_sources} ${aqemu_ui} PARENT_SCOPE )


# Check Qt
SET( QT_MIN_VERSION "4.4.2" )
FIND_PACKAGE( Qt4 REQUIRED )
INCLUDE( ${QT_USE_FILE} )
SET( QT_USE_QTXML TRUE )
SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTTEST TRUE )


# Embedded VNC display
OPTION( WITHOUT_EMBEDDED_DISPLAY "Build without support Embedded VNC Display" )

IF( NOT WITHOUT_EMBEDDED_DISPLAY )
	FIND_PACKAGE( LibVNCServer REQUIRED )
	
	SET( aqemu_headers ${aqemu_headers} Machine_View.h Remote_View.h VNC_Client_Thread.h VNC_View.h )
	SET( aqemu_sources ${aqemu_sources} Machine_View.cpp Remote_View.cpp VNC_Client_Thread.cpp VNC_View.cpp )
	
	ADD_DEFINITIONS( -Wall -g ${QT_DEFINITIONS} -DVNC_DISPLAY )
	ADD_LIBRARY( libvncclient SHARED ${aqemu_headers} ${aqemu_sources} )
	
	INCLUDE_DIRECTORIES( ${VNCCLIENT_INCLUDE_DIRS} )
ELSE( NOT WITHOUT_EMBEDDED_DISPLAY )
	ADD_DEFINITIONS( -Wall -g ${QT_DEFINITIONS} )
ENDIF( NOT WITHOUT_EMBEDDED_DISPLAY )


# MOC and other Qt stuff
QT4_WRAP_UI( ui_headers ${aqemu_ui} )
QT4_WRAP_CPP( moc_srcs ${aqemu_headers} )
QT4_ADD_RESOURCES( moc_srcs ${aqemu_resouces} )


# For translation
OPTION( UPDATE_TRANSLATIONS "Update source translation files (WARNING: make clean will delete the source .ts files!)" )

IF( UPDATE_TRANSLATIONS )
	QT4_CREATE_TRANSLATION( qm_files ${CMAKE_CURRENT_SOURCE_DIR} ${files_to_translate} ${translations} )
ELSE( UPDATE_TRANSLATIONS )
	QT4_ADD_TRANSLATION( qm_files ${translations} )
ENDIF( UPDATE_TRANSLATIONS )

ADD_CUSTOM_TARGET( translations_target DEPENDS ${qm_files} )


# Building
INCLUDE_DIRECTORIES(
	${QT_INCLUDES}
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_INCLUDE_DIR}
	${CMAKE_BINARY_DIR}
	${QT_QTNETWORK_INCLUDE_DIR}
	${QT_QTTEST_INCLUDE_DIR}
)

IF( COMMAND CMAKE_POLICY )
	CMAKE_POLICY( SET CMP0003 NEW )
ENDIF( COMMAND CMAKE_POLICY )

ADD_EXECUTABLE( aqemu
	${aqemu_sources}
	${ui_headers}
	${moc_srcs}
	${qm_files}
)

TARGET_LINK_LIBRARIES( aqemu
	${QT_LIBRARIES}
	${QT_QTCORE_LIBRARY}
	${QT_QTGUI_LIBRARY}
	${QT_QTNETWORK_LIBRARIES}
	${QT_QTTEST_LIBRARIES}
	${QT_QTXML_LIBRARY}
)

# Embedded VNC display
IF( NOT WITHOUT_EMBEDDED_DISPLAY )
	TARGET_LINK_LIBRARIES( aqemu ${LIBVNCCLIENT_LIBRARIES} )
ENDIF( NOT WITHOUT_EMBEDDED_DISPLAY )

# Install files
INSTALL( TARGETS aqemu DESTINATION bin )

FILE( GLOB translations "${CMAKE_CURRENT_SOURCE_DIR}/*.qm" )
INSTALL( FILES ${translations} DESTINATION share/aqemu )

FILE( GLOB gui_icons "${CMAKE_CURRENT_SOURCE_DIR}/*.rcc" )
INSTALL( FILES ${gui_icons} DESTINATION share/aqemu/ )

FILE( GLOB os_icons "${CMAKE_CURRENT_SOURCE_DIR}/os_icons/*.*" )
INSTALL( FILES ${os_icons} DESTINATION share/aqemu/os_icons )

FILE( GLOB os_templates "${CMAKE_CURRENT_SOURCE_DIR}/os_templates/*.*")
INSTALL( FILES ${os_templates} DESTINATION share/aqemu/os_templates )

FILE( GLOB links "${CMAKE_CURRENT_SOURCE_DIR}/aqemu_links.html" )
INSTALL( FILES ${links} DESTINATION share/aqemu )

FILE( GLOB pixmaps "${CMAKE_CURRENT_SOURCE_DIR}/menu_data/*.png" )
INSTALL( FILES ${pixmaps} DESTINATION share/pixmaps )

FILE( GLOB app_menu "${CMAKE_CURRENT_SOURCE_DIR}/menu_data/aqemu.desktop" )
INSTALL( FILES ${app_menu} DESTINATION share/applications )


# Create docs .bz2 files
MACRO( create_docs _sources )
	FOREACH( curFile ${ARGN} )
		GET_FILENAME_COMPONENT( _in ${curFile} ABSOLUTE )
		GET_FILENAME_COMPONENT( _basename ${curFile} NAME_WE )
		
		SET( _out ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.bz2 )
		
		ADD_CUSTOM_COMMAND(
            OUTPUT ${_out}
            COMMAND bzip2
            ARGS -zkf9 ${_in}
            DEPENDS ${_in}
        )
		
		SET( ${_sources} ${${_sources}} ${_out} )
	ENDFOREACH( curFile )
ENDMACRO( create_docs )

CREATE_DOCS( docs_bz2 AUTHORS CHANGELOG README TODO )

ADD_CUSTOM_TARGET( docs ALL DEPENDS ${docs_bz2} )

INSTALL( FILES ${docs_bz2} DESTINATION share/doc/aqemu )