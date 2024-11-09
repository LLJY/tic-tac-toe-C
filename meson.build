project('ttt-basic', 'c')

# Source files
src_files = files(
    'src/util.c',
    'src/main.c',
    'src/tui.c',
    'src/game.c',
    'src/linked_list.c',
    'src/gui.c',
    'src/minimax.c',
    'src/deep_q.c',
    'src/sound.c'
    # Add any other specific source files here if needed
)

# Include directories
incdir = include_directories('include')


# Optimization flags
optimization_flags = [
  '-O3',                # Optimize for speed
  '-march=native',      # Optimize for the current CPU architecture
  '-flto',              # Enable link-time optimization
  '-lm',           
  '-Wall'               # Enable all warnings
]

gtkdep = dependency('gtk+-3.0', required : true)
tensorflow_dep = dependency('tensorflow', required : true)
gst_dep = dependency('gstreamer-1.0', required: true)

if host_machine.system() == 'windows'
  subsystem = 'windows' 
else 
  subsystem = 'console'
endif

# Executable
executable('ttt-basic', 
           sources: src_files,  # List all source files here
           include_directories: incdir,
           c_args: optimization_flags,
           dependencies : [gtkdep, tensorflow_dep, gst_dep],
           win_subsystem: subsystem
)
out_dir = 'out'
copy = find_program('cp')
mkdir = find_program('mkdir')
run_command(mkdir, '-p', out_dir)
run_command(copy, 'build/ttt-basic.exe', out_dir)
run_command(copy, 'r', 'weights', out_dir)
run_command(copy, 'sounds/*', out_dir)
if host_machine.system() == 'windows'
  run_command(copy, 'lib/*', out_dir)
endif


# Example for adding more source files in the future:
# Just add the new source file to the 'sources' list
# e.g., if you add src/utils.c:
# executable('myproject', 
#            sources: [main_c, gui_c, 'src/utils.c'], 
#            include_directories: incdir
# )

# Example for adding a subdirectory with source files:
#   - Create a new subdirectory, e.g., 'src/network'
#   - Add source files to the new subdirectory, e.g., 'src/network/client.c'
#   - In meson.build:
#     network_sources = files(
#       'src/network/client.c',
#       # ... more network source files
#     )
#     executable('myproject', 
#                sources: [main_c, gui_c, network_sources], 
#                include_directories: incdir
#     )

# Example for linking with external libraries:
#   - Use dependency() to find external libraries
#   - Add the dependency to the 'link_with' keyword argument
#   - e.g., to link with GTK4:
#     gtk4_dep = dependency('gtk4')
#     executable('myproject', 
#                sources: [main_c, gui_c], 
#                include_directories: incdir,
#                link_with: gtk4_dep
#     )