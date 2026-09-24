#!/usr/bin/env python3

import os
import re
import sys
import hashlib
import colorsys


# ============================================================
# Source file discovery
# ============================================================

def find_source_files(root):
    source_files = []

    for directory, _, files in os.walk(root):
        for filename in files:
            if filename.endswith((".c", ".asm")):
                source_files.append(
                    os.path.join(directory, filename)
                )

    return sorted(source_files)


# ============================================================
# C parsing
# ============================================================

C_FUNCTION_RE = re.compile(
    r"""
    ^\s*
    (?:[A-Za-z_]\w*[\s\*]+)+
    ([A-Za-z_]\w*)
    \s*
    \([^;]*\)
    \s*
    \{
    """,
    re.MULTILINE | re.VERBOSE,
)


C_CALL_RE = re.compile(
    r"\b([A-Za-z_]\w*)\s*\("
)


C_KEYWORDS = {
    "if",
    "for",
    "while",
    "switch",
    "return",
    "sizeof",
}


def extract_c_function_bodies(text):
    functions = {}

    for match in C_FUNCTION_RE.finditer(text):

        name = match.group(1)

        brace_start = text.find(
            "{",
            match.start()
        )

        if brace_start == -1:
            continue

        depth = 0
        position = brace_start

        while position < len(text):

            if text[position] == "{":
                depth += 1

            elif text[position] == "}":
                depth -= 1

                if depth == 0:

                    body = text[
                        brace_start + 1:position
                    ]

                    functions[name] = body
                    break

            position += 1

    return functions


def find_c_calls(body):
    """
    Return function calls in source-code order.

    Duplicate calls are removed later while preserving
    the first occurrence.
    """

    calls = []

    for match in C_CALL_RE.finditer(body):

        name = match.group(1)

        if name not in C_KEYWORDS:
            calls.append(name)

    return calls


# ============================================================
# Assembly parsing
# ============================================================

ASM_LABEL_RE = re.compile(
    r"^\s*([A-Za-z_][A-Za-z0-9_]*):",
    re.MULTILINE,
)


ASM_CALL_RE = re.compile(
    r"^\s*call\s+([A-Za-z_][A-Za-z0-9_]*)",
    re.MULTILINE | re.IGNORECASE,
)


def extract_asm_function_bodies(text):
    matches = list(
        ASM_LABEL_RE.finditer(text)
    )

    functions = {}

    for i, match in enumerate(matches):

        name = match.group(1)

        start = match.end()

        if i + 1 < len(matches):
            end = matches[i + 1].start()
        else:
            end = len(text)

        functions[name] = text[start:end]

    return functions


def find_asm_calls(body):
    """
    Return assembly CALL instructions in source order.
    """

    calls = []

    for match in ASM_CALL_RE.finditer(body):

        calls.append(
            match.group(1)
        )

    return calls


# ============================================================
# Stable function colors
# ============================================================

def function_color(name):
    """
    Generate a stable pastel color from the function name.

    The same function receives the same color every time
    the graph is regenerated.
    """

    digest = hashlib.md5(
        name.encode("utf-8")
    ).hexdigest()

    hue = (
        int(digest[:8], 16)
        / 0xFFFFFFFF
    )

    r, g, b = colorsys.hsv_to_rgb(
        hue,
        0.35,
        0.95
    )

    return "#{:02x}{:02x}{:02x}".format(
        int(r * 255),
        int(g * 255),
        int(b * 255)
    )


# ============================================================
# Build call graph
# ============================================================

def build_call_graph(root):
    source_files = find_source_files(
        root
    )

    functions = {}
    bodies = {}

    # --------------------------------------------------------
    # PASS 1
    #
    # Discover every function/routine first.
    #
    # This allows calls between functions located in
    # different source files.
    #
    # Example:
    #
    # sched.c
    #
    #     sched()
    #
    # can call:
    #
    #     dequeue()      dequeue.c
    #     enqueue()      enqueue.c
    #     pick_proc()    pick_proc.c
    # --------------------------------------------------------

    for path in source_files:

        with open(
            path,
            "r",
            encoding="utf-8",
            errors="ignore"
        ) as f:

            text = f.read()

        if path.endswith(".c"):

            file_functions = (
                extract_c_function_bodies(text)
            )

            for name, body in file_functions.items():

                functions[name] = {
                    "type": "c",
                    "file": path,
                }

                bodies[name] = body

        elif path.endswith(".asm"):

            file_functions = (
                extract_asm_function_bodies(text)
            )

            for name, body in file_functions.items():

                functions[name] = {
                    "type": "asm",
                    "file": path,
                }

                bodies[name] = body

    # --------------------------------------------------------
    # PASS 2
    #
    # Resolve calls.
    #
    # Calls are stored in lists rather than sets so their
    # original source-code order is preserved.
    #
    # Repeated calls from one caller are collapsed.
    # --------------------------------------------------------

    call_order = {}

    for caller, info in functions.items():

        body = bodies[caller]

        if info["type"] == "c":

            found_calls = find_c_calls(
                body
            )

        else:

            found_calls = find_asm_calls(
                body
            )

        ordered_calls = []
        seen = set()

        for callee in found_calls:

            # Ignore calls to functions outside the scanned
            # source directory.

            if callee not in functions:
                continue

            # Collapse repeated calls while preserving
            # the first occurrence.

            if callee in seen:
                continue

            seen.add(
                callee
            )

            ordered_calls.append(
                callee
            )

        call_order[caller] = (
            ordered_calls
        )

    return functions, call_order


# ============================================================
# Generate Graphviz DOT
# ============================================================

