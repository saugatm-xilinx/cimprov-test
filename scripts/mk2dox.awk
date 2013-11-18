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
    if (comment_block) {
        gsub(/\n/, "\n *", documentation);
        print "/**" documentation "\n*/"
    }
    comment_block = 1;
    standalone = 0;
    sub(/^##!/, "##"); 
    documentation = "";
    block_params = "";
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

configure_block && /^#+[[:space:]]*<\/example>[[:space:]]*$/ {
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

comment_block && /^#+[[:space:]]+[@\\]param/ {
    block_params = block_params "" (block_params ? ", " : "") $3;
}

comment_block && /^#+[[:space:]]*<example>[[:space:]]*$/ {
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

$1 == "include" {
    sub(/\$\(TOP)\//, "./", $2);
    if ($2 !~ /\$/) {
        sa = "\\sa " $2 "\n";
        if (comment_block)
            documentation = documentaton sa;
        else {
            print "/** \\file\n" sa "*/";
        }
    }
    next
}

function makename(funcname)
{
        gsub(/\./, "_DOT_", funcname);
        gsub(/%/, "_PERCENT_", funcname);        
        gsub(/\$\(/, "_VAR_", funcname);
        gsub(/\)/, "", funcname);
        gsub(/:/, "_COLON_", funcname);
        gsub(/\//, "_DIR_", funcname);
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
    if ($2 == ":" || $3 == ":") {
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
    if (configure_block) {
        print "error: <example> without closing </example>" >"/dev/stderr";
        exit 1;
    }
    
    if (comment_block) {
        if (!standalone)
            print "warning: missing entity to comment at end of file" >"/dev/stderr";
        gsub(/\n/, "\n *", documentation);
        print "/**" documentation "\n*/"
    }    
}
