#!/usr/bin/python -tt
import getopt
import sys
import yaml

def main(args):
    options, _ = getopt.getopt(args, 'i:s:h:')
    inp = None
    src = None
    hdr = None
    for (option, value) in options:
        if option == '-i':
            inp = value
        if option == '-s':
            src = value
        if option == '-h':
            hdr = value
    if not inp:
        print >>sys.stderr, '-i is required'
        return 2
    if not src:
        print >>sys.stderr, '-s is required'
        return 2
    if not hdr:
        print >>sys.stderr, '-h is required'
        return 2

    syscalls = yaml.load(open(inp).read())

    with open(src, 'w') as f:
        make_source(syscalls, f)
    with open(hdr, 'w') as f:
        make_header(syscalls, f)


def make_source(syscalls, f):
    f.write('#include "syscalls/syscalls.h"\n')
    f.write('#include "syscall.h"\n')
    f.write('#include <stddef.h>\n')
    f.write('#include <stdint.h>\n')
    f.write('#include "bits/signal.h"\n')
    f.write('#include "bits/stat.h"\n')
    f.write('#include "bits/syscall.h"\n')
    f.write('#include "bits/types.h"\n')
    f.write('#include "interrupts.h"\n')
    f.write('#include "log.h"\n')
    f.write('\n')
    make_stubs(syscalls, f)
    f.write('\n')
    make_init_function(syscalls, f)
    f.write('\n')


def make_header(syscalls, f):
    f.write('#ifndef SYSCALLS_H\n')
    f.write('#define SYSCALLS_H\n')
    f.write('\n')
    f.write('#include <stddef.h>\n')
    f.write('#include <stdint.h>\n')
    f.write('#include "bits/signal.h"\n')
    f.write('#include "bits/stat.h"\n')
    f.write('#include "bits/syscall.h"\n')
    f.write('#include "bits/types.h"\n')
    f.write('#include "interrupts.h"\n')
    f.write('\n')
    f.write('extern void init_syscalls_array(void);\n')
    f.write('\n')
    make_declarations(syscalls, f)
    f.write('\n')
    f.write('#endif /* SYSCALLS_H */\n')


def make_declarations(syscalls, f):
    fmt = 'extern %s sys_%s(%s)%s;\n'
    for syscall in syscalls:
        args = []
        if 'args' in syscall:
            for arg in syscall['args']:
                args.append('%s %s' % (arg['type'], arg['name']))
        else:
            args.append('void')
        if 'type' in syscall:
            f.write(fmt % (syscall['type'], syscall['name'], ', '.join(args), ''))
        else:
            f.write(fmt % ('void', syscall['name'], ', '.join(args), ' __attribute__((noreturn))'))


def make_stubs(syscalls, f):
    registers = [
        'ebx',
        'ecx',
        'edx',
    ]
    for syscall in syscalls:
        f.write('static void _sys_%s(struct isr_frame* frame) {\n' % syscall['name'])
        args = []
        fmts = []
        fmtargs = []
        for i, arg in enumerate(syscall.get('args', [])):
            f.write('  %s %s = (%s)frame->%s;\n' % (arg['type'], arg['name'], arg['type'], registers[i]))
            args.append(arg['name'])
            fmts.append('%s=%s' % (arg['name'], format_string(arg['type'])))
            if arg['type'][-1] == '*':
                fmtargs.append('(void*)%s' % arg['name'])
            else:
                fmtargs.append(arg['name'])
        f.write('#define LOG(...) log("SYSCALL [%s]", __VA_ARGS__)\n' % syscall['name'])
        if args:
            f.write('  LOG("in:  %s\\n", %s);\n' % (' '.join(fmts), ', '.join(fmtargs)))
        else:
            f.write('  LOG("in:  (none)\\n");\n')
        if 'type' in syscall:
            f.write('  frame->eax = (uint32_t)sys_%s(%s);\n' % (syscall['name'], ', '.join(args)))
            f.write('  LOG("out: %s\\n", (%s)frame->eax);\n' % (format_string(syscall['type']), syscall['type']))
        else:
            f.write('  sys_%s(%s);\n' % (syscall['name'], ', '.join(args)))
            f.write('  LOG("out: (none)\\n");\n')
        f.write('#undef LOG\n')
        f.write('}\n')


def make_init_function(syscalls, f):
    f.write('void init_syscalls_array(void) {\n')
    for syscall in syscalls:
        f.write('  syscalls[__SYSCALL_%s] = _sys_%s;\n' % (syscall['name'], syscall['name']))
    f.write('}\n')


def format_string(type_):
    if type_[-1] == '*':
        return '%8p'
    return {
        'gid_t':       '%d',
        'int':         '%d',
        'intptr_t':    '%ld',
        'mode_t':      '%d',
        'pid_t':       '%d',
        'size_t':      '%ld',
        'ssize_t':     '%d',
        'uid_t':       '%d',
        'uint32_t':    '%ld',
    }[type_]


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