def generate_dot(
    functions,
    call_order,
    output_file
):
    with open(
        output_file,
        "w",
        encoding="utf-8"
    ) as f:

        f.write(
            "digraph TinyMinixCallGraph {\n\n"
        )

        # ----------------------------------------------------
        # TOP-TO-BOTTOM layout
        #
        # This is the layout that gave us the clearest
        # Tiny MINIX graph.
        # ----------------------------------------------------

        f.write(
            "    rankdir=TB;\n"
        )

        # Encourage Graphviz to preserve outgoing call order.

        f.write(
            "    ordering=out;\n"
        )

        f.write(
            "    newrank=true;\n"
        )

        f.write(
            "    compound=true;\n"
        )

        f.write(
            "    splines=true;\n"
        )

        f.write(
            "    nodesep=0.35;\n"
        )

        f.write(
            "    ranksep=0.70;\n\n"
        )

        # ----------------------------------------------------
        # Default appearance
        # ----------------------------------------------------

        f.write(
            '    node ['
            'fontname="Helvetica", '
            'fontcolor="black"];\n'
        )

        f.write(
            '    edge ['
            'fontname="Helvetica", '
            'arrowsize=0.7];\n\n'
        )

        # ----------------------------------------------------
        # Nodes
        #
        # C   -> rectangle
        # ASM -> ellipse
        # ----------------------------------------------------

        for name, info in sorted(
            functions.items()
        ):

            if info["type"] == "c":
                shape = "box"
            else:
                shape = "ellipse"

            color = function_color(
                name
            )

            label = (
                f"{name}\\n"
                f"[{info['type'].upper()}]"
            )

            f.write(
                f'    "{name}" '
                f'[label="{label}", '
                f'shape={shape}, '
                f'style=filled, '
                f'fillcolor="{color}", '
                f'color="{color}"];\n'
            )

        f.write("\n")

        # ----------------------------------------------------
        # REAL FUNCTION CALLS
        #
        # These colored arrows are the actual calls.
        #
        # Arrow color is based on the caller.
        # ----------------------------------------------------

        f.write(
            "    // Real function calls\n\n"
        )

        edge_count = 0

        for caller, callees in (
            call_order.items()
        ):

            caller_color = (
                function_color(caller)
            )

            for callee in callees:

                f.write(
                    f'    "{caller}" -> "{callee}" '
                    f'[color="{caller_color}", '
                    f'penwidth=1.5, '
                    f'weight=5];\n'
                )

                edge_count += 1

        f.write("\n")

        # ----------------------------------------------------
        # INVISIBLE SOURCE-ORDER CONSTRAINTS
        #
        # This is the important ordering mechanism.
        #
        # Suppose the source contains:
        #
        #     console_clear();
        #     kprint();
        #     prot_init();
        #     pic_init();
        #     pit_init();
        #
        # The visible edges remain:
        #
        #             kernel_main
        #            /    |    \
        #           /     |     \
        # console_clear kprint prot_init ...
        #
        # But we also add invisible constraints:
        #
        # console_clear -> kprint -> prot_init
        #               -> pic_init -> pit_init
        #
        # These invisible edges are NOT calls.
        #
        # They only tell Graphviz the preferred visual order.
        # ----------------------------------------------------

        f.write(
            "    // Invisible source-order constraints\n\n"
        )

        for caller, callees in (
            call_order.items()
        ):

            if len(callees) < 2:
                continue

            # ------------------------------------------------
            # Keep all direct callees of this caller on
            # the same horizontal rank.
            # ------------------------------------------------

            rank_nodes = " ".join(
                f'"{callee}";'
                for callee in callees
            )

            f.write(
                f"    {{ rank=same; "
                f"{rank_nodes} }}\n"
            )

            # ------------------------------------------------
            # Preserve source order from LEFT TO RIGHT.
            #
            # first -> second is invisible and exists only
            # as a layout constraint.
            # ------------------------------------------------

            for i in range(
                len(callees) - 1
            ):

                first = callees[i]
                second = callees[i + 1]

                f.write(
                    f'    "{first}" -> "{second}" '
                    f'[style=invis, '
                    f'weight=100, '
                    f'constraint=true];\n'
                )

            f.write("\n")

        f.write(
            "}\n"
        )

    return edge_count


# ============================================================
# Main
# ============================================================

def main():

    if len(sys.argv) != 2:

        print()

        print(
            "Usage:"
        )

        print(
            "  python3 tools/callgraph.py "
            "<source-directory>"
        )

        print()

        print(
            "Example:"
        )

        print(
            "  python3 tools/callgraph.py kernel"
        )

        print()

        sys.exit(1)

    root = sys.argv[1]

    if not os.path.isdir(
        root
    ):

        print(
            f"Error: directory does not exist: "
            f"{root}"
        )

        sys.exit(1)

    functions, call_order = (
        build_call_graph(root)
    )

    if not functions:

        print(
            "No C functions or ASM routines found."
        )

        sys.exit(1)

    output_file = (
        "callgraph.dot"
    )

    edge_count = generate_dot(
        functions,
        call_order,
        output_file
    )

    print()

    print(
        "Tiny MINIX Call Graph"
    )

    print(
        "---------------------"
    )

    print(
        f"Source directory : "
        f"{root}"
    )

    print(
        f"Functions found  : "
        f"{len(functions)}"
    )

    print(
        f"Calls found      : "
        f"{edge_count}"
    )

    print()

    print(
        f"Generated        : "
        f"{output_file}"
    )

    print()

    print(
        "Generate SVG with:"
    )

    print()

    print(
        "  dot -Tsvg "
        "callgraph.dot "
        "-o callgraph.svg"
    )

    print()


if __name__ == "__main__":
    main()