# RTF decomposer

Public tool for bulk extracting various embedded object types from a given RTF file or a whole folder repository.


## Presentation :

I've decided to release this tool to the public in order to assist fellow Malware analysts & Antivirus engineers at their daily tasks while dealing whith RTF objects. As you perfectly know, manual extraction of <b>RTF embedded data</b> is such a painful process. As for the other available public alternatives, without taking anything away from the excellent work done by their respective authors, I honestly think that some are simply very outdated and thus unable to cope with obfuscated data and the rest are either limited by a bad design or by some kind of hacky implementations. I don't claim that my tool is perfect either, its just something different, its <b>supposed to succeed</b> where most of the other available public alternatives fail.

I am really sorry but <b>RTF Decomposer</b> is not open source because the used code is just an extremely <b>stripped down</b> version taken from <b>Tornado Antivirus</b> unpacking framework's code base.


## Usage :


![](pictures/rtf_decomp.png)

Command line options are pretty straightforward. If you omit to specify a <b>destination folder</b> using the <b>-t</b> switch, then the extracted data will be placed inside the tool's executable folder. Please note that the tool doesn't create folders, thus the specified destination folder has to be created beforehand. 

The <b>-l</b> switch enables error logging. All encountered errors while extracting <b>RTF</b> embedded data will be then logged. In case of bugs or data extraction's error please feel free to contact me so I can release a fix.

Please note that extracted data's file names follow this naming scheme : <b>{original_file_name}_{extracted data_type}</b> where :

<b>_OLE</b> : OLE1 native data.

<b>_OVL</b> : RTF Overlay data.

<b>_STG</b> : RTF Storage data.


etc..

## Examples :

![](pictures/rtf_decomp_result.png)


