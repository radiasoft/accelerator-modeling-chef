SUBDIRS = src

includedir = ${prefix}/include/parsers/xsif

include_HEADERS = include/Expression.h include/Macro.h include/XSIFFactory.h  include/XsifParserDriver.h

install-exec-local: 
	${INSTALL} -d ${prefix}/include/parsers/xsif/tree-container; \
	sed 's/[a-zA-Z0-9_]*\.hh/parsers\/xsif\/&/'  ./src/XsifParser_ypp.hh > ._temp; ${INSTALL} ._temp  ${prefix}/include/parsers/xsif/XsifParser_ypp.hh; \
	sed 's/[a-zA-Z0-9_]*\.hh/parsers\/xsif\/&/'  ./src/location.hh       > ._temp; ${INSTALL} ._temp  ${prefix}/include/parsers/xsif/location.hh ; \
	sed 's/[a-zA-Z0-9_]*\.hh/parsers\/xsif\/&/'  ./src/position.hh       > ._temp; ${INSTALL} ._temp  ${prefix}/include/parsers/xsif/position.hh ; \
	sed 's/[a-zA-Z0-9_]*\.hh/parsers\/xsif\/&/'  ./src/stack.hh          > ._temp; ${INSTALL} ._temp  ${prefix}/include/parsers/xsif/stack.hh;\
        ${INSTALL} ${top_srcdir}/include/tree-container/associative_tree.h  	 ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/associative_tree.inl     ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/basic_tree.h             ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/basic_tree.inl           ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/child_iterator.h         ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/child_iterator.inl       ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/descendant_iterator.h    ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/descendant_iterator.inl  ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/multitree.h              ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/multitree.inl            ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/ordered_iterator.h       ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/ordered_iterator.inl     ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/sequential_tree.h        ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/sequential_tree.inl      ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/tree.h                   ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/tree.inl                 ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/unique_tree.h            ${prefix}/include/parsers/xsif/tree-container; \
        ${INSTALL} ${top_srcdir}/include/tree-container/unique_tree.inl          ${prefix}/include/parsers/xsif/tree-container; \
        if [ -f ._temp ]; then rm ._temp; fi

install-data-hook:
	sed 's/[a-zA-Z0-9_]*\.hh/parsers\/xsif\/&/'  ${prefix}/include/parsers/xsif/Expression.h > ._temp;\
	${INSTALL} ._temp  ${prefix}/include/parsers/xsif/Expression.h; \
	if [ -f ._temp ]; then rm ._temp; fi
