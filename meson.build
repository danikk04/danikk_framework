project(
	'danikk_framework', 
	'cpp',
    default_options: [
        'cpp_std=c++20',
    ])

subdir('source')

include = [include_directories('include'), include_directories('include_lib')]

#add_project_arguments('-O3', language : 'cpp')

cpp_args = [
	'-Wvla',
	'-Wclass-memaccess',
	'-Wunused-function'
]

lib = static_library(
    'danikk_framework', 
    source,
    include_directories : include,
    cpp_args : cpp_args
)

dep = declare_dependency(
	link_with : lib,
	include_directories : include
)
