#!/usr/bin/env python3

import os
import re
import sys
import hashlib
import colorsys


# ------------------------------------------------------------
# Source file discovery
# ------------------------------------------------------------

def find_source_files(root):
    source_files = []

    for directory, _, files in os.walk(root):
        for filename in files:
            if filename.endswith((".c", ".asm")):
                source_files.append(
                    os.path.join(directory, filename)
                )

    return source_files


# ------------------------------------------------------------
# C parsing
# ------------------------------------------------------------

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
    """
    Find C functions and return:

        {
            "function_name": "function body..."
        }

    This is intentionally a simple parser suitable for the
    current Tiny MINIX source.
    """

    functions = {}

    for match in C_FUNCTION_RE.finditer(text):
        name = match.group(1)

        brace_start = text.find("{", match.start())

        depth = 0
        position = brace_start

        while position < len(text):

            if text[position] == "{":
                depth += 1

            elif text[position] == "}":
                depth -= 1

                if depth == 0:
                    body = text[brace_start + 1:position]
                    functions[name] = body
                    break

            position += 1

    return functions


def find_c_calls(body):
    calls = set()

    for match in C_CALL_RE.finditer(body):
        name = match.group(1)

        if name not in C_KEYWORDS:
            calls.add(name)

    return calls


# ------------------------------------------------------------
# Assembly parsing
# ------------------------------------------------------------

ASM_LABEL_RE = re.compile(
    r"^\s*([A-Za-z_][A-Za-z0-9_]*):",
    re.MULTILINE,
)


ASM_CALL_RE = re.compile(
    r"^\s*call\s+([A-Za-z_][A-Za-z0-9_]*)",
    re.MULTILINE | re.IGNORECASE,
)


def extract_asm_function_bodies(text):
    """
    Treat each normal assembly label as the beginning
    of an assembly routine.

    Local labels beginning with '.' are ignored by the
    regular expression.
    """

    matches = list(ASM_LABEL_RE.finditer(text))

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
    return {
        match.group(1)
        for match in ASM_CALL_RE.finditer(body)
    }


# ------------------------------------------------------------
# Function colors
# ------------------------------------------------------------

def function_color(name):
    """
    Generate a stable pastel color from the function name.

    The color is deterministic:
    the same function name always receives the same color.

    This is preferable to truly random colors because the
    graph keeps its visual identity every time it is rebuilt.
    """

    digest = hashlib.md5(
        name.encode("utf-8")
    ).hexdigest()

    #
    # Convert part of the hash into a hue from 0.0 to 1.0.
    #
    hue = int(digest[:8], 16) / 0xFFFFFFFF

    #
    # Moderate saturation + high brightness gives us
    # readable pastel colors with black text.
    #
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


# ------------------------------------------------------------
# Build call graph
# ------------------------------------------------------------

def build_call_graph(root):

    source_files = find_source_files(root)

    functions = {}
    bodies = {}

    #
    # Pass 1:
    #
    # Discover all C functions and assembly routines.
    #
    for path in source_files:

        with open(
            path,
            "r",
            encoding="utf-8",
            errors="ignore"
        ) as f:
            text = f.read()

        if path.endswith(".c"):

            file_functions = extract_c_function_bodies(text)

            for name, body in file_functions.items():

                functions[name] = {
                    "type": "c",
                    "file": path,
                }

                bodies[name] = body

        elif path.endswith(".asm"):

            file_functions = extract_asm_function_bodies(text)

            for name, body in file_functions.items():

                functions[name] = {
                    "type": "asm",
                    "file": path,
                }

                bodies[name] = body

    #
    # Pass 2:
    #
    # Look inside each discovered function and find
    # direct calls to other discovered functions.
    #
    edges = set()

    for caller, info in functions.items():

        body = bodies[caller]

        if info["type"] == "c":
            calls = find_c_calls(body)
        else:
            calls = find_asm_calls(body)

        for callee in calls:

            #
            # Only draw calls where the target function
            # exists somewhere in our source tree.
            #
            if callee in functions:
                edges.add(
                    (caller, callee)
                )

    return functions, edges


# ------------------------------------------------------------
# Generate Graphviz DOT
# ------------------------------------------------------------

def generate_dot(functions, edges, output_file):

    with open(
        output_file,
        "w",
        encoding="utf-8"
    ) as f:

        f.write(
            "digraph TinyMinixCallGraph {\n\n"
        )

        #
        # Draw from left to right.
        #
        f.write("    rankdir=LR;\n")

        #
        # Default appearance.
        #
        f.write(
            '    node [fontname="Helvetica", '
            'fontcolor="black"];\n'
        )

        f.write(
            '    edge [fontname="Helvetica"];\n\n'
        )

        # ----------------------------------------------------
        # Nodes
        # ----------------------------------------------------

        for name, info in sorted(functions.items()):

            #
            # C functions are boxes.
            # Assembly routines are ellipses.
            #
            if info["type"] == "c":
                shape = "box"
            else:
                shape = "ellipse"

            color = function_color(name)

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
        # Edges
        # ----------------------------------------------------

        for caller, callee in sorted(edges):

            #
            # An outgoing arrow receives the color
            # of the function that makes the call.
            #
            color = function_color(caller)

            f.write(
                f'    "{caller}" -> "{callee}" '
                f'[color="{color}", '
                f'penwidth=1.5];\n'
            )

        f.write("\n}\n")


# ------------------------------------------------------------
# Main
# ------------------------------------------------------------

def main():

    if len(sys.argv) != 2:

        print(
            "Usage: "
            "python3 callgraph.py <source-directory>"
        )

        sys.exit(1)

    root = sys.argv[1]

    if not os.path.isdir(root):

        print(
            f"Error: directory does not exist: {root}"
        )

        sys.exit(1)

    functions, edges = build_call_graph(root)

    output_file = "callgraph.dot"

    generate_dot(
        functions,
        edges,
        output_file
    )

    print()
    print("Tiny MINIX Call Graph")
    print("---------------------")
    print(f"Source directory : {root}")
    print(f"Functions found  : {len(functions)}")
    print(f"Calls found      : {len(edges)}")
    print()
    print(f"Generated: {output_file}")
    print()
    print(
        "Generate SVG with:"
    )
    print(
        "dot -Tsvg callgraph.dot -o callgraph.svg"
    )
    print()


if __name__ == "__main__":
    main()