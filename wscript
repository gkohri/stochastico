#! /usr/bin/env python
# encoding: utf-8
#
#  Copyright 2011 The Stochastico Project
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.
#
#

APPNAME='stochastico'
VERSION='0.1.0'

top='.'
out='build'

def options(opt):
        opt.load('compiler_cxx')

def configure(cnf):
        cnf.check_waf_version(mini='1.6.3')
        cnf.load('compiler_cxx')
        cnf.env.append_unique('CXXFLAGS', ['-O2', '-g', '-std=c++0x',
                              '-Wall','-mtune=native','-march=native'])
        cnf.check_cxx(lib=['m'], uselib_store='M')
        cnf.check_cxx(lib=['rt'], uselib_store='M')
        cnf.check_cxx(lib=['stdc++'], uselib_store='M')
        cnf.check_cxx(lib=['pthread'], uselib_store='M')
        cnf.define('APPNAME',APPNAME)
        cnf.define('VERSION',VERSION)
        cnf.write_config_header('config.h')
        print('→ configuring the project in ' + cnf.path.abspath())


def build(bld):
        srcs = bld.path.ant_glob('src/main/c++/**/*.cc',src='true',bld='true')
        tsrcs = bld.path.ant_glob('src/test/c++/*.cc',src='true',bld='true')
        bld(features='cxx cxxprogram',source=srcs,
            includes = ['.', 'src/main/c++'],
            target=APPNAME, use=['M'])
        bld(features='cxx cxxprogram',source=tsrcs,
            includes = ['.', 'src/main/c++'],
            target='unit-tests', use=['M'])

def dist(ctx):
        ctx.algo      = 'tar.bz2'
        ctx.excl      = '**/.waf-* **/*~ **/*.git **/*.swp **/.lock-* DataSets build junk'
