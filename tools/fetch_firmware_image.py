#!/usr/bin/python

"""
This script copies bootrom, uefirom and firmware(medford onwards)
images as mentioned in the input firmwareFamilyVersion's ivy.xml file
to the output directory mentioned.
The output directory consists the following structure -
       <output Dir>/firmware/mcfw
       <output Dir>/firmware/bootrom
       <output Dir>/firmware/uefirom

Additionally generates a JSON file with the input name<jsonfile>
in the output directory
       <output Dir>/fimware/<jsonFile>

The script copies the encode.py from v5 repository
v5/scripts/encode.py and uses it to create .dat bootom files
The script has been tested on Linux machines and is expected to
be executed in Linux environment.
"""

import json
import os
import sys
import optparse
import shutil
import xml.etree.ElementTree as xmlparser

class FirmwareImageDetails(object):
    """ Class fetches the firmware variants details"""
    def __init__(self):
        self.image_type = {'mcfw':3, 'bootrom':2, 'uefirom':12}
        self.mcfw_subtype = {'sparta':31, 'thebes':32, 'icarus':33,
                             'jericho':34, 'byblos':35, 'groat':36,
                             'shilling':37, 'florin':38, 'threepence':39,
                             'cyclops':40, 'penny':41, 'bob':42,
                             'hog':43, 'sovereign':44, 'solidus':45}
        self.bootrom_subtype = {'medford':6, 'medford2':7}
        self.uefirom_subtype = {'universal':0}
        self.firmware_list = ['mcfw-sparta', 'mcfw-thebes', 'mcfw-icarus',
                              'mcfw-jericho', 'mcfw-byblos', 'mcfw-cyclops',
                              'mcfw-shilling', 'mcfw-florin', 'mcfw-bob',
                              'mcfw-hog', 'mcfw-sovereign', 'mcfw-solidus',
                              'edk2', 'gpxe']


    def get_image_type(self, image_type_arg):
        """ Function returns the image type number """
        try:
            return self.image_type[image_type_arg]
        except KeyError:
            print "Unknown Firmware Image type"
            raise

    def get_mcfw_subtype(self, mcfw_type_arg):
        """ Function returns the mcfw sub type number """
        try:
            return self.mcfw_subtype[mcfw_type_arg]
        except KeyError:
            print "Unknown MCFW sub type"
            raise

    def get_bootrom_subtype(self, boot_type_arg):
        """ Function returns the bootrom sub type number """
        try:
            return self.bootrom_subtype[boot_type_arg]
        except KeyError:
            print "Unknown bootrom sub type"
            raise

    def get_uefirom_subtype(self, uefi_type_arg):
        """ Function returns the uefirom sub type number """
        try:
            return self.uefirom_subtype[uefi_type_arg]
        except KeyError:
            print "Unknown uefirom sub type"
            raise

    def check_firmware_list(self, name):
        """ Function checks if the image type is present in
            firmware list """
        return  name in self.firmware_list


class ImageOutputDir(object):
    """ Class stores the details of firmware variants """
    ivy_base_dir = "/project/ivy/solarflare/"
    encode_file_name = 'encode.py'

    def __init__(self, dir_arg):
        self.ivy_file = "ivys/ivy.xml"
        self.output_dir = dir_arg
        self.base_output_dir = dir_arg
        self.mcfw_dir = None
        self.uefi_dir = None
        self.boot_dir = None

    def check_output_dir(self):
        """ Checks if the output directory exists. Prompts user for overwriting
            or creates the sub directrory"""
        try:
            if not os.path.exists(self.output_dir):
                print "Output Directory doesn't exists"
                return False
            modified_dir = os.path.join(self.output_dir, "firmware")
            if os.path.exists(modified_dir):
                print "Firmware sub directory present in:" + self.output_dir
                overwrite = raw_input("Do you want to Overwrite it ?[y/n]")
                if not overwrite == 'y':
                    print "ERROR: Cannot Overwrite " + modified_dir + " Exiting"
                    return False
                else:
                    self.output_dir = os.path.join(self.output_dir, "firmware")
            else:
                self.output_dir = os.path.join(self.output_dir, "firmware")
                os.mkdir(self.output_dir)
            return True
        except OSError:
            print "Fail to create output directory"
            raise

    def check_output_subdir(self):
        """ Checks if the output directory structure exists else creates
            them """
        try:
            self.mcfw_dir = os.path.join(self.output_dir, "mcfw")
            if not os.path.exists(self.mcfw_dir):
                os.mkdir(self.mcfw_dir)

            self.uefi_dir = os.path.join(self.output_dir, "uefirom")
            if not os.path.exists(self.uefi_dir):
                os.mkdir(self.uefi_dir)

            self.boot_dir = os.path.join(self.output_dir, "bootrom")
            if not os.path.exists(self.boot_dir):
                os.mkdir(self.boot_dir)
        except OSError:
            print "Fail to create output sub directory"
            raise


