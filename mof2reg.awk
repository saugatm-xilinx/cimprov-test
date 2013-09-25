BEGIN {
    split(CLASSLIST, classes);
    for (i in classes)
    {
        IMPLEMENTED[classes[i]] = 1;
    }

    if (TARGET == "pegasus") 
    {
        print "instance of PG_ProviderModule\n{"
        print "\tName = \"" PRODUCTNAME "_Module\";"
        print "\tVendor = \"" VENDORNAME "\";"
        if (INTERFACE == "cmpi")
        {
            print "\tVersion = \"2.0.0\";"
            print "\tInterfaceType = \"CMPI\";"
            print "\tInterfaceVersion = \"2.0.0\";"
        } else {
            print "\tVersion = \"2.5.0\";"
            print "\tInterfaceType = \"C++Default\";"
            print "\tInterfaceVersion = \"2.5.0\";"
        }
        print "\tLocation = \"" PRODUCTNAME "\";"
        print "};"
        print ""
    }
}

$1 == "class" && IMPLEMENTED[$2] {
    if (TARGET == "pegasus")
    {
        print "instance of PG_Provider\n{"
        print "\tProviderModuleName = \"" PRODUCTNAME "_Module\";"
        print "\tName = \"" $2 "_Provider\";"
        print "};"
        print ""
        print "instance of PG_ProviderCapabilities\n{"
        print "\tProviderModuleName = \"" PRODUCTNAME "_Module\";"
        print "\tProviderName = \"" $2 "_Provider\";"
        print "\tCapabilityID = \"1\";"
        print "\tClassName = \"" $2 "\";"
        print "\tNamespaces = {\"" NAMESPACE "\""  (in_interop ? ", \"" INTEROP_NAMESPACE "\"" : "") (in_root && ROOT_NAMESPACE ? ", \"" ROOT_NAMESPACE "\"" : "") "};"
        if (indication)
            print "\tProviderType = { 4 };"
        else if (association)
            print "\tProviderType = { 2, 3 };"
        else
            print "\tProviderType = { 2, 5 };"
        print "\tSupportedProperties = NULL; // All properties"
        print "\tSupportedMethods = NULL; // All methods"
        print "};"
        print ""
    }
    else
    {
        print "[" $2 "]"
        print "\tprovider:", $2 "_Provider"
        print "\tlocation:", PRODUCTNAME
        if (indication) printf "\ttype: indication\n";
        else
        {
            printf "\ttype: instance method";
            if (association) printf " association";
            printf "\n"
        }
        print "\tgroup:", tolower(PRODUCTNAME)
        print "\tunload: never"
        print "\tnamespace:", NAMESPACE, (in_interop ? INTEROP_NAMESPACE : ""), (in_root ? ROOT_NAMESPACE : "")
    }
        
    association = 0;
    indication = 0;
    
    next;
}

/ASSOCIATION/ { association = 1; }
/INDICATION/ { indication = 1; }
/ASSOCIATION_NWMI/ { association = 1; }
/INDICATION_NWMI/ { indication = 1; }


/\/\/ IMPNS/ { in_interop = in_root = 0 }
/\/\/ ROOT_OR_IMPNS/ { in_interop = 0; in_root = 1 }
/\/\/ INTEROP_OR_IMPNS/ { in_interop = 1; in_root = 0 }

