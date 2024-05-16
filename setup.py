import os
import sys
import platform
import re
import subprocess

from setuptools import setup, Extension, find_namespace_packages
from setuptools.command.build_ext import build_ext


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir='', namespace=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)
        self.namespace = namespace


class CMakeBuild(build_ext):
    def build_extension(self, ext):
        self.package = ext.namespace
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        # required for auto-detection of auxiliary "native" libs
        if not extdir.endswith(os.path.sep):
            extdir += os.path.sep

        # check if Z3 should be used. If no argument is given, pyfiction does not depend on Z3 by default.
        z3 = os.environ.get("z3", "OFF")

        # store environment variable
        with open(".env", "w") as env_file:
            env_file.write(f"z3={z3}")

        cmake_args = ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable,
                      '-DMOCKTURTLE_EXAMPLES=OFF',
                      '-DFICTION_PROGRESS_BARS=OFF',
                      '-DFICTION_CLI=OFF',
                      '-DFICTION_TEST=OFF',
                      '-DFICTION_EXPERIMENTS=OFF',
                      '-DFICTION_BENCHMARK=OFF',
                      '-DFICTION_ENABLE_IPO=ON',
                      '-DFICTION_ENABLE_PCH=ON',
                      '-DFICTION_ENABLE_UNITY_BUILD=ON',
                      '-DFICTION_PYTHON_BINDINGS=ON',
                      f'-DFICTION_Z3={z3}',
                      ]

        if "Z3_ROOT" in os.environ:
            cmake_args += ['-DZ3_ROOT={}'.format(os.environ.get("Z3_ROOT"))]

        if self.compiler.compiler_type != "msvc":
            # Using Ninja-build since it a) is available as a wheel and b)
            # multithreads automatically. MSVC would require all variables be
            # exported for Ninja to pick it up, which is a little tricky to do.
            # Users can override the generator with CMAKE_GENERATOR in CMake
            # 3.15+.
            cmake_generator = os.environ.get("CMAKE_GENERATOR", "")
            if not cmake_generator:
                cmake_args += ["-GNinja"]

        cfg = 'Debug' if self.debug else 'Release'
        cmake_args += ['-DCMAKE_BUILD_TYPE={}'.format(cfg)]
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            cmake_args += ['-T', 'ClangCl']
            if sys.maxsize > 2 ** 32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cpus = os.cpu_count()
            if cpus is None:
                cpus = 2
            build_args += ['--', f'-j{cpus}']

        # cross-compile support for macOS - respect ARCHFLAGS if set
        if sys.platform.startswith("darwin"):
            archs = re.findall(r"-arch (\S+)", os.environ.get("ARCHFLAGS", ""))
            if archs:
                arch_argument = "-DCMAKE_OSX_ARCHITECTURES={}".format(";".join(archs))
                print('macOS building with: ', arch_argument, flush=True)
                cmake_args += [arch_argument]

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        else:
            os.remove(os.path.join(self.build_temp, 'CMakeCache.txt'))

        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.', '--target', ext.name] + build_args, cwd=self.build_temp)


README_PATH = os.path.join(os.path.abspath(os.path.dirname(__file__)), 'README.md')
with open(README_PATH, encoding="utf8") as readme_file:
    README = readme_file.read()

setup(
    name='mnt.pyfiction',
    version='0.6.1',
    author='Marcel Walter',
    author_email='marcel.walter@tum.de',
    description='Design Automation for Field-coupled Nanotechnologies',
    long_description=README,
    long_description_content_type='text/markdown',
    license='MIT',
    url='https://www.cda.cit.tum.de/research/fcn/',
    ext_modules=[CMakeExtension('pyfiction', namespace='mnt')],
    cmdclass={"build_ext": CMakeBuild},
    zip_safe=False,
    python_requires=">=3.8",
    classifiers=[
        'Development Status :: 4 - Beta',
        'Programming Language :: Python :: 3',
        'Programming Language :: C++',
        'Environment :: Console',
        'License :: OSI Approved :: MIT License',
        'Operating System :: Microsoft :: Windows',
        'Operating System :: MacOS',
        'Operating System :: POSIX :: Linux',
        'Intended Audience :: Science/Research',
        'Natural Language :: English',
        'Topic :: Scientific/Engineering :: Electronic Design Automation (EDA)',
    ],
    keywords='MNT fiction nanotechnology fcn qca nml sidb design automation placement routing clocking simulation',
    project_urls={
        'Source': 'https://github.com/cda-tum/fiction',
        'Tracker': 'https://github.com/cda-tum/fiction/issues',
        'Documentation': 'https://fiction.readthedocs.io/en/latest/',
        'Research': 'https://www.cda.cit.tum.de/research/fcn/',
    }
)