class JsonParsing(object):
    """ Class to encode json file """
    json_file_name = None
    create_json_file = 0
    mcfw_list = []
    bootrom_list = []
    uefirom_list = []

    def __init__(self, file_name, flag):
        self.create_json_file = flag
        self.json_file_name = file_name

    def check_json_file(self, outdir_handle):
        """ Checks if the output Json file exists, if yes removes it
            and creates it"""
        try:
            self.json_file_name = os.path.join(outdir_handle.output_dir,
                                               self.json_file_name)
            if os.path.exists(self.json_file_name):
                print self.json_file_name + " file exists. Over writing it"
                os.remove(self.json_file_name)
            filehandle = os.open(self.json_file_name, os.O_CREAT)
            os.close(filehandle)
        except OSError:
            print "Fail to check output json file"
            raise
        except IOError:
            print "Fail to create output json file"
            raise

    def create_image_metadata(self, nameval, firmwaretype, subtype,
                              versionstring, path):
        """ Returns a dictionary object containing an image metadata """
        imagedata = {'name':nameval, 'type':firmwaretype, 'subtype':subtype,
                     'versionString':versionstring, 'path':path}
        return imagedata

    def create_json_object(self, json_obj, firmware_type):
        """ Creates a Json Object with Image details and adds to image type
            file list """
        if firmware_type == 'mcfw':
            self.mcfw_list.append(json_obj)
        elif firmware_type == 'bootrom':
            self.bootrom_list.append(json_obj)
        elif firmware_type == 'uefirom':
            self.uefirom_list.append(json_obj)

    def writejsonfile(self):
        """ Creates the final JSON file """
        try:
            mcfw_file_dict = {'files' : self.mcfw_list}
            boot_file_dict = {'files' : self.bootrom_list}
            uefi_file_dict = {'files' : self.uefirom_list}
            json_file_dict = {'controller' : mcfw_file_dict,
                              'bootROM' : boot_file_dict,
                              'uefiROM' : uefi_file_dict}
            with open(self.json_file_name, 'a') as filehandle:
                json.dump(json_file_dict, filehandle)
        except:
            print "Fail to write output json file"
            raise


def fail(msg):
    """ Function prints the error message and exits """
    print >>sys.stderr, msg
    sys.exit(1)

