#! /usr/bin/gawk -f 
/^##!/ {
    comment_block = 1;
    standalone = /[@\\](file|page)/;
    block_params = "";
    sub(/^##!/, ""); 
    documentation = $0 "\n"
    next
}

comment_block {
    $0 = gensub(/<L5_PRIVATE[[:space:]]+([^>]*)>/, "\\\\L5_PRIVATE{\\\\\\1}", "g");
    $0 = gensub(/<\/L5_PRIVATE>/, "\\\\endL5_PRIVATE", "g");
}


comment_block && /^#+[[:space:]]+[@\\](file|page)/ {
    standalone = 1;
}

comment_block && /^#+[[:space:]]+[@\\]param/ {
    block_params = block_params "" (block_params ? ", " : "") $3;
}


comment_block && (/^#/ || (!standalone && /^[[:space:]]*$/)) {
    sub(/^#+/, "");
    documentation = documentation $0 "\n"
    next
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
        title = /%/ ? $0 : $1;
        id = title;
        gsub(/[^a-zA-Z0-9-]/, "-0-", id);
        documentation = "\\page target-" id " (Target " title ")\n *" documentation;
    } else if ($2 ~ /^[?+:]?=$/) {
        name = $1;
    } else if (($1 == "override" || $1 == "export") && ($3 ~ /^[?+:]?=$/)) {
        documentation = "\\internal " documentation;
        name = $2;
    } else if ($1 == "define") {
        name = $2;
    }
    print "/**" documentation "\n*/"
    if (name)
    {
        print "#define", name "(" block_params ")"
    }
}




    