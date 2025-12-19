import yaml

def BuildFileStart(sourceFile):
    sourceFile.write(
        "/*******************************************************************************\n" +
        " * @file SettingsDefault.cpp\n" +
        " *\n" +
        " * @see Settings.h\n" +
        " *\n" +
        " * @author Alexy Torres Aurora Dugo\n" +
        " *\n" +
        " * @date 18/12/2025\n" +
        " *\n" +
        " * @version 1.0\n" +
        " *\n" +
        " * @brief Weather Station Firmware default setting repository.\n" +
        " *\n" +
        " * @details Weather Station Firmware default setting repository. This file \n" +
        " * is auto-generated and contains the default settings used by the firmware.\n" +
        " *\n" +
        " * @copyright Alexy Torres Aurora Dugo\n" +
        " ******************************************************************************/\n" +
        "\n" +
        "/*******************************************************************************\n" +
        " * INCLUDES\n" +
        " ******************************************************************************/\n" +
        "#include <map>        /* Standard hashmap */\n" +
        "#include <cstdint>    /* Standard int types */\n" +
        "#include <Settings.h> /* Settings */\n" +
        "\n" +
        "/*******************************************************************************\n" +
        " * CONSTANTS\n" +
        " ******************************************************************************/\n" +
        "\n" +
        "/*******************************************************************************\n" +
        " * STRUCTURES AND TYPES\n" +
        " ******************************************************************************/\n" +
        "/* None */\n" +
        "\n" +
        "/*******************************************************************************\n" +
        " * MACROS\n" +
        " ******************************************************************************/\n" +
        "/* None */\n" +
        "\n" +
        "/*******************************************************************************\n" +
        " * STATIC FUNCTIONS DECLARATIONS\n" +
        " ******************************************************************************/\n" +
        "/* None */\n" +
        "\n" +
        "/*******************************************************************************\n" +
        " * GLOBAL VARIABLES\n" +
        " ******************************************************************************/\n" +
        "\n" +
        "/************************* Imported global variables **************************/\n" +
        "/* None */\n" +
        "\n" +
        "/************************* Exported global variables **************************/\n" +
        "/* None */\n" +
        "\n" +
        "/************************** Static global variables ***************************/\n"
    )


def BuildSettings(sourceFile, settingPath):
    with open(settingPath, 'r') as file:

        table = {}

        # Load the settings
        loaded = {}
        try:
            loaded = yaml.safe_load(file)
        except yaml.YAMLError as exc:
            print(exc)

        for key, value in loaded.items():
            print("==== Setting: {}".format(key))
            sourceFile.write("/** @brief Default setting for {} item. */\n".format(key))
            sourceFile.write("static const {} sk{} = {};\n".format(value["type"], key, value["value"]))

        sourceFile.write("\n")
        return loaded

def BuildFileNext(sourceFile):
    sourceFile.write(
        "/*******************************************************************************\n" +
        " * FUNCTIONS\n" +
        " ******************************************************************************/\n" +
        "/* None */\n" +
        "\n" +
        "/*******************************************************************************\n" +
        " * CLASS METHODS\n" +
        " ******************************************************************************/\n"
    )

def BuildFileInit(sourceFile, settings):
    sourceFile.write("void Settings::InitializeDefault(void) noexcept {\n")
    for key, value in settings.items():
            addRef = True
            if '*' in value["type"]:
                addRef = False
            sourceFile.write(
                f"\tthis->_defaults.emplace(\n" +
                f"\t\t{'SETTING_' + key.upper()},\n" +
                f"\t\tS_SettingField {{\n")

            if addRef:
                sourceFile.write(f"\t\t\t.pValue = (uint8_t*)&sk{key},\n")
            else:
                sourceFile.write(f"\t\t\t.pValue = (uint8_t*)sk{key},\n")

            sourceFile.write(
                f"\t\t\t.fieldSize = {value["size"]}\n" +
                f"\t\t}}\n" +
                f"\t);\n")

    sourceFile.write("}")


def GenerateDerfaultSettings(settingPath, sourcePath):
    with open(sourcePath, 'w') as sourceFile:
        BuildFileStart(sourceFile)
        settings = BuildSettings(sourceFile, settingPath)
        BuildFileNext(sourceFile)
        BuildFileInit(sourceFile, settings)