def get_mc_uefi_file(name, rev, firmware_img_obj, outdir_handle, json_handle):
    """ Gets MCFW and UEFIROM files from version specified in ivy.xml
        and copies them to output directory. Also creates a Json object
        of the image metadata """
    try:
        ivydir = ImageOutputDir.ivy_base_dir + name + '/default/' + rev + '/bins/'
        for updatefile in os.listdir(ivydir):
            if updatefile.endswith('dat'):
                if name.find("mcfw") > -1:
                    subtype_index = name.find('-')
                    if subtype_index == -1:
                        fail('Cannot determine firmware subtype')
                    else:
                        subtype_index += 1
                    controller_subtype = name[subtype_index:]
                    newfilename = controller_subtype.upper() + '.dat'
                    srcfilepath = os.path.join(ivydir, updatefile)
                    destfilepath = os.path.join(outdir_handle.mcfw_dir,
                                                newfilename)
                    shutil.copy(srcfilepath, destfilepath)
                    print "Adding File: " + newfilename
                    if json_handle.create_json_file == 1:
                        basedirlen = len(outdir_handle.base_output_dir)
                        temppath = destfilepath[basedirlen:]
                        typeval = firmware_img_obj.get_image_type("mcfw")
                        subtype = firmware_img_obj.get_mcfw_subtype(controller_subtype)
                        jsonobj = json_handle.create_image_metadata(newfilename,
                                                                    typeval,
                                                                    subtype,
                                                                    rev,
                                                                    temppath)
                        json_handle.create_json_object(jsonobj, "mcfw")
                else:
                    newfilename = updatefile
                    srcfilepath = os.path.join(ivydir, updatefile)
                    destfilepath = os.path.join(outdir_handle.uefi_dir,
                                                updatefile)
                    shutil.copy(srcfilepath, destfilepath)
                    print "Adding File: " + newfilename
                    if json_handle.create_json_file == 1:
                        basedirlen = len(outdir_handle.base_output_dir)
                        temppath = destfilepath[basedirlen:]
                        typeval = firmware_img_obj.get_image_type("uefirom")
                        subtype = firmware_img_obj.get_uefirom_subtype('universal')
                        jsonobj = json_handle.create_image_metadata(newfilename,
                                                                    typeval,
                                                                    subtype,
                                                                    rev,
                                                                    temppath)
                        json_handle.create_json_object(jsonobj, "uefirom")
    except KeyError:
        fail("Fail to get Image details. Exiting")
    except OSError:
        if name.find("mcfw") > -1:
            fail("Fail to generate mcfw dat Image. Exiting")
        else:
            fail("Fail to generate uefirom dat Image. Exiting")

def get_bootrom_file(name, rev, firmware_img_obj, outdir_handle, json_handle):
    """ Gets BOOTROM files from version specified in ivy.xml and
        copies them to output directory. Also creates a Json object of
        the image metadata. Support added for both medford and medford2
        boards """
    try:
        ivydir = ImageOutputDir.ivy_base_dir + name + '/default/' + rev + '/bins/'
        medford_bootrom_file = 'SFC9220.mrom'
        medford_ini_file = 'SFC9220.mrom.ini'
        medford_bootrom_dat_file = 'SFC9220.dat'
        medford2_bootrom_file = 'SFC9250.mrom'
        medford2_ini_file = 'SFC9250.mrom.ini'
        medford2_bootrom_dat_file = 'SFC9250.dat'
        for updatefile in os.listdir(ivydir):
            if (updatefile.endswith(medford_bootrom_file)or
                    updatefile.endswith(medford2_bootrom_file)):
                inifilefound = 0
                if updatefile.endswith(medford_bootrom_file):
                    subtype = firmware_img_obj.get_bootrom_subtype('medford')
                    inifilename = medford_ini_file
                    datfilename = medford_bootrom_dat_file
                else:
                    subtype = firmware_img_obj.get_bootrom_subtype('medford2')
                    inifilename = medford2_ini_file
                    datfilename = medford2_bootrom_dat_file
                srcfilepath = os.path.join(ivydir, updatefile)
                destfilepath = os.path.join(outdir_handle.base_output_dir,
                                            updatefile)
                shutil.copy(srcfilepath, destfilepath)
                inidir = ImageOutputDir.ivy_base_dir + name + '/default/' + rev + '/inis/'
                for inifile in os.listdir(inidir):
                    if inifile.endswith(inifilename):
                        srcfilepath = os.path.join(inidir, inifile)
                        destinifilepath = os.path.join(outdir_handle.
                                                       base_output_dir, inifile)
                        shutil.copy(srcfilepath, destinifilepath)
                        inifilefound = 1
                        break
                if inifilefound == 1:
                    bootromfile = os.path.join(outdir_handle.boot_dir, datfilename)
                    encodefilepath = os.path.join(outdir_handle.base_output_dir,
                                                  ImageOutputDir.encode_file_name)
                    ret_val = os.system("python "+ encodefilepath + " " +
                              destfilepath + " " + bootromfile)
                    if ret_val != 0:
                        fail("Unable to crate bootrom file. Exiting.")
                    print "Adding File: " + datfilename
                    if json_handle.create_json_file == 1:
                        basedirlen = len(outdir_handle.base_output_dir)
                        temppath = bootromfile[basedirlen:]
                        typeval = firmware_img_obj.get_image_type("bootrom")
                        jsonobj = json_handle.create_image_metadata(datfilename,
                                                                    typeval,
                                                                    subtype,
                                                                    rev,
                                                                    temppath)
                        json_handle.create_json_object(jsonobj, "bootrom")
                    os.remove(destfilepath)
                    os.remove(destinifilepath)
                else:
                    fail("Cannot Create BOOTROM dat file")
    except KeyError:
        fail("Fail to get Bootrom Image details. Exiting")
    except OSError:
        fail("Fail to generate Bootrom dat Image. Exiting")


