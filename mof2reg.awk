BEGIN {
    split(CLASSLIST, classes);
    for (i in classes)
    {
        IMPLEMENTED[classes[i]] = 1;
    }
}

$1 == "class" && IMPLEMENTED[$2] {
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
    association = 0;
    indication = 0;
    in_interop = 0;
    in_root = 0;
    next;
}

/ASSOCIATION/ { association = 1; }
/INDICATION/ { indication = 1; }

/#if.*defined\(INTEROPNS\)/ { in_interop = 1; }
/#if.*defined\(ROOTNS\)/ { in_root = 1; }
    
