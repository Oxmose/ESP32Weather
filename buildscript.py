import datetime
import shutil

Import("env")

FILENAME_BUILDNO = 'versioning'
FILENAME_VERSION_H = 'include/version.h'

MAJOR = 0
MINOR = 1
version = 'v0.1.'
build_no = 0

FULL_VERSION = ""

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


update_versioning()