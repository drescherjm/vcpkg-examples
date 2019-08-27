# These macros initializes each project.

macro( setup_project ProjectName )
	RESET_OPTIMIZATIONS()
endmacro( setup_project )

macro( initialize_project ProjectName)
	setup_naming_convention(${ProjectName})
endmacro( initialize_project )
