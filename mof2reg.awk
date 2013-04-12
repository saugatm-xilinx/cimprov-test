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
    printf "\ttype: instance method";
    if (association) printf " association";
    if (indication) printf " indication";
    printf "\n"
    print "\tgroup:", tolower(PRODUCTNAME)
    print "\tunload: never"
    print "\tnamespace:", NAMESPACE
    association = 0;
    indication = 0;
    next;
}

/\[.*Association.*\]/ { association = 1; }
/\[.*Indication.*\]/ { indication = 1; }

    
