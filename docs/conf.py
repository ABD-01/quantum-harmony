# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'ATP::ASL'
copyright = '2026, ABD'
author = 'Accolade Electronic Pvt. Ltd'
release = '1.0.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration


myst_fence_as_directive = ["mermaid"]

mermaid_init_config = {
    "theme": "base",
    "themeVariables": {
        "background": "#ffffff",
        "primaryColor": "#e6f3ff",
        "primaryTextColor": "#000000",
        "primaryBorderColor": "#0066cc",
        "lineColor": "#000000",
        "secondaryColor": "#f0f0f0",
        "tertiaryColor": "#cccccc",
    },
    "fontFamily": "arial",
    "fontSize": 12,
}

extensions = [
    'myst_parser',
    # 'furo50',
    'sphinx.ext.graphviz',
    'breathe',
    'sphinxcontrib.mermaid'
]

templates_path = ['_templates']
exclude_patterns = []



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'furo'
# html_css_files = ['nord.css']
html_static_path = ['_static']
html_logo = 'new_logo.png'
html_favicon = 'blue_logo.ico'
html_theme_options = {
    "light_css_variables": {
        "color-brand-primary": "#009c7c",
        "color-brand-content": "#009c7c",
        "color-brand-visited": "#009c7c",
    },
    "dark_css_variables": {
        "color-brand-primary": "#009c7c",
        "color-brand-content": "#009c7c",
        "color-brand-visited": "#009c7c",
    },
}

suppress_warnings = ["cpp.duplicate_declaration"]


# -- Extension configuration -------------------------------------------------

import subprocess
# subprocess.call('.\\make clean', shell=True)
# subprocess.call('cd ../../doxygen ; doxygen', shell=True)

breathe_projects = { "asl": "./xml/" }
breathe_default_project = "asl"
