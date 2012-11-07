$1 == "class" {
    print "[" $2 "]"
    print "\tprovider:", $2 "_Provider"
    print "\tlocation:", PRODUCTNAME
    printf "\ttype: instance method";
    if (association) printf " association";
    if (indication) printf " indication";
    print ""
    print "\tgroup:", tolower(PRODUCTNAME)
    print "\tunload: never"
    print "\tnamespace:", NAMESPACE
    association = 0;
    indication = 0;
    next;
}

/\[.*Association.*\]/ { association = 1; }
/\[.*Indication.*\]/ { indication = 1; }

    
