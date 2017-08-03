# pylint: disable=missing-docstring
from conans import ConanFile, CMake


class LibdabdemodConan(ConanFile):
    name = 'libdabdemod'
    description = 'The demodulation subsystem for the ODR DAB data toolkit'
    license = 'BSD 3-clause'
    version = '1.0.1'
    url = 'https://github.com/Opendigitalradio/libdabdemod.git'
    settings = ['os', 'compiler', 'build_type', 'arch']
    options = {'shared': [True, False]}
    default_options = 'shared=True'
    generators = ['cmake', 'txt']
    exports_sources = (
        'cmake/*',
        'CMakeLists.txt',
        'include/*',
        'src/*'
    )

    def build(self):
        cmake = CMake(self)
        lib = '-DBUILD_SHARED_LIBS=%s' % ('On' if self.options.shared else 'Off')
        args = [lib, '-DCMAKE_INSTALL_PREFIX="%s"' % self.package_folder]
        self.run('cmake %s %s %s'
                 % (self.source_folder,
                    cmake.command_line,
                    ' '.join(args)))
        self.run('cmake --build . --target install %s' % cmake.build_config)

    def package_info(self):
        self.cpp_info.libs = ['dabdemod']

    def requirements(self):
        self.requires('libdabcommon/[>=1.0]@fmorgner/stable')
