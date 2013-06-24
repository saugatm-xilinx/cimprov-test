BEGIN {
    split(CLASSLIST, classes);
    for (i in classes)
    {
        IMPLEMENTED[classes[i]] = 1;
    }
    if_level = 0;
    in_interop_if_level = 0;
    in_root_if_level = 0;
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
    next;
}

/ASSOCIATION/ { association = 1; }
/INDICATION/ { indication = 1; }

/#if/ { if_level++; } 
/#endif/ {
  if_level--;
    if (in_interop_if_level > if_level)
        in_interop = 0;
    if (in_root_if_level > if_level)
        in_root = 0;
} 
/#if.*defined\(INTEROPNS\)/ { in_interop = 1; in_interop_if_level = if_level; }
/#if.*defined\(ROOTNS\)/ { in_root = 1; in_root_if_level = if_level; }
