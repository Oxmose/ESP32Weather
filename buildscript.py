Import("env")
try:
    import yaml
except ImportError:
    env.Execute("$PYTHONEXE -m pip install pyyaml")
    import yaml

import datetime
import settings.defaultsettings as DefaultSettings


FILENAME_BUILDNO = 'versioning'
FILENAME_VERSION_H = 'include/version.h'
FILENAME_DEFAULT_SETTINGS_CPP = "src/DefaultSettings.cpp"
FILENAME_DEFAULT_SETTINGS_YAML = "settings/default.yaml"

MAJOR = 0
MINOR = 1
version = 'v0.1.'
build_no = 0

FULL_VERSION = ""

def is_pio_build():
    from SCons.Script import DefaultEnvironment
    env = DefaultEnvironment()
    if "IsCleanTarget" in dir(env) and env.IsCleanTarget(): return False
    return not env.IsIntegrationDump()

def update_versioning():
    global build_no
    global FULL_VERSION

    build_no = 0
    try:
        with open(FILENAME_BUILDNO) as f:
            build_no = int(f.readline()) + 1
    except:
        print('Starting build number from 1..')
        build_no = 1
    with open(FILENAME_BUILDNO, 'w+') as f:
        f.write(str(build_no))
        print('Build number: {}'.format(build_no))

    hf = """
    #ifndef BUILD_NUMBER
        #define BUILD_NUMBER "{}"
    #endif
    #ifndef VERSION
        #define VERSION "{}.{}.{} {}"
    #endif
    #ifndef VERSION_SHORT
        #define VERSION_SHORT "{}.{}.{}"
    #endif
    """.format(
        build_no,
        MAJOR, MINOR, build_no, str(datetime.datetime.now())[:-7],
        MAJOR, MINOR, build_no)

    FULL_VERSION = "{}.{}.{} {}".format(MAJOR, MINOR, build_no, str(datetime.datetime.now())[:-7])
    with open(FILENAME_VERSION_H, 'w+') as f:
        f.write(hf)

def generate_default_settings():
    DefaultSettings.GenerateDerfaultSettings(FILENAME_DEFAULT_SETTINGS_YAML, FILENAME_DEFAULT_SETTINGS_CPP)

if is_pio_build():

    generate_default_settings()
    update_versioning()