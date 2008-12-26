# -*- mode: python -*-
# Common definitions for all SConstruct files

opts = Variables('custom.py')
opts.AddVariables(
    BoolVariable('VERBOSE', 'output the commands used to build', False),
)

# Create the build environment
env = Environment(options=opts)
env.Append(CFLAGS=['-Wall',
                   '-Werror',
                   '-Wextra',
                   '-Wno-unused-parameter',
                   '-std=gnu99'])

if not env['VERBOSE']:
    # Set the output patterns when verbose is disabled
    env['CCCOMSTR']     = '  CC                $TARGET'
    env['CXXCOMSTR']    = '  CXX               $TARGET'
    env['LINKCOMSTR']   = '  LD                $TARGET'
    env['SHCCCOMSTR']   = '  CC       [shared] $TARGET'
    env['SHLINKCOMSTR'] = '  LD       [shared] $TARGET'
    env['ARCOMSTR']     = '  AR                $TARGET'
    env['RANLIBCOMSTR'] = '  RANLIB            $TARGET'
    env['ASCOMSTR']     = '  AS                $TARGET'
    env['ASPPCOMSTR']   = '  ASPP              $TARGET'

# Return the environment to the calling SConscript file
Return('env')
