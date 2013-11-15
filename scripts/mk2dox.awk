#! /usr/bin/gawk -f 

/\\$/ {
    sub(/\\$/, "");
    while ((getline cont) > 0) {
        $0 = $0 cont;
        if (/\\$/) {
            sub(/\\$/, "");
        } else {
            break
        }
        
    }
}
    

/^##!/ {
    comment_block = 1;
    standalone = /([@\\](file|page|name|dir|defgroup|addtogroup|weakgroup|mainpage)\>|@[{}])/;
    block_params = "";
    sub(/^##!/, ""); 
    documentation = $0 "\n"
    next
}

configure_block && (($1 == "export" || $1 == "override") && ($3 ~ /[?+:]?=/)) {
    sub(/^(export|override)/, "");
}

configure_block && ($2 ~ /^[?+:]?=$/) {
    gsub(/[$<>%"]/, "\\\\&", $1);
    documentation = documentation "\n<dt>" $1 "</dt><dd><code>";
    for (i = 3; i <= NF; i++) {
        gsub(/[$<>%"]/, "\\\\&", $i);
        documentation = documentation " " $i;
    }
    documentation = documentation "</code></dd>\n";
    next
}

configure_block && /^#+[[:space:]]*[@\\]endconfig[[:space:]]*$/ {
    documentation = documentation "\n</dl>\n";
    configure_block = 0;
    comment_block = 1;
    next;
}

comment_block {
    $0 = gensub(/<L5_PRIVATE[[:space:]]+([^>]*)>/, "\\\\L5_PRIVATE{\\\\\\1}", "g");
    $0 = gensub(/<\/L5_PRIVATE>/, "\\\\endL5_PRIVATE", "g");
}


comment_block && /^#+[[:space:]]*([@\\](file|page|name|dir|defgroup|addtogroup|weakgroup|mainpage)\>|@[{}])/ {
    standalone = 1;
}

comment_block && /^#+[[:space:]]*[@\\]param/ {
    block_params = block_params "" (block_params ? ", " : "") $3;
}

comment_block && /^#+[[:space:]]*[@\\]config[[:space:]]*$/ {
    comment_block = 0;
    configure_block = 1;
    documentation = documentation "\n<dl>\n";
    next
}   

comment_block && (/^#/ || (!standalone && /^[[:space:]]*$/)) {
    sub(/^#+/, "");
    documentation = documentation $0 "\n"
    next
}


function makename(funcname)
{
        gsub(/\./, "_DOT_", funcname);
        gsub(/%/, "_PERCENT_", funcname);        
        gsub(/\$\(/, "_VAR_", funcname);
        gsub(/\)/, "", funcname);
        gsub(/:/, "_COLON_", funcname);
        gsub(/[ -]/, "_", funcname);
        return funcname;
}

comment_block {
    comment_block = 0;
    gsub(/\n/, "\n *", documentation);
    if (standalone) {
        print "/**" documentation "\n*/"
        next
    }
    
    name = "";
    if ($2 == ":") {
        target = /%/ ? $0 : $1;
        funcname = makename(target);
        if (target != funcname) 
        {
            gsub(/[$<>%"]/, "\\\\&", target);
            documentation = documentation "\n<CODE>" target "</CODE>" ;
        }
    } else if ($2 ~ /^[?+:]?=$/) {
        name = $1;
    } else if (($1 == "override" || $1 == "export") && ($3 ~ /^[?+:]?=$/)) {
        documentation = "\\internal " documentation;
        name = $2;
    } else if ($1 == "define" || $1 == "ifdef" || $1 == "ifndef") {
        name = $2;
    } else if (($1 == "ifeq" || $1 == "ifneq") && ($2 ~ /^\(\$\(([^)]+)\),/)) {
        name = gensub(/^\(\$\(([^)]+)\).*$/, "\\1", "g", $2);
    }
    print "/**" documentation "\n*/"
    if (name)
    {
        print "#define", name "(" block_params ")"
    } else {
        print "void ", funcname, "() {"
        for (i = 3; i <= NF; i++) 
        {
            if ($i !~ /[%:]/)
                print makename($i) "();"
        }
        print "}";
    }
    
}
    
EOF {
    if (comment_block) {
        if (!standalone)
            print "warning: missing entity to comment at end of file" >"/dev/stderr";
        print "/**" documentation "\n*/"
    }    
}
