# https://gist.github.com/cpradog/aad88d51001ea83ecfc6

import os
import re
import subprocess
import ycm_core

def LoadSystemIncludes():
    regex = re.compile(r'(?:\#include \<...\> search starts here\:)(?P<list>.*?)(?:End of search list)', re.DOTALL)
    process = subprocess.Popen(['clang', '-v', '-E', '-x', 'c++', '-'],
                               stdin=subprocess.PIPE,
                               stdout=subprocess.PIPE,
                               stderr=subprocess.PIPE);
    process_out, process_err = process.communicate('');
    output = process_out + process_err;
    includes = [];

    for p in re.search(regex, str(output).encode('utf8').decode('unicode_escape')).group('list').split('\n'):
        p = p.strip();
        if len(p) > 0 and p.find('(framework directory)') < 0:
            includes.append('-isystem');
            includes.append(p);

    return includes;

SOURCE_EXTENSIONS = [ '.cpp', '.cxx', '.cc', '.c', '.m', '.mm' ]
scriptPath = os.path.dirname(os.path.abspath(__file__));
compilation_database_folder = os.path.join(scriptPath, 'build')
database = None if not os.path.exists(compilation_database_folder) else ycm_core.CompilationDatabase(compilation_database_folder)

flags = [
    '-Wall',
    '-std=c++11',
    '-x',
    'c++',
    '-I',
    scriptPath + '/include',
    '-isystem',
    scriptPath + '/third_party/cute',
    '-Wall',
    '-Wextra',
    '-pedantic',
    '-Werror'
]

systemIncludes = LoadSystemIncludes();
flags = flags + systemIncludes;

def MakeRelativePathsInFlagsAbsolute( flags, working_directory ):
    if not working_directory:
        return list(flags)

    new_flags = []
    make_next_absolute = False
    path_flags = ['-isystem', '-I', '-iquote', '--sysroot=']

    for flag in flags:
        new_flag = flag

        if make_next_absolute:
            make_next_absolute = False
            if not flag.startswith('/'):
                new_flag = os.path.join(working_directory, flag)

        for path_flag in path_flags:
            if flag == path_flag:
                make_next_absolute = True
            break

            if flag.startswith(path_flag):
                path = flag[len(path_flag):]
                new_flag = path_flag + os.path.join(working_directory, path)
                break

        if new_flag:
            new_flags.append(new_flag)
    return new_flags

def IsHeaderFile(filename):
    extension = os.path.splitext(filename)[1]
    return extension in ['.h', '.hxx', '.hpp', '.hh']

def GetCompilationInfoForFile(filename):
    if IsHeaderFile(filename):
        basename = os.path.splitext(filename)[0]
        for extension in SOURCE_EXTENSIONS:
            replacement_file = basename + extension
            if os.path.exists(replacement_file):
                compilation_info = database.GetCompilationInfoForFile(replacement_file)
                if compilation_info.compiler_flags_:
                    return compilation_info
        return None
    return database.GetCompilationInfoForFile(filename)

def FlagsForFile(filename, **kwargs):
    final_flags = MakeRelativePathsInFlagsAbsolute(flags, scriptPath)
    return { 'flags': final_flags, 'do_cache': True }