def main():
    """ Starting point of script execution. Parses the inputs
        calls functions to copy images and generate json file. """
    try:
        ivy_family_dir = "firmwarefamily/default"
        encode_file_dir = '/hg/incoming/v5/rawfile/default/scripts/'
        encode_file_mc = 'http://source.uk.solarflarecom.com'
        encode_file_url = encode_file_mc + encode_file_dir
        encode_file_url = encode_file_url + ImageOutputDir.encode_file_name
        firmware_img_obj = FirmwareImageDetails()
        parser = optparse.OptionParser(usage=
                                       """Usage: %prog [options]
                                          output_directory
                                          firmware_family_version""",
                                       version="%prog 1.0")
        parser.add_option("-f", "--file", dest="json_file_name",
                          help="Creates Json file with Image details")
        options, args = parser.parse_args()
        if len(args) < 2:
            fail("You must specify output directory & fimrware family version")
        outdir_handle = ImageOutputDir(args[0])
        if not outdir_handle.check_output_dir():
            fail('')
        outdir_handle.check_output_subdir()
        input_ivy_dir = args[1]
        if not input_ivy_dir.startswith('v'):
            input_ivy_dir = 'v' + input_ivy_dir
        if options.json_file_name is None:
            print "Not Creating Json File"
            json_handle = JsonParsing(None, 0)
        else:
            json_handle = JsonParsing(options.json_file_name, 1)
            json_handle.check_json_file(outdir_handle)
        ivydir = ImageOutputDir.ivy_base_dir + ivy_family_dir
        input_ivy_dir = os.path.join(ivydir, input_ivy_dir)
        outdir_handle.ivy_file = os.path.join(input_ivy_dir, outdir_handle.ivy_file)
        if (not os.path.isfile(outdir_handle.ivy_file) or
            not os.access(outdir_handle.ivy_file, os.R_OK)):
            fail("Input Ivy File :"+ outdir_handle.ivy_file +" not acessible.")
       #getting encode.py from v5 repo
        encodefilepath = os.path.join(outdir_handle.base_output_dir,
                                      ImageOutputDir.encode_file_name)
        if not os.path.exists(encodefilepath):
            ret_val = os.system('wget -q -P ' + outdir_handle.base_output_dir
                      + ' ' + encode_file_url)
            if ret_val != 0:
                fail("Unable to get encode file. Exiting.")
        tree = xmlparser.parse(outdir_handle.ivy_file)
        root_node = tree.getroot()
        for child in root_node:
            if child.tag == "dependencies":
                for dependency in child.getchildren():
                    if firmware_img_obj.check_firmware_list(dependency.
                                                            get('name')):
                        if((dependency.get('name')).find('mcfw') > -1 or
                           (dependency.get('name')).find('edk2') > -1):
                            get_mc_uefi_file(dependency.get('name'),
                                             dependency.get('rev'),
                                             firmware_img_obj,
                                             outdir_handle,
                                             json_handle)
                        else:
                            get_bootrom_file(dependency.get('name'),
                                             dependency.get('rev'),
                                             firmware_img_obj,
                                             outdir_handle,
                                             json_handle)
        os.remove(encodefilepath)
        if json_handle.create_json_file == 1:
            json_handle.writejsonfile()
    except KeyError:
        fail("Image variant not determined. Exiting.")
    except OSError:
        fail("OS Environment error. Exiting.")
    except IOError:
        fail("Image File creation failed. Exiting.")

if __name__ == '__main__':
    main()
