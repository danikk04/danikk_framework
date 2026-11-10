project(
	'danikk_framework', 
	'cpp',
    default_options: [
        'cpp_std=c++20',
    ])

libzip_dep = dependency('libzip')
gumbo_dep = dependency('gumbo', required : false)

if not gumbo_dep.found()
	message('Gumbo not found; HTML implementation will not be compiled.')
endif

subdir('source')

include = [include_directories('include'), include_directories('include_lib')]

#add_project_arguments('-O3', language : 'cpp')

cpp_args = [
	'-Wvla',
	'-Wclass-memaccess',
	'-Wunused-function'
]

framework_dependencies = [libzip_dep]
if gumbo_dep.found()
	framework_dependencies += [gumbo_dep]
endif

lib = static_library(
    'danikk_framework', 
    source,
    include_directories : include,
    cpp_args : cpp_args,
    dependencies : framework_dependencies
)

dep = declare_dependency(
	link_with : lib,
	include_directories : include,
	dependencies : framework_dependencies
)
