# -*- mode: python -*-
# Common definitions for all SConstruct files
import os
import SCons

Import('opts')

# Create common build environment
env = Environment(options=opts)
env.Append(CFLAGS=[
    '-Wall',
    '-Werror',
    '-Wextra',
    '-Wno-unused-parameter',
    '-std=c99',
])
if env["COLOR"]:
    env.Append(CFLAGS=["-fdiagnostics-color"])

# Create a builder for mkisofs
mkisofs_builder = Builder(action=Action('$MKISOFSCOM', '$MKISOFSCOMSTR'))
env.Append(BUILDERS={'ISO': mkisofs_builder})
env['MKISOFS'] = ARGUMENTS.get('MKISOFS', 'mkisofs')
env['MKISOFSCOM'] = '$MKISOFS $MKISOFSFLAGS -o $TARGET $ROOT'
env['MKISOFSCOMSTR'] = '$MKISOFSCOM'

# Create a builder for sed
env.Append(BUILDERS={'Sed': Builder(action=Action('$SEDCOM', '$SEDCOMSTR'))})
env['SEDCOM'] = 'sed -e"$PATTERN" < $SOURCE > $TARGET'
env['SEDCOMSTR'] = '$SEDCOM'

# Create a builder for strip
env.Append(BUILDERS={'Strip': Builder(action=Action('$STRIPCOM', '$STRIPCOMSTR'))})
env['STRIPCOM'] = '$STRIP -s -o $TARGET $SOURCE'
env['STRIPCOMSTR'] = '$STRIPCOM'

# Set the output patterns when verbose is disabled
if not env['V']:
    env['CCCOMSTR']      = '  CC                $TARGET'
    env['LINKCOMSTR']    = '  LD                $TARGET'
    env['ASPPCOMSTR']    = '  ASPP              $TARGET'
    env['MKISOFSCOMSTR'] = '  MKISOFS           $TARGET'
    env['SEDCOMSTR']     = '  SED               $TARGET'
    env['COPYSTR']       = '  COPY              $TARGET'
    env['STRIPCOMSTR']   = '  STRIP             $TARGET'
    SCons.Script.Copy.strfunc = lambda d, s: env.subst('$COPYSTR', 1, target=d)

# Return the environment to the calling SConscript file
Return('env')
