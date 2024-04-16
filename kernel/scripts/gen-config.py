#   Lunaris kernel config generator

def config_gen(config_file, header_file):
    config_lines = []
    try:
        with open(config_file, 'r') as f:
            config_lines = f.readlines()
    except: # womp womp, no .config file
        print(".config not found!")
        exit(1)

    header_lines = []
    header_lines.append("#ifndef __CONFIG_H")
    header_lines.append("#define __CONFIG_H")
    header_lines.append("")
    for line in config_lines:
        line = line.strip()
        if line.startswith("#") or line == "":
            continue
        elif "CONFIG_" in line:
            option, value = line.split("=")
            option = option.strip()
            value = value.strip()
            if value.isdigit():  # Check if the value is a number
                header_lines.append("#define " + option + " " + value)
            else:
                if value.startswith("y") and len(value) < 2:
                    header_lines.append("#define " + option + " 1")
                else:
                    header_lines.append("#define " + option + " \"" + value + "\"")
        else:
            header_lines.append(line)
    header_lines.append("")
    header_lines.append("#endif")

    with open(header_file, 'w') as f:
        f.write("\n".join(header_lines))

if __name__ == "__main__":
    config_file = ".config"
    header_file = "include/config.h"
    config_gen(config_file, header_file)

